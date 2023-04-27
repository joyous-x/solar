#pragma once
#include "common/utils/scope_guard.h"
#include "common/utils/utils.h"
#include "common/office/cfb_base.h"
#include "common/file/simplefile.h"
#include "common/file/filetype.h"
#include "common/3rds/zip/zip.h"
#include "common/3rds/md5.h"

inline
ifile* create_file(const file_obj_t* parent, const char* dir, const char* name, const char* alias, const char* mode) {
    std::string filepath = XsUtils::append_backslash(dir ? dir : ".");
    std::string filename = name ? XsUtils::unicode2ansi(XsUtils::utf2uni(name)) : "";
    if (filename.empty()) {
        filename = XsUtils::random_str();
    }
    std::string prefix = "";
    for (const file_obj_t* cur = parent; cur; cur = cur->container) {
        prefix = std::string(cur->file->alias()) + "__" + prefix;
    }

    std::replace(prefix.begin(), prefix.end(), '/', '.');
    std::replace(prefix.begin(), prefix.end(), '\\', '.');
    std::replace(filename.begin(), filename.end(), '/', '.');
    std::replace(filename.begin(), filename.end(), '\\', '.');

    SimpleFile* bqFile = SimpleFile::newfile();
    if (0 != bqFile->open((filepath + prefix + filename).c_str(), alias ? alias : filename.c_str(), mode ? mode : "wb+")) {
        bqFile->release();
        bqFile = nullptr;
    }
    return bqFile;
}

class SimpleLogger : public iloger {
public:
    virtual void debug(const char* format, ...) {
        va_list valist;
        va_start(valist, format);
        _message(0, format, valist);
        va_end(valist);
    }
    virtual void warn(const char* format, ...) {
        va_list valist;
        va_start(valist, format);
        _message(3, format, valist);
        va_end(valist);
    }
    virtual void fatal(const char* format, ...) {
        va_list valist;
        va_start(valist, format);
        _message(5, format, valist);
        va_end(valist);
    }
private:
    void _message(int level, const char* format, va_list vars) {
        vprintf(format, vars);
    }
};

inline
cfb_ctx_t* new_cfb_ctx(const char* temp_dir) {
    cfb_ctx_t* ctx = new cfb_ctx_t();
    ctx->loger = new SimpleLogger();
    ctx->temp_dirpath = temp_dir;
    ctx->file_creator = create_file;
    ctx->cutoff_write_tmp = 0;

    ctx->try_decrypt = true;
    ctx->extract_mbd = true;
    ctx->extract_xlm = true;
    ctx->xlm_deobfuscation = true;
    ctx->extract_xlm_drawings = false;

    return ctx;
}

inline
void free_cfb_ctx(cfb_ctx_t*& ctx) {
    if (ctx) {
        if (ctx->loger) free(ctx->loger);
        free(ctx);
        ctx = nullptr;
    }
}

inline
std::string generate_tmp_filepath(const char* temp_dir, const char* parent_filename, const char* filename, const char* suffix) {
    std::string str_temp_dir(".");
    if (nullptr != temp_dir) {
        str_temp_dir = temp_dir;
    }

    if (str_temp_dir.at(str_temp_dir.size() - 1) != '\\' && str_temp_dir.at(str_temp_dir.size() - 1) != '/') {
#if (_WIN32 || _WIN64) 
        str_temp_dir += "\\";
#else
        str_temp_dir += "/";
#endif
    }

    std::string str_filename = filename ? filename : XsUtils::random_str();
    if (filename) {
        std::replace(str_filename.begin(), str_filename.end(), '\\', '_');
        std::replace(str_filename.begin(), str_filename.end(), '/', '_');
    }

    str_temp_dir += parent_filename ? parent_filename : "";
    str_temp_dir += "__";
    str_temp_dir += str_filename;
    if (suffix) {
        str_temp_dir += "__" + std::string(suffix);
    }
    return str_temp_dir;
}




class ZipHelper : public iziper {
public:
    ZipHelper() {}
    ~ZipHelper() {
        clean();
    }

    void clean() {
        if (keeped) { keeped->release(); keeped = nullptr; }
    }

    zip_t* get_readable(ifile* in) {
        if (in == nullptr) {
            return nullptr;
        }

        zip_t* hz_r = nullptr;
        hz_r = zip_open(in->fullpath(), ZIP_DEFAULT_COMPRESSION_LEVEL, 'r');
        if (hz_r == nullptr) {
            // only for read
            hz_r = zip_stream_open((const char*)in->mapping(), (size_t)in->size(), ZIP_DEFAULT_COMPRESSION_LEVEL, 'r');
        }

        return hz_r;
    }

    zip_t* get_writable(ifile* in) {
        if (in == nullptr) {
            return nullptr;
        }

        return zip_open(in->fullpath(), ZIP_DEFAULT_COMPRESSION_LEVEL, 'a');
    }

    int read_catalog(ifile* in) {
        if (in == nullptr) {
            return BQ::INVALID_ARG;
        }

        zip_t* hz_read = get_readable(in);
        ON_SCOPE_EXIT([&]() { if (hz_read) zip_close(hz_read); });

        int nret = BQ::OK;
        do {
            auto total_entries = zip_entries_total(hz_read);
            if (total_entries < 0) {
                nret = BQ::ERR_UNCOMPRESS;
                break;
            }

            for (auto i = 0; i < total_entries; i++) {
                // only avaiable in 'r' mode
                if (0 > zip_entry_openbyindex(hz_read, i)) {
                    continue;
                }

                auto name = zip_entry_name(hz_read);
                auto size = zip_entry_size(hz_read);
                if (name) {
                    subfiles.push_back(XsUtils::uni2utf(XsUtils::ansi2unicode(name)));
                    subfiles_size.push_back(size);
                }
            }
        } while (false);
        return nret;
    }

    virtual int32_t parse(ifile* in, const char* modes = "r") {
        int nret = read_catalog(in);
        if (nret != BQ::OK) {
            return nret;
        }
        keeped = in;
        keeped->addref();
        return nret;
    }

    virtual int32_t get_item(int32_t id, uint8_t** data, uint32_t* size) {
        if (size == nullptr || (uint32_t)id >= subfiles.size()) {
            return -1;
        }

        if (data == nullptr) {
            *size = subfiles_size[id];
            return 0;
        }

        uint32_t bufsize = subfiles_size[id];
        uint8_t* buffer = (uint8_t*)malloc(bufsize);
        if (buffer == nullptr) {
            return -2;
        }
        ON_SCOPE_EXIT([&] { if (buffer) free(buffer); });

        zip_t* hz_read = get_readable(keeped);
        ON_SCOPE_EXIT([&]() { if (hz_read) zip_close(hz_read); });

        int nret = -3;
        if (hz_read) {
            // only valid in 'r' (readonly) mode.
            nret = zip_entry_openbyindex(hz_read, id);
            if (nret != 0) {
                nret = zip_entry_open(hz_read, subfiles[id].c_str());
            }
        }
        if (nret != 0) {
            return nret;
        }
        ON_SCOPE_EXIT([&]() { zip_entry_close(hz_read); });

        nret = zip_entry_noallocread(hz_read, buffer, bufsize);
        if (nret < 0) {
            return -4;
        }

        *data = buffer;
        *size = bufsize;
        buffer = nullptr;
        return 0;
    };

    virtual int32_t add_item(int32_t id, const uint8_t* data, uint32_t size) {
        if ((uint32_t)id >= subfiles.size()) {
            return -1;
        }

        del_item(id);
        
        zip_t* hz_write = get_writable(keeped);
        ON_SCOPE_EXIT([&]() { if (hz_write) zip_close(hz_write); });

        const char* entryname = subfiles[id].c_str();
        auto nret = zip_entry_open(hz_write, entryname);
        if (nret != 0) {
            return nret;
        }
        ON_SCOPE_EXIT([&]() { zip_entry_close(hz_write); });

        nret = zip_entry_write(hz_write, data, size);
        if (nret != 0) {
            return nret;
        }
        return nret;
    };

    virtual int32_t del_item(int32_t id) {
        if ((uint32_t)id >= subfiles.size()) {
            return -1;
        }

        zip_t* hz_write = get_writable(keeped);
        ON_SCOPE_EXIT([&]() { if (hz_write) zip_close(hz_write); });

        const char* entryname = subfiles[id].c_str();
        auto nret = zip_entries_delete(hz_write, (char* const*)&entryname, 1);
        return nret == 1 ? 0 : nret;
    };

    virtual int32_t item_cnt() {
        return subfiles.size();
    }

    virtual const char* item_utf8path(int32_t id_based_zero) {
        if ((uint32_t)id_based_zero >= subfiles.size()) {
            return nullptr;
        }
        return subfiles[id_based_zero].c_str();
    }

    int deprecated_catalogs(const char** names, uint32_t* count) {
        if (count == nullptr) {
            return -1;
        } else if (names == nullptr) {
            *count = subfiles.size();
            return 0;
        } else if (*count < subfiles.size()) {
            *count = subfiles.size();
            return -2;
        }

        *count = subfiles.size();
        for (uint32_t i = 0; i < subfiles.size(); i++) {
            *(names + i) = subfiles[i].c_str();
        }
        return 0;
    }

    virtual int addref() {
        return ++m_count;
    };
    virtual int release() {
        if (m_count && --m_count == 0) { delete this; }
        return 0;
    };
protected:
    volatile int m_count{ 1 };
    ifile* keeped{ nullptr };
    std::vector<std::string> subfiles;
    std::vector<uint32_t> subfiles_size;
};
