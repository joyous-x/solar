#include <assert.h>
#include <set>
#include "../utils/utils.h"
#include "../utils/bitmap.h"
#include "../utils/codepage.h"
#include "ms_vba.h"
#include "ms_vba_pcode.h"
#include "rle.h"
#include "ole_utils.h"

enum emDirRecord {
    // InformationRecord
    PROJECTSYSKIND = 0x0001,
    PROJECTLCID = 0x0002,
    PROJECTLCIDINVOKE = 0x0014,
    PROJECTCODEPAGE = 0x0003,
    PROJECTNAME = 0x0004,
    PROJECTDOCSTRING = 0x0005,
    PROJECTHELPFILEPATH = 0x0006,
    PROJECTHELPCONTEXT = 0x0007,
    PROJECTLIBFLAGS = 0x0008,
    PROJECTVERSION = 0x0009,
    PROJECTCONSTANTS = 0x000C,
    // ReferencesRecord
    REFERENCENAME = 0x0016,
    REFERENCEORIGINAL = 0x0033,
    REFERENCECONTROL = 0x002F,
    REFERENCECONTROL_Reserved3 = 0x0030,
    REFERENCEREGISTERED = 0x000D,
    REFERENCEPROJECT = 0x000E,
    // ModulesRecord
    PROJECTMODULES = 0x000F,
    PROJECTCOOKIE = 0x0013,
    MODULENAME = 0x0019,
    MODULENAMEUNICODE = 0x0047,
    MODULESTREAMNAME = 0x001A,
    MODULEDOCSTRING = 0x001C,
    MODULEOFFSET = 0x0031,
    MODULEHELPCONTEXT = 0x001E,
    MODULECOOKIE = 0x002C,
    MODULETYPE_procedural = 0x0021,
    MODULETYPE_document = 0x0022,
    MODULEREADONLY = 0x0025,
    MODULEPRIVATE = 0x0028,
    PROJECTMODULES_Terminator = 0x002B,
    // dir stream end
    DirStream_Terminator = 0x0010,
};

auto get_ole_tree_item_did = [](const olefile_t* ole, entry_tree_item_t* node) -> uint32_t {
    if (ole == nullptr || ole->entry_tree == nullptr || node == nullptr) {
        return 0;
    }
    return node - ole->entry_tree;
};

int msvba::parse_dir_stream(const uint8_t* dir_stream_unpacked, uint32_t size, vba_project_props_t* props, bool ole_is_little_endian) {
    uint32_t nret = BQ::OK;
    
    const uint8_t* unpacked = dir_stream_unpacked;
    uint32_t unpacked_size = size;

    for (uint32_t pos = 0, id = 0, size = 0, end = 0; 0 == end && pos < unpacked_size; pos += size) {
        if (unpacked_size < pos + 6) {
            break;
        }
        id = XsUtils::byteorder2host<uint16_t>(*(uint16_t*)(unpacked + pos), true);
        pos += sizeof(uint16_t);
        size = XsUtils::byteorder2host<uint32_t>(*(uint32_t*)(unpacked + pos), true);
        pos += sizeof(uint32_t);

        switch (id) {
        case PROJECTSYSKIND: {
            /* 
            * SysKind 
            *   0x00000000 For 16-bit Windows Platforms.
            *   0x00000001 For 32-bit Windows Platforms.
            *   0x00000002 For Macintosh Platforms.
            *   0x00000003 For 64-bit Windows Platforms.
            */
            props->syskind = XsUtils::byteorder2host<uint32_t>(*(uint32_t*)(unpacked + pos), true);
            size = sizeof(uint32_t);
            break;
        }
        case PROJECTLCID: {
            /* Lcid */
            if (pos + sizeof(uint32_t) > unpacked_size) {
                break;
            }
            uint32_t Lcid = XsUtils::byteorder2host<uint32_t>(*(uint32_t*)(unpacked + pos), true);
            assert(Lcid == 0x00000409);
            size = sizeof(uint32_t);
            break;
        }
        case PROJECTCODEPAGE: {
            if (pos + sizeof(uint16_t) > unpacked_size) {
                break;
            }
            props->codepage = XsUtils::byteorder2host<uint16_t>(*(uint16_t*)(unpacked + pos), true);
            size = sizeof(props->codepage);
            break;
        }
        case PROJECTNAME: {
            if (size > 128 || size < 1) {
                // error, SizeOfProjectName value not in range [1-128]
            }
            break;
        }
        case PROJECTDOCSTRING: {
            if (size > 2000) {
                // error, SizeOfDocString value not in range [0,2000]
            }
            uint32_t min_bytes_size = sizeof(uint16_t) + sizeof(uint32_t);
            if (pos + size + min_bytes_size > unpacked_size) {
                break;
            }
            uint16_t reserved = XsUtils::byteorder2host<uint16_t>(*(uint16_t*)(unpacked + pos + size), true);
            uint32_t sizeof_docstring_unicode = XsUtils::byteorder2host<uint32_t>(*(uint32_t*)(unpacked + pos + size + 2), true);
            if (sizeof_docstring_unicode % 2 != 0) {
                // error, MUST be even
            }
            size += min_bytes_size + sizeof_docstring_unicode;
            break;
        }
        case PROJECTLCIDINVOKE: {
            /* LcidInvoke */
            if (pos + sizeof(uint32_t) > unpacked_size) {
                break;
            }
            uint32_t LcidInvoke = XsUtils::byteorder2host<uint32_t>(*(uint32_t*)(unpacked + pos), true);
            assert(LcidInvoke == 0x00000409);
            size = sizeof(uint32_t);
            break;
        }
        case PROJECTHELPFILEPATH: {
            if (size > 260) {
                // error, SizeOfHelpFile1 value not in range [0,260]
            }
            uint32_t min_bytes_size = sizeof(uint16_t) + sizeof(uint32_t);
            if (pos + size + min_bytes_size > unpacked_size) {
                break;
            }
            uint16_t reserved = XsUtils::byteorder2host<uint16_t>(*(uint16_t*)(unpacked + pos + size), true);
            uint32_t sizeof_helpfile2 = XsUtils::byteorder2host<uint32_t>(*(uint32_t*)(unpacked + pos + size + 2), true);
            if (sizeof_helpfile2 != size) {
                // error,  MUST be equal to SizeOfHelpFile1.
            }
            size += min_bytes_size + sizeof_helpfile2;
            break;
        }
        case PROJECTHELPCONTEXT: {
            /* HelpContext */
            size = sizeof(uint32_t);
            break;
        }
        case PROJECTLIBFLAGS: {
            /* ProjectLibFlags */
            size = sizeof(uint32_t);
            break;
        }
        case PROJECTVERSION: {
            if (pos + sizeof(uint32_t) + sizeof(uint16_t) > unpacked_size) {
                break;
            }
            /* id(2 bytes) + reserved(4 bytes) + major_version(4 bytes) + minor_version(2 bytes) */
            props->version_major = XsUtils::byteorder2host<uint32_t>(*(uint32_t*)(unpacked + pos), true);
            props->version_minor = XsUtils::byteorder2host<uint16_t>(*(uint16_t*)(unpacked + pos + 4), true);
            size = sizeof(uint32_t) + sizeof(uint16_t);
        }
        break;
        case PROJECTCONSTANTS: {
            if (size > 1015) {
                // error, SizeOfConstants value not in range [0,1015]
            }
            uint32_t min_bytes_size = sizeof(uint16_t) + sizeof(uint32_t);
            if (pos + size + min_bytes_size > unpacked_size) {
                break;
            }
            uint16_t reserved = XsUtils::byteorder2host<uint16_t>(*(uint16_t*)(unpacked + pos + size), true);
            uint32_t sizeof_constants_unicode = XsUtils::byteorder2host<uint32_t>(*(uint32_t*)(unpacked + pos + size + 2), true);
            if (sizeof_constants_unicode % 2 != 0) {
                // error, MUST be even
            }
            size += min_bytes_size + sizeof_constants_unicode;
            break;
        }
        case REFERENCENAME: {
            /*
            * Reserved (2 bytes): MUST be 0x003E. MUST be ignored.
            *   contrary to the specification I think that the unicode name
            *   is optional. if reference_reserved is not 0x003E I think it
            *   is actually the start of another REFERENCE record
            *   at least when projectsyskind_syskind == 0x02 (Macintosh)
            */
            if (pos + size + sizeof(uint16_t) > unpacked_size) {
                break;
            }
            uint16_t reserved = XsUtils::byteorder2host<uint16_t>(*(uint16_t*)(unpacked + pos + size), true);
            if (reserved == 0x003E) {
                uint32_t sizeof_name_unicode = XsUtils::byteorder2host<uint32_t>(*(uint32_t*)(unpacked + pos + size + 2), true);
                size += sizeof(uint16_t) + sizeof(uint32_t) + sizeof_name_unicode;
            }
            else {
                size += 0;
            }
            break;
        }
        case REFERENCEORIGINAL: {
            break;
        }
        case REFERENCECONTROL: {
            if (pos + sizeof(uint32_t) + sizeof(uint16_t) > unpacked_size) {
                break;
            }
            /* SizeOfLibidTwiddled (4bytes) + LibidTwiddled (variable) + Reserved1 (4bytes) + Reserved2 (2bytes) */
            uint32_t sizeof_libidtwiddled = XsUtils::byteorder2host<uint32_t>(*(uint32_t*)(unpacked + pos), true);
            size = sizeof(uint32_t) + sizeof_libidtwiddled + sizeof(uint32_t) + sizeof(uint16_t);
            break;
        }
        case REFERENCECONTROL_Reserved3: {
            if (pos + sizeof(uint32_t) + sizeof(uint16_t) > unpacked_size) {
                break;
            }
            /* 
            * SizeOfLibidExtended (4bytes)
            * LibidTwiddled (variable)
            * Reserved4 (4bytes)
            * Reserved5 (2bytes) 
            * OriginalTypeLib (16 bytes)
            * Cookie (4 bytes)
            */
            uint32_t sizeof_libidextended = XsUtils::byteorder2host<uint32_t>(*(uint32_t*)(unpacked + pos), true);
            size = sizeof(uint32_t) + sizeof_libidextended + sizeof(uint32_t) * 6 + sizeof(uint16_t);
            break;
        }
        case REFERENCEREGISTERED: {
            if (pos + sizeof(uint32_t) > unpacked_size) {
                break;
            }
            /*
            * Id (2bytes)
            * Size (4bytes), specifies the total size in bytes of SizeOfLibid, Libid, Reserved1 and Reserved2. MUST be ignored on read.
            * SizeOfLibid (4bytes)
            * Libid (variable)
            * Reserved1 (4bytes), MUST be 0x00000000
            * Reserved2 (2bytes), MUST be 0x0000
            */
            uint32_t sizeof_libid = XsUtils::byteorder2host<uint32_t>(*(uint32_t*)(unpacked + pos), true);
            uint32_t size_real = sizeof_libid + sizeof(sizeof_libid) * 2 + sizeof(uint16_t);
            /* NOTE: size != size_real may be there */
            size = size_real; 
            break;
        }
        case REFERENCEPROJECT: {
            if (pos + sizeof(uint32_t) > unpacked_size) {
                break;
            }
            /*
            * Id (2bytes)
            * Size (4bytes), specifies the total size in bytes of SizeOfLibidAbsolute, LibidAbsolute, SizeOfLibidRelative. LibidRelative, MajorVersion, and MinorVersion. MUST be ignored on read.
            * SizeOfLibidAbsolute (4bytes)
            * LibidAbsolute (variable)
            * SizeOfLibidRelative (4bytes)
            * LibidRelative (variable)
            * MajorVersion (4bytes)
            * MinorVersion (2bytes)
            */
            uint32_t size_real = 0;
            uint32_t sizeof_libidabsolute = XsUtils::byteorder2host<uint32_t>(*(uint32_t*)(unpacked + pos), true);
            size_real += sizeof(sizeof_libidabsolute) + sizeof_libidabsolute;
            if (pos + size_real + sizeof(uint32_t) > unpacked_size) {
                size_real += sizeof(uint32_t);
            }
            else {
                uint32_t sizeof_libidrelative = XsUtils::byteorder2host<uint32_t>(*(uint32_t*)(unpacked + pos + size_real), true);
                size_real += sizeof(sizeof_libidrelative) + sizeof_libidrelative;
                size_real += sizeof(uint32_t) + sizeof(uint16_t);
            }
            size = size_real;
            break;
        }
        case MODULEOFFSET: {
            if (pos + sizeof(uint32_t) > unpacked_size) {
                break;
            }
            uint32_t text_offset = XsUtils::byteorder2host<uint32_t>(*(uint32_t*)(unpacked + pos), true);
            if (props->modules.size() && 0 == props->modules[props->modules.size() - 1].offset) {
                props->modules[props->modules.size() - 1].offset = text_offset;
            }
            else {
                throw "code module name and offset error!";
            }
        }
        break;
        case MODULESTREAMNAME: {
            if (pos + size > unpacked_size) {
                break;
            }

            std::string code_module_name;
            uint32_t stream_name_size = size;
            for (uint32_t i = 0; i < stream_name_size; i++, pos++) {
                code_module_name += *(char*)(unpacked + pos);
            }

            if (pos + 6 > unpacked_size) {
                nret = BQ::ERR_OLE_DIR_PROP_FROMAT;
                break;
            }

            uint16_t reserved = XsUtils::byteorder2host<uint16_t>(*(uint16_t*)(unpacked + pos), true);
            if (reserved != 0x0032) {
                end += 1;
                break;
            }
            pos += sizeof(uint16_t);

            uint32_t stream_uni_name_size = XsUtils::byteorder2host<uint32_t>(*(uint32_t*)(unpacked + pos), true);
            pos += sizeof(uint32_t);

            std::u16string code_module_uni_name;
            if (pos + stream_uni_name_size <= unpacked_size) {
                for (uint32_t i = 0; i < stream_uni_name_size; i += 2, pos += 2) {
                    code_module_uni_name += *(char16_t*)(unpacked + pos);
                }
            }
            size = 0;

            if (code_module_uni_name.empty()) {
                /* TODO : codepage */
                code_module_uni_name = XsUtils::wstr2u16s(XsUtils::ansi2unicode(code_module_name));
            }

            vba_project_module_t module_item;
            module_item.name = code_module_uni_name;
            props->modules.push_back(module_item);
            break;
        }
        case PROJECTMODULES: {
            break;
        }
        case PROJECTCOOKIE: {
            /* Cookie */
            size = sizeof(uint16_t);
            break;
        }
        case MODULENAME: {
            break;
        }
        case MODULENAMEUNICODE: {
            break;
        }
        case MODULEDOCSTRING: {
            break;
        }
        case MODULEHELPCONTEXT: {
            break;
        }
        case MODULECOOKIE: {
            break;
        }
        case MODULETYPE_procedural: {
            break;
        }
        case MODULETYPE_document: {
            break;
        }
        case PROJECTMODULES_Terminator: // Terminator of MODULE Record in PROJECTMODULES
            break;
        case DirStream_Terminator: // Terminator of dir stream
            end += 1;
            pos += sizeof(uint32_t); // Reserved (4 bytes): MUST be 0x00000000. 
            break;
        case MODULEPRIVATE:
        case MODULEREADONLY: {
            /* the position of the field size is Reserved, must be 0x00000000 */
            size = 0;
            break;
        }
        default:
            break;
        }
    }

    return nret;
}

int msvba::parse_vba_project_stream(const uint8_t* stream_unpacked, uint32_t size, vba_project_header_t* props, bool ole_is_little_endian) {
    return BQ::NOT_IMPLEMENT;
}

int msvba::vba_attribute_only(const uint8_t* script_utf8, uint32_t size) {
    if (script_utf8 == nullptr || size == 0) {
        return false;
    }

    bool attribute_only = true;

    for (uint32_t offset = 0; offset < size; ) {
        const char* curr_start = (const char*)script_utf8 + offset;
        const char* next_start = strchr(curr_start, '\n');
        if (next_start == nullptr) {
            offset = size;
            next_start = (const char*)script_utf8 + offset;
        }
        else {
            offset = next_start - (const char*)script_utf8 + 1;
        }

        /* skip prefix blank character */
        for (; curr_start < next_start && isblank(uint8_t(*curr_start)); curr_start++);
        if (curr_start == next_start) {
            continue;
        }

        if (ucs::nicmp("Attribute ", curr_start, 10) == 0) {
            continue;
        }
        else if (ucs::nicmp("Option Explicit", curr_start, 15) == 0) {
            /* skip suffix blank character */
            for (curr_start += 15; curr_start < next_start && isblank(uint8_t(*curr_start)); curr_start++);
            if (curr_start == next_start) {
                continue;
            }
        }

        attribute_only = false;
        break;
    }

    return attribute_only;
};

const uint8_t* search_mem_for_vba(const uint8_t* data, uint32_t size, uint32_t start_offset) {
    if (data == nullptr || size == 0) {
        return nullptr;
    }

    /* expected(ignore case): \0Attribut[0x..]e */
    const uint8_t pattern[] = { 0,'a','t','t','r','i','b','u','t', 0, 'e' };
    uint32_t parttern_size = sizeof(pattern) / sizeof(pattern[0]);

    const uint8_t* found = nullptr;
    for (uint32_t offset = start_offset, match_len = 0; offset + parttern_size < size; offset++) {
        auto p = data + offset;
        if (*p != 0) continue;
        match_len = 1;
        for (; match_len < parttern_size; match_len++) {
            if (*(p + match_len) == pattern[match_len] || *(p + match_len) == pattern[match_len] + 'A' - 'a') {
                continue;
            }
            if (match_len == parttern_size - 2) {
                continue;
            }
            break;
        }
        if (match_len != parttern_size) {
            continue;
        }
        if (offset < 3) {
            continue;
        }
        found = p - 3;
        break;
    }

    return found;
}

int msvba::search_stream_for_vba(const uint8_t* data, uint32_t size, uint32_t* soffset, uint8_t** script, uint32_t* script_size) {
    if (data == nullptr || size == 0) {
        return BQ::INVALID_ARG;
    }
    
    int nret = BQ::OK;
    for (uint32_t search_start = 0, offset = 0; search_start < size; search_start = offset + 3) {
        auto start_find = search_mem_for_vba(data, size, search_start);
        if (start_find == nullptr) {
            nret = BQ::ERR_FORMAT;
            break;
        }
        offset = start_find - data;

        uint8_t* unpacked = 0;
        uint32_t unpacked_size = 0;
        nret = RleCoding().decompress(data + offset, size - offset, &unpacked, &unpacked_size);
        if (nret != BQ::OK) {
            continue;
        }

        if (soffset) {
            *soffset = offset;
        }

        if (script && script_size) {
            *script = unpacked;
            *script_size = unpacked_size;
        }
        else {
            if (unpacked) free(unpacked);
        }

        break;
    }

    return nret;
}

int msvba::unpack_stream(const olefile_t* ole, uint32_t did, uint8_t** unpacked, uint32_t* unpacked_size, uint32_t stream_offset) {
    if (ole == nullptr || did >= ole->entry_count || unpacked_size == nullptr || unpacked == nullptr) {
        return BQ::INVALID_ARG;
    }

    uint32_t nret = BQ::OK;
    do {
        uint8_t* buffer = 0;
        uint32_t size = 0;
        uint8_t* buffer_unpacked = nullptr;
        uint32_t buffer_unpacked_size = 0;

        ON_SCOPE_EXIT([&] { if (buffer) free(buffer); });
        nret = ole::read_stream(ole, did, -1, &buffer, &size);
        if (nret != BQ::OK) {
            break;
        }

        if (size <= stream_offset) {
            nret = BQ::ERR_OLE_DIR_PROP_FROMAT;
            break;
        }

        nret = RleCoding().decompress(buffer + stream_offset, size - stream_offset, &buffer_unpacked, &buffer_unpacked_size);
        if (nret != BQ::OK) {
            break;
        }

        *unpacked = buffer_unpacked;
        *unpacked_size = buffer_unpacked_size;
    } while (false);

    return nret;
}

int process_extracted_vba(const cfb_ctx_t* ctx, const olefile_t* ole, uint32_t did, uint32_t offset, const char* utf8name
    , uint8_t* unpacked, uint32_t unpacked_size, uint16_t codepage, bool is_pcode, ifilehandler* cb_file) {
    if (unpacked == 0 || unpacked_size == 0 || ctx == 0 || 0 == ctx->file_creator || 0 == cb_file) {
        return BQ::INVALID_ARG;
    }

    int nret = BQ::OK;
    do {
        ifile* cur_file = nullptr;
        ON_SCOPE_EXIT([&]() {if (cur_file) cur_file->release(); });

        bool need_free_out = true;
        uint8_t* out = nullptr;
        uint32_t out_size = 0;
        ON_SCOPE_EXIT([&] { if (need_free_out && out) free(out); });

        nret = XsCodepage::cp2utf8(unpacked, unpacked_size, codepage, &out, &out_size);
        if (nret != BQ::OK) {
            out = unpacked;
            out_size = unpacked_size;
            need_free_out = false;
        }

        std::string alias = utf8name ? utf8name : "";
        std::string fname = alias + "." + std::to_string(did);
#if _DEBUG
        fname += ".";
        fname += is_pcode ? "pcode" : (msvba().vba_attribute_only(out, out_size) ? "m" : "vba");
#endif
        cur_file = (ctx->file_creator)(ole, ctx->temp_dirpath, fname.c_str(), alias.c_str(), 0);
        if (cur_file == nullptr) {
            nret = BQ::ERR_FILE_CREATOR;
            MESSAGE_WARN(ctx, "unpack_stream create file error: %s \r\n", alias.c_str());
            break;
        }
        if (ctx && ctx->write_subfile_header && ctx->header_vba_xlm) {
            std::string header = ctx->header_vba_xlm;
            header += (is_pcode ? "PCODE\n" : "VBA\n");
            if (0 > cur_file->write(0, (const uint8_t*)header.c_str(), header.size())) {
                nret = BQ::ERR_WRITE;
                break;
            }
        }
        if (0 > cur_file->write(-1, out, out_size)) {
            nret = BQ::ERR_WRITE;
            break;
        }

        uint32_t file_type = is_pcode ? ft_vba_pcode : ft_vba;
        embedded_object_t fileobj{ file_obj_t{cur_file, file_type, 0, ole}, did, offset };
        if (0 > cb_file->handle(ctx, (const file_obj_t*)&fileobj)) {
            nret = BQ::ERR_USER_INTERRUPT;
            break;
        }

        nret = BQ::OK;
    } while (false);

    return nret;
}

int try_extract_vba_orphans(const cfb_ctx_t* ctx, const olefile_t* ole, entry_tree_item_t* vba_storage, ifilehandler* cb_file) {
    if (ole == nullptr || cb_file == nullptr || vba_storage == nullptr) {
        return BQ::INVALID_ARG;
    }

    /*
    * for perfermance, we only try to search vba scripts in the isolated ole stream
    * which is the same level with a dir stream or a _VBA_PROJECT stream.
    */

    int nret = BQ::OK;
    for (auto cur_node = vba_storage->child_first; cur_node; cur_node = cur_node->slibing_next) {
        if (cur_node->item->obj_type != ole_dir_entry_stream) {
            continue;
        }

        uint32_t did = get_ole_tree_item_did(ole, cur_node);
        uint8_t* buffer = 0;
        uint32_t size = 0;
        nret = ole::read_stream(ole, did, -1, &buffer, &size);
        if (nret != BQ::OK) {
            break;
        }
        ON_SCOPE_EXIT([&] { if (buffer) free(buffer); });

        do {
            uint32_t offset = 0;
            uint8_t* unpacked = 0;
            uint32_t unpacked_size = 0;

            nret = msvba::search_stream_for_vba(buffer, size, &offset, &unpacked, &unpacked_size);
            if (nret != BQ::OK) {
                break;
            }
            ON_SCOPE_EXIT([&] { if (unpacked) free(unpacked); });
            
            std::string name = XsUtils::u16s2utf(cur_node->item->name);
            nret = process_extracted_vba(ctx, ole, did, offset, name.c_str(), unpacked, unpacked_size, 0, false, cb_file);
            if (nret != BQ::OK) {
                MESSAGE_WARN(ctx, "[try_extract_vba_orphans] process_extracted_vba error: %s \r\n", name.c_str());
            }
            break;
        } while (false);

        if (nret == BQ::ERR_USER_INTERRUPT) {
            break;
        }
        nret = BQ::OK;
    }

    return nret;
}

int try_decode_pcode(const cfb_ctx_t* ctx, const olefile_t* ole, entry_tree_item_t* vba_project, const vba_project_props_t* props, std::vector<uint32_t> module_ids, ifilehandler* cb_file) {
    if (ole == nullptr || vba_project == nullptr || props == nullptr || module_ids.size() == 0) {
        return BQ::INVALID_ARG;
    }

    int nret = BQ::OK;
    bool little_endian = OleUtils::little_endian(&ole->header);

    vba_project_header_t proj_header;
    string_list_t identifiers;
    ON_SCOPE_EXIT([&]() { identifiers.release(); });

    do {
        uint32_t did = vba_project - ole->entry_tree;
        uint8_t* data = 0;
        uint32_t size = 0;
        uint32_t size_min = sizeof(proj_header) - 1;

        ON_SCOPE_EXIT([&] { if (data) free(data); });
        nret = ole::read_stream(ole, did, -1, &data, &size);
        if (nret != BQ::OK) {
            break;
        }

        if (size <= size_min) {
            nret = BQ::ERR_FORMAT;
            break;
        }

        memcpy(&proj_header, data, size_min);
        proj_header.version = XsUtils::byteorder2host(proj_header.version, little_endian);

        nret = parse_performance_cache(data, size, &identifiers, little_endian);
        if (nret != BQ::OK) {
            break;
        }
    } while (false);

    if (nret != BQ::OK) {
        return nret;
    }

    for (uint32_t i = 0, module_id = 0, offset = 0, cur_did = 0, codepage = 0; i < module_ids.size(); i++) {
        module_id = module_ids[i];
        if (module_id >= props->modules.size()) {
            continue;
        }
        codepage = props->codepage;
        auto& cur_module = props->modules[module_id];
        offset = cur_module.offset;
        cur_did = cur_module.directory_id;
        if (cur_did == 0) {
            continue;
        }

        uint8_t* data = 0;
        uint32_t size = 0;
        ON_SCOPE_EXIT([&] { if (data) free(data); });
        nret = ole::read_stream(ole, cur_did, -1, &data, &size);
        if (nret != BQ::OK) {
            break;
        }
        std::string name = XsUtils::u16s2utf(cur_module.name);

        uint8_t* out = 0;
        uint32_t out_size = 0;
        ON_SCOPE_EXIT([&] { if (out) free(out); });
        nret = decode_pcode(data, size, &identifiers, props->syskind, proj_header.version, &out, &out_size);
        if (nret != BQ::OK) {
            MESSAGE_WARN(ctx, "[try_decode_pcode] decode_pcode %s error: 0x%08X \r\n", name.c_str(), nret);
            continue;
        }
        
        if (out_size == 0 || out == nullptr) {
            continue;
        }
        
        nret = process_extracted_vba(ctx, ole, cur_did, offset, name.c_str(), out, out_size, codepage, true, cb_file);
        if (nret != BQ::OK) {
            if (nret == BQ::ERR_USER_INTERRUPT) {
                break;
            }
        }
    }

    return nret;
}

int try_extract_vba_project(const cfb_ctx_t* ctx, const olefile_t* ole, ifilehandler* cb_file, entry_tree_item_t* vba_project, entry_tree_item_t* dir, entry_tree_item_t* project) {
    if (ole == nullptr || dir == nullptr || project == nullptr) {
        return BQ::INVALID_ARG;
    }

    int nret = BQ::OK;
    vba_project_props_t vba_props;
    do {
        uint8_t* buffer = 0;
        uint32_t size = 0;
        uint32_t dir_did = ((uint8_t*)dir - (uint8_t*)ole->entry_tree) / sizeof(entry_tree_item_t);

        ON_SCOPE_EXIT([&] { if (buffer) free(buffer); });
        nret = msvba::unpack_stream(ole, dir_did, &buffer, &size);
        if (nret != BQ::OK) {
            MESSAGE_WARN(ctx, "unpack_stream error: %s \r\n", XsUtils::u16s2utf(dir->item->name).c_str());
            break;
        }

        nret = msvba().parse_dir_stream(buffer, size, &vba_props);
        if (nret != BQ::OK) {
            break;
        }

        for (uint32_t i = 0; i < vba_props.modules.size(); i++) {
            entry_tree_item_t* cur_node = nullptr;
            for (auto next_node = dir->parent->child_first; next_node; next_node = next_node->slibing_next) {
                if (0 == ucs::icmp(next_node->item->name, vba_props.modules[i].name.c_str())) {
                    cur_node = next_node;
                    break;
                }
            }
            if (cur_node == nullptr) {
                continue;
            }
            vba_props.modules[i].directory_id = ((uint8_t*)cur_node - (uint8_t*)ole->entry_tree) / sizeof(entry_tree_item_t);
        }
    } while (false);

    if (nret != BQ::OK) {
        if (ctx && ctx->extract_vba_orphan) {
            nret = try_extract_vba_orphans(ctx, ole, dir->parent, cb_file);
            if (nret != BQ::OK) {
                MESSAGE_WARN(ctx, "[try_extract_vba_project] try_extract_vba_orphans error: %08X \r\n", nret);
            }
        }
        return nret;
    }

    std::vector<uint32_t> try_depcode;
    for (uint32_t i = 0, offset = 0, cur_did = 0, codepage = 0; i < vba_props.modules.size(); i++) {
        codepage = vba_props.codepage;
        offset = vba_props.modules[i].offset;
        cur_did = vba_props.modules[i].directory_id;
        if (cur_did == 0) {
            continue;
        }

        uint8_t* unpacked = 0;
        uint32_t unpacked_size = 0;
        ON_SCOPE_EXIT([&] { if (unpacked) free(unpacked); });

        nret = msvba::unpack_stream(ole, cur_did, &unpacked, &unpacked_size, offset);
        if (nret != BQ::OK) {
            if (ctx && ctx->try_depcode) {
                try_depcode.push_back(i);
            }
            MESSAGE_WARN(ctx, "unpack_stream error: %s \r\n", XsUtils::u16s2utf(vba_props.modules[i].name).c_str());
            continue;
        }

        //std::string name = XsUtils::unicode2ansi(XsUtils::utf2uni(XsUtils::u16s2utf(vba_props.modules[i].name)));
        std::string name = XsUtils::u16s2utf(vba_props.modules[i].name);
        nret = process_extracted_vba(ctx, ole, cur_did, offset, name.c_str(), unpacked, unpacked_size, codepage, false, cb_file);
        if (nret != BQ::OK) {
            MESSAGE_WARN(ctx, "process_extracted_vba error: %s \r\n", name.c_str());
            if (nret == BQ::ERR_USER_INTERRUPT) {
                break;
            }
        }
    }

    if (try_depcode.size() > 0 && nret != BQ::ERR_USER_INTERRUPT) {
        try_decode_pcode(ctx, ole, vba_project, &vba_props, try_depcode, cb_file);
    }

    return nret;
}

int msvba::find_vba_projects(const olefile_t* ole, bool strict_mode, vba_project_nodes_t* projects, uint32_t* count) {
    if (ole == nullptr) {
        return BQ::INVALID_ARG;
    }

    std::vector<vba_project_nodes_t> vec_projs;
    for (uint32_t i = 0; i < ole->entry_count; i++) {
        entry_tree_item_t* cur_node = ole->entry_tree + i;
        if (cur_node->item->obj_type != ole_dir_entry_stream) {
            continue;
        }

        if (0 != ucs::icmp(cur_node->item->name, u"_vba_project")) {
            continue;
        }
        vba_project_nodes_t project;

        /*
        * 1. parent should be VBA
        * 2. should have slibing stream named dir
        */
        if (cur_node->parent && 0 == ucs::icmp(cur_node->parent->item->name, u"VBA")) {
            for (auto next_node = cur_node->parent->child_first; next_node; next_node = next_node->slibing_next) {
                if (0 == ucs::icmp(next_node->item->name, u"dir")) {
                    project.dir = next_node;
                    break;
                }
            }
            if (cur_node->parent->parent != nullptr) {
                for (auto next_node = cur_node->parent->parent->child_first; next_node; next_node = next_node->slibing_next) {
                    if (0 != ucs::icmp(next_node->item->name, u"project")) {
                        project.project = next_node;
                        break;
                    }
                }
            }
            else {
                /*
                 * orphans vba_project entries
                 */
                for (uint32_t i = 0; i < ole->entry_count; i++) {
                    entry_tree_item_t* for_project = ole->entry_tree + i;
                    if (0 != ucs::icmp(for_project->item->name, u"project") || for_project->parent != nullptr) {
                        continue;
                    }
                    project.project = for_project;
                    break;
                }
            }
        }

        if (project.dir == nullptr || (strict_mode && project.project == nullptr)) {
            continue;
        }
        project.vba_project = cur_node;

        vec_projs.push_back(project);
    }

    if (projects && count && *count >= vec_projs.size()) {
        for (uint32_t i = 0; i < vec_projs.size(); i++) {
            memcpy((void*)(projects + i), (void*)&vec_projs[i], sizeof(vec_projs[i]));
        }
    }
    if (count) {
        *count = vec_projs.size();
    }
    return BQ::OK;
}


int msvba::extract_vba_project(const cfb_ctx_t* ctx, const olefile_t* ole, ifilehandler* cb_file, bool strict_mode) {
    if (ole == nullptr || nullptr == ctx || cb_file == nullptr) {
        return BQ::INVALID_ARG;
    }

    int nret = BQ::OK;
    uint32_t project_cnt = 0;
    vba_project_nodes_t* projects = nullptr;
    ON_SCOPE_EXIT([&]() { if (projects) delete[] projects; projects = nullptr; });
    
    nret = find_vba_projects(ole, strict_mode, nullptr, &project_cnt);
    if (nret != BQ::OK) {
        return nret;
    }
    projects = new vba_project_nodes_t[project_cnt];
    nret = find_vba_projects(ole, strict_mode, projects, &project_cnt);
    if (nret != BQ::OK) {
        return nret;
    }

    for (uint32_t i = 0; i < project_cnt; i++) {
        auto project = projects + i;
        nret = try_extract_vba_project(ctx, ole, cb_file, project->vba_project, project->dir, project->project);
        if (nret != BQ::OK) {
            uint32_t did = project->dir && ole->entry_tree ? project->dir - ole->entry_tree : 0;
            MESSAGE_WARN(ctx, "[extract_vba_project] try_extract_vba_project did=%d error: %08X \r\n", did, nret);
        }
    }

    return nret;
};