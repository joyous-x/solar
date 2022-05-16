#include <assert.h>
#include <stack>
#include "xlm.h"
#include "rle.h"
#include "offcrypto.h"
#include "ole_walker.h"
#include "helper.h"

///////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////

std::string visible_property_name(const char* name, uint32_t size) {
    if ((name[0] == 0 && name[1] == 0) || size <= 0 || size > 128) {
        return "";
    }

    char newname[OLE_DIRECTOR_ENTRY_NAME_SIZE * 3 + 7] = { 0 };

    /* size-2 to ignore trailing NULL */
    for (uint32_t i = 0, j = 0; i < size - 2 && j < OLE_DIRECTOR_ENTRY_NAME_SIZE * 3; i += 2) {
        if ((!(name[i] & 0x80)) && isprint(name[i]) && name[i + 1] == 0) {
            newname[j++] = tolower(name[i]);
        } else {
            if (name[i] < 10 && name[i] >= 0 && name[i + 1] == 0) {
                newname[j++] = '_';
                newname[j++] = name[i] + '0';
            }
            else {
                const uint16_t x = (((uint16_t)name[i]) << 8) | name[i + 1];

                newname[j++] = '_';
                newname[j++] = 'a' + ((x & 0xF));
                newname[j++] = 'a' + ((x >> 4) & 0xF);
                newname[j++] = 'a' + ((x >> 8) & 0xF);
                newname[j++] = 'a' + ((x >> 12) & 0xF);
            }
            newname[j++] = '_';
        }
    }

    return std::string(newname);
}

int OlePropertyHandler::walk_through_for_debug(const cfb_ctx_t* ctx, olefile_t* ole, uint32_t directory_id, ifilehandler* handler) {
    if (nullptr == ole) {
        return BQ::INVALID_ARG;
    }

    auto entry = ole->entry_tree + directory_id;
    std::string msg = "";
    for (size_t i = 0; i < entry->depth; i++) {
        msg += "....";
    }

    msg += visible_property_name((const char*)entry->item->name, entry->item->name_bytes);
    MESSAGE_DEBUG(ctx, "%s\n", msg.c_str());

    return BQ::OK;
}

int OlePropertyHandler::walk_through_files(const cfb_ctx_t* ctx, olefile_t* ole, uint32_t directory_id, ifilehandler* handler) {
    if (ctx == nullptr || ole == nullptr || directory_id >= ole->entry_count) {
        return BQ::INVALID_ARG;
    }

    int nret = BQ::OK;
    ole2_directory_entry_t* entry = ole->entries + directory_id;

    if (entry->obj_type != ole_dir_entry_stream) {
        return nret;
    }

    if (0 == ucs::icmp(entry->name, u"\001ole10native") && ole->filetype != ft_ms_doc) {
        /* processed in doc¡¢xls */
    }

    return nret;
}
