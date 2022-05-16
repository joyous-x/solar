#include <map>
#include <assert.h>
#include <string.h>
#include "ms_shellink.h"
#include "../utils/utils.h"

/*
* reference : 
*   https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-shllink/16cb4ca1-9339-4d0c-a68d-bf1d6cc0f943
*   https://bbs.pediy.com/thread-262082.htm
*/

typedef struct __st_clsid {
    uint8_t bytes[16];
} clsid_t;

const std::map<uint32_t, clsid_t> clsid_known = {
    //{208D2C60-3AEA-1069-A2D7-08002B30309D}
    {ClsidType_NetworkPlaces, {0x60, 0x2C, 0x8D, 0x20, 0xEA, 0x3A, 0x69, 0x10, 0xA2, 0xD7, 0x08, 0x00, 0x2B, 0x30, 0x30, 0x9D }},
    //{46e06680-4bf0-11d1-83ee-00a0c90dc849}
    {ClsidType_NetworkDomain, { 0x80, 0x66, 0xE0, 0x46, 0xF0, 0x4B, 0xD1, 0x11, 0x83, 0xEE, 0x00, 0xA0, 0xC9, 0x0D, 0xC8, 0x49 }},
    //{c0542a90-4bf0-11d1-83ee-00a0c90dc849}
    {ClsidType_NetworkServer, { 0x90, 0x2A, 0x54, 0xC0, 0xF0, 0x4B, 0xD1, 0x11, 0x83, 0xEE, 0x00, 0xA0, 0xC9, 0x0D, 0xC8, 0x49 }},
    //{54a754c0-4bf1-11d1-83ee-00a0c90dc849}
    {ClsidType_NetworkShare, { 0xC0, 0x54, 0xA7, 0x54, 0xF1, 0x4B, 0xD1, 0x11, 0x83, 0xEE, 0x00, 0xA0, 0xC9, 0x0D, 0xC8, 0x49 }},
    //{20D04FE0-3AEA-1069-A2D8-08002B30309D}
    {ClsidType_MyComputer, { 0xE0, 0x4F, 0xD0, 0x20, 0xEA, 0x3A, 0x69, 0x10, 0xA2, 0xD8, 0x08, 0x00, 0x2B, 0x30, 0x30, 0x9D }},
    //{871C5380-42A0-1069-A2EA-08002B30309D}
    {ClsidType_Internet, { 0x80, 0x53, 0x1C, 0x87, 0xA0, 0x42, 0x69, 0x10, 0xA2, 0xEA, 0x08, 0x00, 0x2B, 0x30, 0x30, 0x9D }},
    //{F3364BA0-65B9-11CE-A9BA-00AA004AE837}
    {ClsidType_ShellFSFolder, { 0xA0, 0x4B, 0x36, 0xF3, 0xB9, 0x65, 0xCE, 0x11, 0xA9, 0xBA, 0x00, 0xAA, 0x00, 0x4A, 0xE8, 0x37 }},
    //{645FF040-5081-101B-9F08-00AA002F954E}
    {ClsidType_RecycleBin, { 0x40, 0xF0, 0x5F, 0x64, 0x81, 0x50, 0x1B, 0x10, 0x9F, 0x08, 0x00, 0xAA, 0x00, 0x2F, 0x95, 0x4E }},
    //{21EC2020-3AEA-1069-A2DD-08002B30309D}
    {ClsidType_ControlPanel, { 0x20, 0x20, 0xEC, 0x21, 0xEA, 0x3A, 0x69, 0x10, 0xA2, 0xDD, 0x08, 0x00, 0x2B, 0x30, 0x30, 0x9D }},
    //{450D8FBA-AD25-11D0-98A8-0800361B1103}
    {ClsidType_MyDocuments, { 0xBA, 0x8F, 0x0D, 0x45, 0x25, 0xAD, 0xD0, 0x11, 0x98, 0xA8, 0x08, 0x00, 0x36, 0x1B, 0x11, 0x03 }},
    //{088e3905-0323-4b02-9826-5d99428e115f}
    {ClsidType_Download, { 0x05, 0x39, 0x8E, 0x08, 0x23, 0x03, 0x02, 0x4B, 0x98, 0x26, 0x5D, 0x99, 0x42, 0x8E, 0x11, 0x5F }},
    //{24ad3ad4-a569-4530-98e1-ab02f9417aa8}
    {ClsidType_Pictures, { 0xD4, 0x3A, 0xAD, 0x24, 0x69, 0xA5, 0x30, 0x45, 0x98, 0xE1, 0xAB, 0x02, 0xF9, 0x41, 0x7A, 0xA8 }},
    //{3dfdf296-dbec-4fb4-81d1-6a3438bcf4de}
    {ClsidType_Music, { 0x96, 0xF2, 0xFD, 0x3D, 0xEC, 0xDB, 0xB4, 0x4F, 0x81, 0xD1, 0x6A, 0x34, 0x38, 0xBC, 0xF4, 0xDE }},
    //{B4BFCC3A-DB2C-424C-B029-7FE99A87C641}
    {ClsidType_Desktop, { 0x3A, 0xCC, 0xBF, 0xB4, 0x2C, 0xDB, 0x4C, 0x42, 0xB0, 0x29, 0x7F, 0xE9, 0x9A, 0x87, 0xC6, 0x41 }},
    //{d3162b92-9365-467a-956b-92703aca08af}
    {ClsidType_Documents, { 0x92, 0x2B, 0x16, 0xD3, 0x65, 0x93, 0x7A, 0x46, 0x95, 0x6B, 0x92, 0x70, 0x3A, 0xCA, 0x08, 0xAF }},
    //{f86fa3ab-70d2-4fc7-9c99-fcbf05467f3a}
    {ClsidType_Videos, { 0xAB, 0xA3, 0x6F, 0xF8, 0xD2, 0x70, 0xC7, 0x4F, 0x9C, 0x99, 0xFC, 0xBF, 0x05, 0x46, 0x7F, 0x3A }},
    //{0DB7E03F-FC29-4DC6-9020-FF41B59E513A}
    {ClsidType_3DObjects, { 0x3F, 0xE0, 0xB7, 0x0D, 0x29, 0xFC, 0xC6, 0x4D, 0x90, 0x20, 0xFF, 0x41, 0xB5, 0x9E, 0x51, 0x3A }},
    //{374DE290-123F-4565-9164-39C4925E467B}
    {ClsidType_Download7, { 0x90, 0xE2, 0x4D, 0x37, 0x3F, 0x12, 0x65, 0x45, 0x91, 0x64, 0x39, 0xC4, 0x92, 0x5E, 0x46, 0x7B }},
};

int get_clsid_type(const uint8_t* clsid, uint32_t size) {
    if (clsid == nullptr || size != sizeof(clsid_t::bytes)) {
        return ClsidType_Invalid;
    }
    int rst = ClsidType_Unknown;
    for (auto& iter : clsid_known) {
        if (0 == memcmp(clsid, iter.second.bytes, sizeof(clsid_t::bytes))) {
            rst = iter.first;
            break;
        }
    }
    return rst;
}

int parse_target_id_list(const uint8_t* data, uint32_t size, uint32_t pos, bool little_endian) {
    if (data == nullptr || size <= pos) {
        return BQ::INVALID_ARG;
    }
    
    link_targetid_list_t id_list;
    id_list.size = XsUtils::byteorder2host(*(const uint16_t*)(data+pos), little_endian);
    if ((pos += sizeof(uint16_t)) + id_list.size > size) {
        return BQ::ERR_FORMAT;
    }

    std::vector<link_target_item_id_t> id_vector;
    for (uint32_t list_end = pos + id_list.size; pos < list_end; ) {
        if (pos + sizeof(uint16_t) > list_end) {
            break;
        }

        auto item_data = data + pos;
        auto item_offset = (uint32_t)0;

        auto item_size = XsUtils::byteorder2host(*(const uint16_t*)(item_data + item_offset), little_endian);
        if (item_size == 0) {
            break; /* terminal_id */
        }

        if (pos + item_size > list_end) {
            break; /* size error */
        }

        link_target_item_id_t id_item;
        id_item.size = item_size;
        id_item.type = *(item_data + sizeof(uint16_t));
        item_offset = sizeof(uint16_t) + 1;

        switch (id_item.type_cata()) {
        case virtualPathType_Root: {
            if (id_item.type_data() != 0x0F) {
                break; /* unknown data */
            }

            itemid_root_t& item_root = id_item.data.root;
            item_root.sort_index = *(item_data + sizeof(uint16_t) + 1);
            memcpy(item_root.clsid, item_data + sizeof(uint16_t) + 2, sizeof(item_root.clsid));
            uint32_t clsid_type = get_clsid_type(item_root.clsid, sizeof(item_root.clsid));

            uint32_t item_root_size_min = (const uint8_t*)&item_root.extra_block - (const uint8_t*)&id_item;
            assert(item_root_size_min == 0x14);
            if (id_item.size >= item_root_size_min + 8) {
                auto blockdata = item_data + item_root_size_min;
                itemid_norm_extrablock_t extrablock;
                extrablock.size = XsUtils::byteorder2host(*(const uint16_t*)(blockdata), little_endian);
                extrablock.version = XsUtils::byteorder2host(*(const uint16_t*)(blockdata + 2), little_endian);
                extrablock.signature = XsUtils::byteorder2host(*(const uint32_t*)(blockdata + 4), little_endian);
                extrablock.data[0] = id_item.size - (item_root_size_min + 8);
            }

            break;
        }
        case virtualPathType_Volume: {
            itemid_volume_t& item_volume = id_item.data.volume;
            if ((id_item.type_data() & 0x1) == 0x1) { /* volume_name */
                for (uint8_t i = 0; i < id_item.size - 3 && i < sizeof(item_volume.name) - 1; i++) {
                    item_volume.name[i] = *(item_data + sizeof(uint16_t) + 1 + i);
                    if (item_volume.name[i] == 0) break;
                }
            }
            else if (id_item.size == 0x14) { /* volume_clsid */
                item_volume.sort_index = *(item_data + sizeof(uint16_t) + 1);
                memcpy(item_volume.clsid, item_data + sizeof(uint16_t) + 2, sizeof(item_volume.clsid));
                uint32_t clsid_type = get_clsid_type(item_volume.clsid, sizeof(item_volume.clsid));
            }
            else { /* unknown */
                item_volume.unknown_data[0] = *(item_data + sizeof(uint16_t) + 1);
            }
            break;
        }
        case virtualPathType_File: {
            itemid_file_t& item_file = id_item.data.file; 
            item_file.filesize = XsUtils::byteorder2host(*(uint32_t*)(item_data + item_offset + 1), little_endian);
            item_file.modified = XsUtils::byteorder2host(*(uint32_t*)(item_data + item_offset + 5), little_endian);
            item_file.attributes = XsUtils::byteorder2host(*(uint16_t*)(item_data + item_offset + 9), little_endian);
            item_offset += 11;
            if ((id_item.type_data() & 0x4) == 0x4) { /* unicode */
                for (uint8_t i = 0; i < id_item.size - 14 && i < sizeof(item_file.primary_name) - 1; i += 2) {
                    item_offset += 2;
                    item_file.primary_name[i] = *(item_data + sizeof(uint16_t) * 6 + 2 + i);
                    item_file.primary_name[i+1] = *(item_data + sizeof(uint16_t) * 6 + 3 + i);
                    if (item_file.primary_name[i] == 0 && item_file.primary_name[i+1] == 0) break;
                }
            }
            else { /* ansi */
                for (uint8_t i = 0; i < id_item.size - 14 && i < sizeof(item_file.primary_name) - 1; i++) {
                    item_offset += 1;
                    item_file.primary_name[i] = *(item_data + sizeof(uint16_t) * 6 + 2 + i);
                    if (item_file.primary_name[i] == 0) {
                        item_offset += (item_offset % 2 == 0 ? 0 : 1);
                        break;
                    }
                }
            }

            if (id_item.size >= item_offset + 8) {
                auto blockdata = item_data + item_offset;
                itemid_norm_extrablock_t extrablock;
                extrablock.size = XsUtils::byteorder2host(*(const uint16_t*)(blockdata), little_endian);
                extrablock.version = XsUtils::byteorder2host(*(const uint16_t*)(blockdata + 2), little_endian);
                extrablock.signature = XsUtils::byteorder2host(*(const uint32_t*)(blockdata + 4), little_endian);
                extrablock.data[0] = id_item.size - (item_offset + 8);
            }
            break;
        }
        default: {
            if (id_item.size > 0x18 && *(item_data + item_offset + 0x0D) == 0x6A) {
                itemid_cpl_applet_t item_cpl;
                item_cpl.sort_index = *(item_data + item_offset);
                memcpy(item_cpl.clsid, item_data + item_offset + 1, sizeof(item_cpl.clsid));
                item_offset += sizeof(item_cpl.clsid) + 1;
                item_cpl.path_size = XsUtils::byteorder2host(*(const uint16_t*)(item_data + item_offset), little_endian);
                item_offset += sizeof(item_cpl.path_size);
                item_cpl.display_name_size = XsUtils::byteorder2host(*(const uint16_t*)(item_data + item_offset), little_endian);
                item_offset += sizeof(item_cpl.display_name_size);
            }
            break;
        }
        }

        id_vector.push_back(id_item);
        pos += item_size;
    }

    return -1;
}

int parse_link_info(const uint8_t* data, uint32_t size) {
    return -1;
}

int parse_string_data(const uint8_t* data, uint32_t size) {
    return -1;
}

int parse_extra_data(const uint8_t* data, uint32_t size) {
    return -1;
}

int mshellink::parse(const ctx_t* ctx, ifile* file, ms_shellink_t* lnk) {
    if (file == nullptr || lnk == nullptr) {
        return BQ::INVALID_ARG;
    }

    int nret = BQ::OK;
    uint32_t size = (uint32_t)file->size();
    const uint8_t* data = file->mapping();

    do {
        if (data == nullptr || size < sizeof(lnk->header)) {
            nret = BQ::INVALID_ARG;
            break;
        }

        memcpy(&lnk->header, data, sizeof(lnk->header));
        lnk->header.header_size = XsUtils::byteorder2host(lnk->header.header_size, is_little_endian);
        lnk->header.link_flags = XsUtils::byteorder2host(lnk->header.link_flags, is_little_endian);
        lnk->header.attributes = XsUtils::byteorder2host(lnk->header.attributes, is_little_endian);
        lnk->header.creation_time = XsUtils::byteorder2host(lnk->header.creation_time, is_little_endian);
        lnk->header.write_time = XsUtils::byteorder2host(lnk->header.write_time, is_little_endian);
        lnk->header.access_time = XsUtils::byteorder2host(lnk->header.access_time, is_little_endian);
        lnk->header.filesize = XsUtils::byteorder2host(lnk->header.filesize, is_little_endian);
        lnk->header.icon_index = XsUtils::byteorder2host(lnk->header.icon_index, is_little_endian);
        lnk->header.show_command = XsUtils::byteorder2host(lnk->header.show_command, is_little_endian);
        lnk->header.reserved1 = XsUtils::byteorder2host(lnk->header.reserved1, is_little_endian);
        lnk->header.reserved2 = XsUtils::byteorder2host(lnk->header.reserved2, is_little_endian);
        lnk->header.reserved3 = XsUtils::byteorder2host(lnk->header.reserved3, is_little_endian);

        const uint8_t lnk_clsid[] = MS_LNK_CLSID;
        if (lnk->header.header_size != sizeof(lnk->header) || memcmp(lnk_clsid, lnk->header.clsid, sizeof(lnk->header.clsid))) {
            nret = BQ::ERR_FORMAT;
            break;
        }

        if (lnk->header.link_flags & (1 << (bitLinkFlags_HasLinkTargetIDList))) {
            parse_target_id_list(data, size, lnk->header.header_size, is_little_endian);
        }

        if (lnk->header.link_flags & (1 << (bitLinkFlags_HasLinkInfo))) {
            parse_link_info(data, size);
        }

        nret = parse_string_data(data, size);
        if (nret != BQ::OK) {

        }

        nret = parse_extra_data(data, size);
        if (nret != BQ::OK) {

        }
    } while (false);
    
    return BQ::NOT_IMPLEMENT;
}