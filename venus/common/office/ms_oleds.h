#pragma once
#include<cassert>
#include "./cfb_base.h"
#include "./ms_doc.h"
#include "./helper.h"
#include "./ole_utils.h"

#pragma pack(push, 1)

typedef struct __st_LengthPrefixedAnsiString {
    /*
    * length is the number of ANSI characters in the String field. including the terminating null character.
    * length MUST be set to 0x00000000 to indicate an empty string
    */
    uint32_t length;
    char     data[1]; /* variable, maybe not exists */
} len_prefixed_ansi_str_t;

typedef struct __st_oleds_CompObjStream {
    struct __st_CompObjHeader {
        uint32_t reserved1;
        uint32_t version;
        uint8_t reserved2[20];
    } header;

    len_prefixed_ansi_str_t user_type; /* display name of the linked object or embedded object */

    struct __st_ClipboardFormatOrAnsiString {
        uint32_t MarkerOrLength;
        char     FormatOrAnsiString[1]; /* variable, maybe not exists */
    } ansi_clipboard_format;

    /*
    * Reserved1 (variable)
    * UnicodeMarker (variable)
    * UnicodeUserType (variable)
    * UnicodeClipboardFormat (variable)
    * Reserved2 (variable)
    */
} oleds_compobj_t;

#pragma pack(pop)

typedef struct __st_oleds_embedded {
    ole2_directory_entry_t* stg{ 0 };
    ole2_directory_entry_t* ole10native{ 0 };
    ole2_directory_entry_t* package{ 0 };
    std::string             ansi_user_type;
    std::string             name;
    embedded_object_t       object;
} oleds_embedded_t;

typedef struct __st_mbd_item : oleds_embedded_t {
} mbd_item_t;

typedef struct __st_objectpool_item : oleds_embedded_t {
    odt_t       odt;
    uint32_t    odt_size{ 0 };
} objectpool_item_t;

class ms_embedded {
public:
    int parse_xls_mbd_stg(const cfb_ctx_t* ctx, const olefile_t* ole, const entry_tree_item_t* stg_item, mbd_item_t* obj, bool little_endian) {
        if (ole == nullptr || stg_item == nullptr || obj == nullptr || stg_item->item == nullptr) {
            return BQ::INVALID_ARG;
        }

        if (stg_item->item->obj_type != ole_dir_entry_storage) {
            return BQ::INVALID_ARG;
        }

        if (ucs::nicmp(stg_item->item->name, u"MBD", 3) || ucs::len(stg_item->item->name) != 11) {
            return BQ::INVALID_ARG;
        }

        ole2_directory_entry_t* objinfo = 0, * ole10native = 0, * compobj = 0, * package = 0;
        for (auto cur_stream = stg_item->child_first; cur_stream; cur_stream = cur_stream->slibing_next) {
            if (cur_stream->item->obj_type != ole_dir_entry_stream) {
                continue;
            }

            if (objinfo && compobj && (ole10native || package)) {
                break;
            }

            if (0 == ucs::icmp(cur_stream->item->name, u"\003ObjInfo")) {
                objinfo = cur_stream->item;
            }

            if (0 == ucs::icmp(cur_stream->item->name, u"\001ole10native")) {
                ole10native = cur_stream->item;
            }

            if (0 == ucs::icmp(cur_stream->item->name, u"\001CompObj")) {
                compobj = cur_stream->item;
            }

            if (0 == ucs::icmp(cur_stream->item->name, u"Package")) {
                package = cur_stream->item;
            }
        }

        if (!objinfo || !(ole10native || package)) {
            // TODO: maybe an office document
            return BQ::ERR_FORMAT;
        }

        int nret = BQ::OK;
        if (compobj) {
            /* maybe there is no \001CompObj */
            nret = parse_stream_compobj(ctx, ole, compobj, obj, little_endian);
            if (nret != BQ::OK) {
                return nret;
            }
        }

        obj->stg = stg_item->item;
        obj->ole10native = ole10native;
        obj->package = package;

        if (obj->ole10native == nullptr && obj->package) {
            nret = extract_embedded_package(ctx, ole, obj);
        }
        else if (obj->ole10native) {
            nret = extract_embedded_ole10native(ctx, ole, obj, little_endian);
        }

        return nret;
    }

    int parse_doc_objectpool_stg(const cfb_ctx_t* ctx, const olefile_t* ole, const entry_tree_item_t* stg_item, objectpool_item_t* obj, bool little_endian) {
        if (ole == nullptr || stg_item == nullptr || obj == nullptr || stg_item->item == nullptr) {
            return BQ::INVALID_ARG;
        }

        if (stg_item->item->obj_type != ole_dir_entry_storage) {
            return BQ::INVALID_ARG;
        }

        if (ucs::nicmp(stg_item->item->name, u"_", 1) || ucs::len(stg_item->item->name) != 11) {
            return BQ::INVALID_ARG;
        }

        objectpool_item_t obj_item;
        ole2_directory_entry_t* objinfo = 0, * ole10native = 0, * compobj = 0, * package = 0;
        for (auto cur_stream = stg_item->child_first; cur_stream; cur_stream = cur_stream->slibing_next) {
            if (cur_stream->item->obj_type != ole_dir_entry_stream) {
                continue;
            }

            if (objinfo && compobj && (ole10native || package)) {
                break;
            }

            if (0 == ucs::icmp(cur_stream->item->name, u"\003ObjInfo")) {
                objinfo = cur_stream->item;
            }

            if (0 == ucs::icmp(cur_stream->item->name, u"\001ole10native")) {
                ole10native = cur_stream->item;
            }

            if (0 == ucs::icmp(cur_stream->item->name, u"\001CompObj")) {
                compobj = cur_stream->item;
            }

            if (0 == ucs::icmp(cur_stream->item->name, u"Package")) {
                /*
                * NOTE: don't find any avaiable documents
                *
                * with my tests, storages for embedded OLE objects in ObjectPool Storage should
                * have either a Package stream or a \001ole10native stream.
                */
                package = cur_stream->item;
            }
        }

        if (!objinfo || !(ole10native || package)) {
            // TODO: maybe an office document
            return BQ::ERR_FORMAT;
        }

        int nret = BQ::OK;
        do {
            uint32_t odt_did = objinfo - ole->entries;
            nret = ole::read_stream(ole, odt_did, (uint8_t*)&obj_item.odt, sizeof(odt_t), &obj_item.odt_size);
        } while (false);

        if (nret != BQ::OK) {
            return nret;
        }

        if (compobj) {
            /* maybe there is no \001CompObj */
            nret = parse_stream_compobj(ctx, ole, compobj, obj, little_endian);
            if (nret != BQ::OK) {
                return nret;
            }
        }

        obj->stg = stg_item->item;
        obj->ole10native = ole10native;
        obj->package = package;

        if (obj->ole10native == nullptr && obj->package) {
            nret = extract_embedded_package(ctx, ole, obj);
        }
        else if (obj->ole10native) {
            nret = extract_embedded_ole10native(ctx, ole, obj, little_endian);
        }

        return nret;
    }

protected:
    int parse_stream_compobj(const cfb_ctx_t* ctx, const olefile_t* ole, const ole2_directory_entry_t* compobj, oleds_embedded_t* obj, bool little_endian) {
        if (ole == nullptr || obj == nullptr || compobj == nullptr) {
            return BQ::INVALID_ARG;
        }

        uint32_t compobj_did = compobj - ole->entries;
        uint8_t* data = 0;
        uint32_t size = 0;

        ON_SCOPE_EXIT([&] { if (data) free(data); });
        int nret = ole::read_stream(ole, compobj_did, -1, &data, &size);
        if (nret != BQ::OK) {
            return nret;
        }

        if (data == nullptr || size == 0) {
            return nret;
        }

        oleds_compobj_t coitem;
        memcpy((void*)&coitem, data, sizeof(coitem.header));
        coitem.header.version = XsUtils::byteorder2host<uint32_t>(coitem.header.version, little_endian);

        coitem.user_type.length = XsUtils::byteorder2host<uint32_t>(*(uint32_t*)(data + sizeof(coitem.header)), little_endian);
        if (coitem.user_type.length > 0 && coitem.user_type.length < size - sizeof(coitem.header) - sizeof(uint32_t)) {
            obj->ansi_user_type = read_ansi_str(data + sizeof(coitem.header) + sizeof(uint32_t), coitem.user_type.length);
            assert(obj->ansi_user_type.size() + 1 == coitem.user_type.length);
        }
        else {
            obj->ansi_user_type = "unknown";
        }

        return nret;
    }

    int extract_embedded_package(const cfb_ctx_t* ctx, const olefile_t* ole, oleds_embedded_t* obj) {
        if (obj == nullptr || !(obj->ole10native == nullptr && obj->package)) {
            return BQ::INVALID_ARG;
        }

        int nret = BQ::OK;
        do {
            uint32_t package_did = obj->package - ole->entries;
            uint8_t* data = 0;
            uint32_t size = 0;

            ON_SCOPE_EXIT([&] { if (data) free(data); });
            nret = ole::read_stream(ole, package_did, -1, &data, &size);
            if (nret != BQ::OK) {
                break;
            }

            std::string name = XsUtils::u16s2utf(obj->stg->name) + "_" + obj->ansi_user_type;
            auto embedded = create_embedded(ctx, ole, data, size, ft_unknown, name.c_str());
            if (embedded == nullptr || embedded->obj.file == nullptr) {
                nret = BQ::ERR_FILE_CREATOR;
                if (embedded) { delete embedded, embedded = nullptr; }
            }
            else {
                obj->object = *embedded;
                obj->object.did = package_did;
            }
            ON_SCOPE_EXIT([&]() { if (embedded) delete embedded; });

            break;
        } while (false);

        return nret;
    }

    int extract_embedded_ole10native(const cfb_ctx_t* ctx, const olefile_t* ole, oleds_embedded_t* obj, bool little_endian) {
        if (obj == nullptr || obj->ole10native == nullptr) {
            return BQ::INVALID_ARG;
        }

        uint32_t native_did = obj->ole10native - ole->entries;
        uint8_t* data = 0;
        uint32_t size = 0;

        ON_SCOPE_EXIT([&] { if (data) free(data); });
        int nret = ole::read_stream(ole, native_did, -1, &data, &size);
        if (nret != BQ::OK) {
            return nret;
        }

        if (data == nullptr || size == 0) {
            return nret;
        }

        std::string embedded_raw_name;
        uint8_t* embedded_start = 0;
        uint32_t embedded_size = 0;
        do {
            if (size < 8) {
                nret = BQ::ERR_FORMAT;
                break;
            }
        
            uint32_t pos = 0;
            embedded_size = XsUtils::byteorder2host<uint32_t>(*(uint32_t*)data, little_endian);
            assert(size >= embedded_size + 4);
            pos += sizeof(uint32_t);
            /* probably the OLE type id */
            uint16_t type_id = XsUtils::byteorder2host<uint32_t>(*(uint32_t*)(data + pos), little_endian);
            pos += sizeof(uint32_t);

            std::string name = read_ansi_str(data + pos, size - pos);
            pos += name.size() + 1;
            std::string path = read_ansi_str(data + pos, size - pos);
            pos += path.size() + 1;

            /* 8 bytes, probably a timestamp in FILETIME from olefile */
            pos += 8;
            if (size <= pos) {
                nret = BQ::ERR_FORMAT;
                break;
            }

            std::string temp_path = read_ansi_str(data + pos, size - pos);
            pos += temp_path.size() + 1;

            if (size <= pos + sizeof(uint32_t)) {
                nret = BQ::ERR_FORMAT;
                break;
            }

            embedded_size = XsUtils::byteorder2host<uint32_t>(*(uint32_t*)(data + pos), little_endian);
            if (embedded_size > size - (pos + sizeof(uint32_t))) {
                nret = BQ::ERR_FORMAT;
                break;
            }
            embedded_start = data + pos + sizeof(uint32_t);
            embedded_raw_name = name;
        } while (false);

        if (nret != BQ::OK) {
            return nret;
        }

        if (embedded_raw_name.empty()) {
            embedded_raw_name = obj->ansi_user_type;
        }
        else {
            obj->name = embedded_raw_name;
        }

        std::string name = XsUtils::u16s2utf(obj->stg->name) + "_" + embedded_raw_name;
        auto embedded = create_embedded(ctx, ole, embedded_start, embedded_size, ft_unknown, name.c_str());
        if (embedded == nullptr || embedded->obj.file == nullptr) {
            nret = BQ::ERR_FILE_CREATOR;
            if (embedded) { delete embedded, embedded = nullptr; }
        }
        else {
            obj->object = *embedded;
            obj->object.did = native_did;
        }
        ON_SCOPE_EXIT([&]() { if (embedded) delete embedded; });

        return nret;
    }
};
