#pragma once
#include <cassert>
#include <map>
#include <algorithm>
#include "../file/filetype.h"
#include "../utils/utils.h"
#include "../utils/scope_guard.h"
#include "../3rds/base64.h"
#include "./helper.h"
#include "./ms_activemine.h"

static const char* mine_content_location = "Content-Location";
static const char* mine_content_type = "Content-Type";
static const char* mine_content_type_charset = "charset";

typedef struct __st_mht_header {
    std::string mine_version;
    std::string content_type;
    std::string boundary;
} mht_header_t;

typedef struct __st_mht_part {
    std::string location;
    std::string transfer_encoding;
    std::string type;
    std::string type_charset;
    uint32_t offset;
    uint32_t content_offset;
    uint32_t content_size;
} mht_part_t;

typedef struct __st_mht : file_obj_t {
    mht_header_t header;
    std::vector<mht_part_t> parts;

    virtual ~__st_mht() { parts.clear(); }
protected:
    __st_mht& operator =(const __st_mht& v);
} mht_t;

class mhtml {
public:
    /*
    * check if it is a MHT file(MIME HTML, Word or Excel saved as "Single File Web Page") :
    *
    * According to tests, these files usually start with "MIME-Version: 1.0" on the first line,
    * BUT Office Word accepts a blank line or other MIME headers inserted before, and even
    * whitespaces in between "MIME", "-", "Version" and ":".The version number is ignored.
    * And the line is case insensitive.
    */
    bool is_mhtml(const char* data, uint32_t size) {
        mht_header_t header;
        return (BQ::OK == parse_mhtml_header(data, size, &header));
    }

    int parse(const char* data, uint32_t size, mht_t* mht) {
        if (data == nullptr || 0 == size || nullptr == mht) {
            return BQ::INVALID_ARG;
        }

        int nret = parse_mhtml_header(data, size, &mht->header);
        if (nret != BQ::OK) {
            return nret;
        }

        std::map<std::string, std::string> kvs;
        for (uint32_t curr = 0, next = 0, part_start = 0, part_size = 0; curr < size; curr += next) {
            nret = dig_part_boundary(data + curr, mht->header.boundary.c_str(), part_start, part_size, &next);
            if (nret != BQ::OK) {
                nret = BQ::OK;
                break;
            }
            if (part_start == 0 || part_size == 0 || part_start + part_size >= size) {
                break;
            }
            part_start += curr;

            kvs.clear();
            auto part_header_end = retrieve_kv_pairs(data + part_start, part_size, kvs);
            if (part_header_end == nullptr) {
                continue;
            }
            for (; std::isspace((uint8_t)*part_header_end) && part_header_end < data + size; part_header_end++);

            mht_part_t part{ "", "", "", "", part_start, (uint32_t)(part_header_end - data), (uint32_t)(data + part_start + part_size - part_header_end) };

            if (kvs.find(mine_content_type) != kvs.end()) {
                parse_mine_content_type(kvs.find(mine_content_type)->second.c_str(), kvs);
            }

            for (auto iter = kvs.begin(); iter != kvs.end(); iter++) {
                std::string key = iter->first;
                if (ucs::icmp(key.c_str(), mine_content_location) == 0) {
                    part.location = iter->second;
                }
                else if (ucs::icmp(key.c_str(), "Content-Transfer-Encoding") == 0) {
                    part.transfer_encoding = iter->second;
                }
                else if (ucs::icmp(key.c_str(), mine_content_type) == 0) {
                    part.type = iter->second;
                }
                else if (ucs::icmp(key.c_str(), mine_content_type_charset) == 0) {
                    part.type_charset = iter->second;
                }
            }

            mht->parts.push_back(part);
        }

        return nret;
    }

    int extract(const ctx_t* ctx, ifile* file, const file_obj_t* parent, mht_t* out, ifilehandler* cb_file) {
        if (ctx == nullptr || nullptr == file) {
            return BQ::INVALID_ARG;
        }

        mht_t tmp_mht;
        mht_t* mht = out ? out : &tmp_mht;
        if (nullptr == mht->file) {
            mht->file = file;
            mht->container = parent;
            mht->file->addref();
        }

        auto data = (const char*)file->mapping();
        auto size = (uint32_t)file->size();
        
        int nret = parse(data, size, mht);
        if (nret != BQ::OK) {
            return nret;
        }

        for (auto iter = mht->parts.begin(); iter != mht->parts.end(); iter++) {
            mht_part_t* part = &*iter;

            if (ucs::icmp(part->type.c_str(), "application/x-mso") == 0 && ucs::icmp(part->transfer_encoding.c_str(), "base64") == 0) {
                size_t raw_size = 0;
                auto raw = base64_decode((const unsigned char*)(data + part->content_offset), part->content_size, &raw_size);
                if (raw == nullptr) {
                    continue;
                }
                ON_SCOPE_EXIT([&]() { if (raw) free(raw); });

                auto name = part->location.empty() ? "" : XsUtils::split_filename(part->location.c_str());
                auto embedded = create_embedded(ctx, mht, raw, raw_size, ft_unknown, name.empty() ? 0 : name.c_str());
                if (embedded) {
                    if (cb_file && 0 > cb_file->handle(ctx, (const file_obj_t*)embedded)) {
                        nret = BQ::ERR_USER_INTERRUPT;
                    }
                    delete embedded, embedded = nullptr;
                }
                else {
                    nret = BQ::ERR_FILE_CREATOR;
                }

                if (nret != BQ::OK) {
                    break;
                }
            }
        }

        return nret;
    }

protected:
    int parse_mine_content_type(const char* content_type_val, std::map<std::string, std::string>& kvs, const char delimiter = ';') {
        if (content_type_val == nullptr) {
            return BQ::INVALID_ARG;
        }

        std::string content_type = content_type_val;
        do {
            auto semicolon_pos = content_type.find(delimiter);
            if (semicolon_pos == std::string::npos) {
                break;
            }
            std::string left = content_type.substr(semicolon_pos + 1);
            content_type = content_type.substr(0, semicolon_pos);

            std::string ct_item;
            for (auto next_semicolon = -1, equal_pos = -1; !left.empty();) {
                next_semicolon = left.find(delimiter);
                if (next_semicolon == std::string::npos) {
                    ct_item = left;
                    left = "";
                }
                else {
                    ct_item = left.substr(0, next_semicolon);
                    left = left.substr(next_semicolon + 1);
                }

                equal_pos = ct_item.find('=');
                if (equal_pos == std::string::npos) {
                    // TODO: should do something
                    continue;
                }
                auto ct_item_key = ct_item.substr(0, equal_pos);
                auto ct_item_val = ct_item.substr(equal_pos + 1);
                ucs::trim(ct_item_key);
                ucs::trim(ct_item_val, " \t\r\v\n\"");
                kvs[ct_item_key] = ct_item_val;
            }
        } while (false);

        kvs[mine_content_type] = content_type;
        return BQ::OK;
    }

    /*
    * @brief search for the start and end postions of an avaiable boundary. NO SPACE around the boundary.
    * 
    * eg. 
    *   {start}------=_NextPart_01D27265.4BF42300{size = current_postion - start}
    */
    int dig_avaiable_boundary(const char* content, const char* boundary, uint32_t& start, uint32_t& size, bool* end_reached) {
        if (content == 0 || boundary == 0) {
            return BQ::INVALID_ARG;
        }

        int nret = BQ::ERR, boundary_len = ucs::len(boundary);
        bool end_of_archive_reached = false;
        const char* part_s = nullptr, * part_e = nullptr;
        const char* text_s = content, * text_e = content + ucs::len(content);

        for (const char* text_cur = text_s, *found = nullptr; text_cur < text_e; text_cur = found + boundary_len) {
            found = strstr(text_cur, boundary);
            if (found == nullptr) {
                break;
            }

            /* check prefix */
            for (auto pre = found; pre >= text_cur; pre--) {
                if (*pre == ' ' || *pre == '-') {
                    if (pre == text_cur) {
                        part_s = pre;
                        break;
                    }
                    continue;
                }
                if (*pre == '\r' || *pre == '\n') {
                    part_s = pre + 1;
                }
                else {
                    part_s = nullptr;
                }
                break;
            }

            if (part_s == nullptr) {
                continue;
            }

            /* check suffix, post MUST BE smaller than the postion of the ending flag '\0' */
            for (auto post = found + boundary_len; post < text_e; post++) {
                if (*post == ' ') {
                    if (post + 1 == text_e) {
                        part_e = post + 1;
                        break;
                    }
                    continue;
                }
                if (*post == '-' && *(post + 1) == '-') {
                    /* ending of archive */
                    part_e = ++post + 1;
                    end_of_archive_reached = true;
                    continue;
                }
                if (*post == '\r' || *post == '\n') {
                    part_e = post;
                }
                else {
                    part_e = nullptr;
                }
                break;
            }

            if (part_e == nullptr) {
                continue;
            }

            break;
        }

        if (part_e && part_s) {
            size = part_e - part_s;
            start = part_s - content;
            if (end_reached) *end_reached = end_of_archive_reached;
            nret = BQ::OK;
        }

        return nret;
    }

    /*
    * @brief search for the boundaries of a part in the content. 
    * 
    * eg. the boundaries is not included in the start and size.
    *   ------=_NextPart_01D27265.4BF42300
    *   {start}  
    *       ... 
    *   {size = current_postion - start}
    *   (spaces)
    *   ------=_NextPart_01D27265.4BF42300
    *   {next_start} 
    *       ...
    *   ------=_NextPart_01D27265.4BF42300
    */
    int dig_part_boundary(const char* content, const char* boundary, uint32_t& start, uint32_t& size, uint32_t* next_start) {
        if (content == 0 || boundary == 0) {
            return BQ::INVALID_ARG;
        }

        bool end_reached = false;
        const char* part_s = nullptr, * part_e = nullptr;

        uint32_t part_offset = 0, part_size = 0;
        int nret = dig_avaiable_boundary(content, boundary, part_offset, part_size, &end_reached);
        if (nret != BQ::OK) {
            return nret;
        }
        if (end_reached) {
            start = size = 0;
            return nret;
        }
        part_s = content + part_offset + part_size;
        
        nret = dig_avaiable_boundary(part_s, boundary, part_offset, part_size, &end_reached);
        if (nret != BQ::OK) {
            part_e = content + ucs::len(content);
            if (next_start) { *next_start = -1; }
        }
        else {
            part_e = part_s + part_offset;
            for (; part_e - 1 >= part_s && std::isspace((uint8_t)*(part_e - 1)); part_e--);
            if (next_start) { *next_start = part_e - content; }
        }

        size = part_e - part_s;
        start = part_s - content;
        return BQ::OK;
    }

    int parse_mhtml_header(const char* data, uint32_t size, mht_header_t* header) {
        if (data == nullptr || size == 0 || nullptr == header) {
            return BQ::INVALID_ARG;
        }

        std::map<std::string, std::string> mine_header;
        auto next = retrieve_kv_pairs(data, size, mine_header);

        std::string& content_type = header->content_type;
        std::string& boundary = header->boundary;
        std::string& mine_ver = header->mine_version;
        for (auto iter = mine_header.begin(); iter != mine_header.end(); iter++) {
            if (!mine_ver.empty() && !content_type.empty()) {
                break;
            }

            std::string key = iter->first;

            if (mine_ver.empty() && ucs::nicmp(key.c_str(), "mime", 4) == 0) {
                key.erase(remove(key.begin(), key.end(), ' '), key.end());
                if (ucs::icmp(key.c_str(), "MIME-Version") == 0) {
                    mine_ver = iter->second;
                    mine_ver.erase(remove(mine_ver.begin(), mine_ver.end(), ' '), mine_ver.end());
                }
            }

            if (content_type.empty() && ucs::icmp(key.c_str(), "Content-Type") == 0) {
                content_type = iter->second;
            }
        }

        if (mine_ver != "1.0") {
            return BQ::ERR_FORMAT;
        }

        std::map<std::string, std::string> content_type_kvs;
        parse_mine_content_type(content_type.c_str(), content_type_kvs);
        for (auto iter = content_type_kvs.begin(); iter != content_type_kvs.end(); iter++) {
            if (0 != ucs::icmp(iter->first.c_str(), "boundary")) {
                continue;
            }
            boundary = iter->second;
            break;
        }

        if (mine_ver.empty() || content_type.empty() || boundary.empty()) {
            return BQ::ERR_FORMAT;
        }
        return BQ::OK;
    }

    const char* retrieve_kv_pairs(const char* data, uint32_t size, std::map<std::string, std::string>& kvs) {
        if (data == 0 || size == 0) {
            return nullptr;
        }

        const char* data_s = nullptr;
        for (data_s = data; data_s < data + size && std::isspace((uint8_t)*data_s); data_s++);
        size = std::min<uint32_t>(size - (data_s - data), rfc2821_size);

        char buffer[rfc2821_size + 1] = { 0 };
        const char* line_s = nullptr;
        const char* line_e = nullptr;
        for (auto cur = data_s, end = data + size; cur < end; cur = ++line_e) {
            for (line_s = cur; std::isspace((uint8_t)*line_s) && line_s < end; line_s++);
            for (line_e = cur; *line_e != '\r' && line_e < end; line_e++);

            if (line_s >= line_e) {
                // Empty line means end of key/value section.
                break;
            }

            memset(buffer, 0, sizeof(buffer));
            memcpy(buffer, line_s, std::min<uint32_t>(line_e - line_s, rfc2821_size));

            std::string line = buffer;
            size_t colon_index = line.find(':');
            if (line.npos == colon_index) {
                continue;
            }

            std::string key = line.substr(0, colon_index);
            std::string val = line.substr(colon_index + 1);
            ucs::trim(key);
            ucs::trim(val);
            kvs[key] = val;
        }

        return line_e;
    }

public:
    static const int rfc2821_size = 1000;
};
