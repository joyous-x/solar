#include <cassert>
#include <stack>
#include "ole.h"
#include "ole_utils.h"
#include "../utils/bitmap.h"
#ifndef _N_OFFCRYPTO
#include "../office/offcrypto.h"
#endif

typedef struct __st_ole_type_identifier {
    uint32_t    file_type;
    const char* extensions;
    const char16_t* stream;
    const char* clsid;
    const char* content_type;
} oletype_identifier_t;

oletype_identifier_t ole_identifiers[] = {
    // doc
    oletype_identifier_t{ft_ms_doc, "doc, dot", u"worddocument", "00020906-0000-0000-C000-000000000046", 0}, // Word 97~2003 Document or Template
    oletype_identifier_t{ft_ms_doc, "doc, dot", u"worddocument", "00020900-0000-0000-C000-000000000046", 0}, // Word 6~7 Document or Template
    oletype_identifier_t{ft_ms_docx, "docx", 0, 0, "application/vnd.openxmlformats-officedocument.wordprocessingml.document"},
    oletype_identifier_t{ft_ms_docm, "docm", 0, 0, "application/vnd.ms-word.document.macroEnabled.12"},
    oletype_identifier_t{ft_ms_dotx, "dotx", 0, 0, "application/vnd.openxmlformats-officedocument.wordprocessingml.template"},
    oletype_identifier_t{ft_ms_dotm, "dotm", 0, 0, "application/vnd.ms-word.template.macroEnabled.12"},
    // excel, TODO:  .xlw(Excel 4.0) .xml(2003)
    //> Excel 97-2003 Workbook or Template
    oletype_identifier_t{ft_ms_xls, "xls, xlt, xla", u"book", "00020820-0000-0000-C000-000000000046", 0},
    oletype_identifier_t{ft_ms_xls, "xls, xlt, xla", u"workbook", "00020820-0000-0000-C000-000000000046", 0},
    //> Excel 5.0/95 Workbook, Template or Add-in, this CLSID is also used in Excel addins (.xla) saved by MS Excel 365
    oletype_identifier_t{ft_ms_xls, "xls, xlt, xla", u"book", "00020810-0000-0000-C000-000000000046", 0},
    oletype_identifier_t{ft_ms_xls, "xls, xlt, xla", u"workbook", "00020810-0000-0000-C000-000000000046", 0},
    oletype_identifier_t{ft_ms_xlsx, "xlsx", 0, 0, "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"},
    oletype_identifier_t{ft_ms_xlsm, "xlsm", 0, 0, "application/vnd.ms-excel.sheet.macroEnabled.12"},
    oletype_identifier_t{ft_ms_xltx, "xltx", 0, 0, "application/vnd.openxmlformats-officedocument.spreadsheetml.template"},
    oletype_identifier_t{ft_ms_xltm, "xltm", 0, 0, "application/vnd.ms-excel.template.macroEnabled.12"},
    oletype_identifier_t{ft_ms_xlsb, "xlsb", 0, 0, "application/vnd.ms-excel.sheet.binary.macroEnabled"},
    // ppt
    oletype_identifier_t{ft_ms_ppt, "ppt, pot", u"Current User,PowerPoint Document", 0, 0},
    oletype_identifier_t{ft_ms_pptm, "pptm", 0, 0, "application/vnd.ms-powerpoint.presentation.macroEnabled.12"}, // 97~2003, 2007~
    oletype_identifier_t{ft_ms_pptx, "pptx", 0, 0, "application/vnd.openxmlformats-officedocument.presentationml.presentation"},
    oletype_identifier_t{ft_ms_potx, "potx", 0, 0, "application/vnd.openxmlformats-officedocument.presentationml.template"},
    oletype_identifier_t{ft_ms_potm, "potm", 0, 0, "application/vnd.ms-powerpoint.template.macroEnabled.12"},
};


int make_directory_tree_ex(const cfb_ctx_t* ctx, ole2_directory_entry_t* entries, uint32_t entry_count, entry_tree_item_t** out) {
    if (nullptr == entries || entry_count == 0 || out == nullptr) {
        return BQ::INVALID_ARG;
    }

    entry_tree_item_t* cur_node_parent = nullptr;
    if (entries->obj_type != ole_dir_entry_root_storage) {
        MESSAGE_WARN(ctx, "first entry must be RootEntry");
        return BQ::ERR_FILE_FORMAT;
    }

    auto trees = (entry_tree_item_t*)malloc(sizeof(entry_tree_item_t) * entry_count);
    if (nullptr == trees) {
        return BQ::ERR_MALLOC;
    }
    memset(trees, 0, sizeof(entry_tree_item_t) * entry_count);

    int nret = BQ::OK;
    for (uint32_t i = 0; i < entry_count; i++) {
        (trees + i)->item = entries + i;
    }
    // set child_first and child_first's parent
    for (uint32_t i = 0; i < entry_count; i++) {
        if ((trees + i) == nullptr || (trees + i)->parent || (trees + i)->child_first) {
            continue;
        }

        ole2_directory_entry_t* cur_entry = nullptr;
        entry_tree_item_t* cur_tree = nullptr;
        for (uint32_t cur_did = i; cur_did != OLE_NO_STREAM && cur_did < entry_count; cur_did = cur_entry->child_did) {
            cur_entry = entries + cur_did;
            cur_tree = trees + cur_did;
            if (cur_entry->child_did == 0 || cur_entry->child_did >= entry_count) {
                break;
            }
            if (cur_tree->child_first && cur_tree->child_first->parent) {
                break;
            }
            if (cur_tree->child_first == nullptr) {
                cur_tree->child_first = trees + cur_entry->child_did;
            }
            if (cur_tree->child_first->parent == nullptr) {
                cur_tree->child_first->parent = cur_tree;
            }
        }
    }
    // set all node's parent
    Bitmap walked;
    for (uint32_t i = 0; i < entry_count; i++) {
        if ((trees + i) == nullptr || (trees + i)->child_first == nullptr) {
            continue;
        }

        for (auto child = (trees + i)->child_first; child; child = child->child_first) {
            walked.set(((const uint8_t*)child - (const uint8_t*)trees) / sizeof(entry_tree_item_t));
            auto parent = child->parent;
            auto slibing_chain = child;
            std::stack<uint32_t> siblings;
            siblings.push(child->item->l_sib_did);
            siblings.push(child->item->r_sib_did);
            for (uint32_t cur_did = 0; false == siblings.empty(); ) {
                cur_did = siblings.top(); siblings.pop();
                if (cur_did == 0 || cur_did >= entry_count) {
                    continue;
                }

                if (walked.test(cur_did)) {
                    continue;
                }
                else {
                    walked.set(cur_did);
                }

                if ((trees + cur_did)->parent == nullptr) {
                    (trees + cur_did)->parent = parent;
                }
                if (slibing_chain->slibing_next == nullptr) {
                    slibing_chain->slibing_next = (trees + cur_did);
                    slibing_chain = (trees + cur_did);
                }
                siblings.push((trees + cur_did)->item->l_sib_did);
                siblings.push((trees + cur_did)->item->r_sib_did);
            }
        }
    }
    // set all node's depth
    for (uint32_t i = 0, depth = 0; i < entry_count; i++) {
        if ((trees + i) == nullptr) {
            continue;
        }
        for (auto cur_tree = (trees + i)->parent; cur_tree; cur_tree = cur_tree->parent) {
            (trees + i)->depth++;
        }
    }
    *out = trees;
    return nret;
}


/*
* @NOTE
*    某些情况下，会出现不是所有节点都可以通过 root 遍历到，也就是说会出现多棵树。
*   此时遵循后遍历到的树结构不能修改已经遍历过的节点信息。
*/
int make_directory_tree(cfb_ctx_t* ctx, ole2_directory_entry_t* entries, uint32_t count, uint32_t did, uint32_t depth, Bitmap* walked, entry_tree_item_t* entry_tree) {
    if (nullptr == entries || count == 0 || did >= count) {
        return BQ::INVALID_ARG;
    }

    int nret = BQ::OK;
    entry_tree_item_t* slibing_last = nullptr;
    for (slibing_last = (entry_tree + did); slibing_last && slibing_last->slibing_next; ) {
        slibing_last = slibing_last->slibing_next;
    }

    std::stack<uint32_t> node_list;
    node_list.push(did);
    for (uint32_t cur_index = OLE_NO_STREAM; false == node_list.empty(); ) {
        cur_index = node_list.top(); node_list.pop();
        if (count <= cur_index) {
            continue;
        }

        if (walked->test(cur_index)) {
            continue;
        }
        walked->set(cur_index);

        if ((entry_tree + cur_index)->item == nullptr) {
            (entry_tree + cur_index)->item = entries + cur_index;
            (entry_tree + cur_index)->depth = depth;
            (entry_tree + cur_index)->parent = (entry_tree + did)->parent;
        }

        if (cur_index != did && slibing_last) {
            slibing_last->slibing_next = (entry_tree + cur_index);
            slibing_last = (entry_tree + cur_index);
        }

        const ole2_directory_entry_t* entry = (entry_tree + cur_index)->item;
        switch (entry->obj_type)
        {
        case ole_dir_entry_root_storage:
            if (cur_index != 0) {
                MESSAGE_WARN(ctx, "can only have RootEntry as the top");
                nret = BQ::ERR_FILE_FORMAT;
                break;
            }
        case ole_dir_entry_storage:
            if (entry->child_did == OLE_NO_STREAM) {
                /* no stream */
            }
            else if (entry->child_did < count) {
                (entry_tree + entry->child_did)->parent = entry_tree + cur_index;
                (entry_tree + cur_index)->child_first = entry_tree + entry->child_did;
                nret = make_directory_tree(ctx, entries, count, entry->child_did, depth + 1, walked, entry_tree);
                if (nret != BQ::OK) {
                    MESSAGE_WARN(ctx, "walk property tree error(%08X): %u", nret, entry->child_did);
                    nret = BQ::OK;
                }
            }
            else {
                MESSAGE_WARN(ctx, "invalid child did: %u", entry->child_did);
            }
            break;
        case ole_dir_entry_stream:
            break;
        case ole_dir_entry_property:
            break;
        default:
            break;
        }

        if (entry->l_sib_did != OLE_NO_STREAM) {
            node_list.push(entry->l_sib_did);
        }
        if (entry->r_sib_did != OLE_NO_STREAM) {
            node_list.push(entry->r_sib_did);
        }
    }

    if ((entry_tree + did)->parent && nullptr == (entry_tree + did)->parent->child_first) {
        (entry_tree + did)->parent->child_first = (entry_tree + did);
    }

    return nret;
}

int ole::parse(const cfb_ctx_t* ctx, ifile* pfile, olefile_t* pole) {
    if (pfile == nullptr || pole == nullptr) {
        return BQ::INVALID_ARG;
    }

    olefile_t& olefile = *pole;
    if (olefile.file == nullptr) {
        olefile.file = pfile;
        olefile.file->addref();
    }

    int32_t nret = BQ::OK;
    int64_t filesize = pfile->size();
    int32_t header_size = sizeof(ole2_header_t);

    do {
        if (filesize < header_size) {
            nret = BQ::ERR_FILE_FORMAT;
            break;
        }

        if (0 != pfile->read(0, (unsigned char*)&olefile.header, header_size, 0)) {
            nret = BQ::ERR_READ;
            break;
        }
        OleUtils::correct_byte_order(&olefile.header, OleUtils::little_endian(&olefile.header));

        if (true != OleUtils::check_ole_header(&olefile.header)) {
            return BQ::ERR_OLE_HEADER;
        }

        /* fix illegal min_stream_cutoff */
        if (olefile.header.min_stream_cutoff < (uint32_t)(1 << olefile.header.short_sector_shift) || olefile.header.min_stream_cutoff >(uint32_t)(1 << olefile.header.sector_shift)) {
            olefile.header.min_stream_cutoff = 4096;
        }

        /* check_ole_header 决定了 sector_shift 必须小于 0x1D */
        uint32_t sector_size = (uint32_t)1 << olefile.header.sector_shift;
        uint32_t max_sector_no = uint32_t(filesize / sector_size);
        uint32_t max_short_sector_no = max_sector_no * ((int64_t)1 << (olefile.header.sector_shift - olefile.header.short_sector_shift));
        assert(olefile.header.msat_sector_count < max_sector_no);
        assert(olefile.header.directory_sector_count < max_sector_no);

        if (uint32_t(filesize / 3) < sector_size) {
            nret = BQ::ERR_FILE_FORMAT;
            break;
        }

        /*
        * interesting~
        *    when the sids in msat_table are discontinuous, there is an ambiguity:
        *    1. the tool named offvis thinks them are continuous and parses them in this way.
        *    2. the document ([MS-CFB] - v20210407) thinks them should be what they are.
        * here, they are parsed in method 2.
        */
        nret = OleUtils::make_msat_table(pfile, &olefile.header, &olefile.msat_table, &olefile.msat_table_size);
        if (nret < 0) {
            break;
        }

        nret = OleUtils::make_sat_table(pfile, &olefile.header, olefile.msat_table, olefile.msat_table_size, &olefile.sat_table, &olefile.sat_table_size);
        if (nret < 0) {
            break;
        }

        nret = OleUtils::make_ssat_table(&olefile, &olefile.ssat_table, &olefile.ssat_table_size);
        if (nret < 0) {
            break;
        }

        nret = OleUtils::make_directory_entries(pfile, &olefile.header, olefile.sat_table, olefile.sat_table_size, &olefile.entries, &olefile.entry_count);
        if (nret < 0) {
            break;
        }

#if make_directory_tree_first_version
        olefile.entry_tree = (entry_tree_item_t*)malloc(sizeof(entry_tree_item_t) * olefile.entry_count);
        if (nullptr == olefile.entry_tree) {
            nret = BQ::ERR_MALLOC;
            break;
        }
        memset(olefile.entry_tree, 0, sizeof(entry_tree_item_t) * olefile.entry_count);

        Bitmap walked;
        nret = make_directory_tree(ctx, olefile.entries, olefile.entry_count, 0, 0, &walked, olefile.entry_tree);
        if (nret < 0) {
            break;
        }
        for (uint32_t i = 0; i < olefile.entry_count; i++) {
            if (walked.test(i)) continue;
            (olefile.entry_tree + i)->item = olefile.entries + i;
        }
#else 
        nret = make_directory_tree_ex(ctx, olefile.entries, olefile.entry_count, &olefile.entry_tree);
        if (nret < 0) {
            break;
        }
#endif

        if (olefile.entries->obj_type != ole_dir_entry_root_storage) {
            nret = BQ::ERR_FILE_FORMAT;
            break;
        }
        olefile.ssector_entry_sid = olefile.entries->entry_sid;
    } while (false);

    return nret;
}

int ole::filetype(const cfb_ctx_t* ctx, const olefile_t* ole) {
    int detected = ft_unknown;
    if (ole == nullptr || ole->entry_tree == nullptr) {
        return detected;
    }
    bool little_endian = OleUtils::little_endian(&ole->header);

    for (uint32_t i = 0; i < ole->entry_count; i++) {
        auto cur_entry = (ole->entry_tree + i);
        if (cur_entry == nullptr || cur_entry->depth > 1) {
            continue;
        }

        /*
         * if we can find a root entry fileheader, it may be a HWP file
         * identify the HWP signature "HWP Document File" at offset 0 stream
         */
        if (0 == ucs::icmp<char16_t>(cur_entry->item->name, u"fileheader")) {
            const std::string strHwp5Magic = "HWP Document File";

            hwp5_header_t* hwp5_header = nullptr;
            ON_SCOPE_EXIT([&] { if (hwp5_header) free(hwp5_header); });

            int nret = ole::read_stream(ole, i, sizeof(hwp5_header_t), (uint8_t**)&hwp5_header, 0);
            if (nret == 0 && 0 == memcmp(hwp5_header->signature, strHwp5Magic.c_str(), strHwp5Magic.size())) {
                hwp5_header->version = XsUtils::byteorder2host<int32_t>(hwp5_header->version, little_endian);
                hwp5_header->flags = XsUtils::byteorder2host<int32_t>(hwp5_header->flags, little_endian);
                detected = ft_hwp5;
                break;
            }

            continue;
        }
        else if (0 == ucs::icmp(cur_entry->item->name, u"EncryptionInfo")) {
#ifndef _N_OFFCRYPTO
            auto encrypted = offcrypto::is_ole_encrypted(ctx, ole);
            if (encrypted) {
                detected = ft_ooxml_encrypted;
                break;
            }
#endif
            continue;
        }

        if (detected != ft_unknown) {
            break;
        }
    }

    if (detected != ft_unknown) {
        return detected;
    }

    for (uint32_t j = 0; j < sizeof(ole_identifiers) / sizeof(ole_identifiers[0]); j++) {
        auto identifier = &ole_identifiers[j];
        if (identifier->stream == 0) {
            continue;
        }

        auto found = 0;
        std::vector<std::u16string> streams;
        for (auto stream : ucs::split<std::u16string>(identifier->stream, u",", streams)) {
            for (uint32_t i = 0; i < ole->entry_count; i++) {
                auto cur_entry = (ole->entry_tree + i);
                if (cur_entry == nullptr || cur_entry->depth > 1) {
                    continue;
                }
                if (0 != ucs::icmp(stream.c_str(), cur_entry->item->name)) {
                    continue;
                }
                found++;
                break;
            }
        }
        if (found != streams.size()) {
            continue;
        }
        detected = identifier->file_type;

        std::string clsid;
        if (identifier->clsid && 0 == XsUtils::clsid2str(ole->entries->clsid, 16, little_endian, clsid)
            && 0 != ucs::nicmp<char>(clsid.c_str(), identifier->clsid, sizeof(identifier->clsid))) {
            MESSAGE_WARN(ctx, "[filetype] clsid(%s) don't match with %s \r\n", clsid.c_str(), identifier->extensions);
        }

        break;
    }

    return (detected);
}

int ole::walk_property_tree(const cfb_ctx_t* ctx, olefile_t* pole, uint32_t did, ifilehandler* file_handler, ole_walk_prop_tree_handler handler) {
    if (pole == nullptr || handler == nullptr || pole->sat_table == nullptr || pole->sat_table_size == 0 || did >= pole->entry_count) {
        return BQ::INVALID_ARG;
    }

    int nret = BQ::OK;
    std::stack<entry_tree_item_t*> node_list;

    if ((pole->entry_tree + did)->parent == nullptr) {
        /* 
        * root or orphans
        */
        for (uint32_t i = 0; i < pole->entry_count; i++) {
            if ((pole->entry_tree + i)->parent == nullptr) {
                node_list.push(pole->entry_tree + i);
            }
        }
    }
    else {
        assert((pole->entry_tree + did)->parent->child_first);
        for (auto cur = (pole->entry_tree + did)->parent->child_first; cur; cur = cur->slibing_next) {
            node_list.push(cur);
        }
    }

    for (entry_tree_item_t* cur_item = nullptr; false == node_list.empty(); ) {
        cur_item = node_list.top(); node_list.pop();
        if (nullptr == cur_item) {
            continue;
        }

        uint32_t directory_id = ((uint8_t*)cur_item - (uint8_t*)pole->entry_tree) / sizeof(entry_tree_item_t);
        ole2_directory_entry_t* entry = cur_item->item;
        switch (entry->obj_type)
        {
        case ole_dir_entry_root_storage:
            if (directory_id != 0) {
                MESSAGE_WARN(ctx, "[walk_property_tree] can only have RootEntry as the top\r\n");
                nret = BQ::ERR_FILE_FORMAT;
                break;
            }
        case ole_dir_entry_storage:
            nret = handler(ctx, pole, directory_id, file_handler);
            if (nret != BQ::OK) {
                MESSAGE_WARN(ctx, "[walk_property_tree] storage handler error: %u\r\n", entry->child_did);
                break;
            }
            break;
        case ole_dir_entry_stream:
        case ole_dir_entry_property:
            if (ctx && ctx->max_stream_size && entry->stream_bytes > ctx->max_stream_size) {
                MESSAGE_DEBUG(ctx, "[walk_property_tree] stream size exceeded: %llu\r\n", entry->stream_bytes);
                break;
            }
            nret = handler(ctx, pole, directory_id, file_handler);
            if (nret != BQ::OK) {
                MESSAGE_WARN(ctx, "[walk_property_tree] stream handler error: %u\r\n", entry->child_did);
                break;
            }
            break;
        default:
            break;
        }

        if (nret != BQ::OK) {
            break;
        }

        for (auto cur = cur_item->child_first; cur; cur = cur->slibing_next) {
            node_list.push(cur);
        }
    }

    return nret;
}

//////////////////////////////////////////////////////////////////////
// 
// OLE Read Write
// 
//////////////////////////////////////////////////////////////////////

/**
* @brief read office stream  (ole2 files may not be a block multiple in size)
*
* @param ole
* @param ssid            read will start at this ssid
* @param buff
* @param size            buffer size. read 'size' bytes from the stream beginning。
*                        if stream_size == -1, read the stream until ENDOFCHAIN
* @param readed         if buff == nullptr, return the size needed。otherwise，return the size readed。
*/
int read_norm_stream(const olefile_t* ole, uint32_t sid, uint8_t* buff, uint32_t size, uint32_t* readed) {
    if (ole == nullptr || ole->file == nullptr || ole->header.sector_shift == 0 || (size && buff == nullptr)) {
        return BQ::INVALID_ARG;
    }
    if (OLE_ENDOFCHAIN == sid) {
        if (readed) {
            *readed = 0;
        }
        return BQ::OK;
    }
    if (is_special_ole_sid(sid)) {
        return BQ::ERR_OLE_SID;
    }
    uint32_t sector_size = (uint32_t)1 << ole->header.sector_shift;
    bool little_endian = OleUtils::little_endian(&ole->header);

    std::vector<uint32_t> sid_chain;
    OleUtils::make_sid_chain(ole->sat_table, ole->sat_table_size, sid, sid_chain, &little_endian);

    if (buff == nullptr) {
        if (readed) {
            *readed = sid_chain.size() * sector_size;
        }
        return BQ::OK;
    }

    uint8_t* bytes_buff = buff;
    uint32_t bytes_size = std::min<uint32_t>(size, sid_chain.size() * sector_size);
    memset(bytes_buff, 0, bytes_size);

    int nret = BQ::OK;
    for (uint32_t i = 0, has_read = 0, left = bytes_size; left && i < sid_chain.size(); i++, left -= has_read) {
        nret = ole->file->read(((int64_t)sid_chain[i] + 1) * sector_size, bytes_buff + bytes_size - left, std::min(left, sector_size), &has_read);
        if (nret != 0) {
            nret = BQ::ERR_READ;
            break;
        }
        if (readed) {
            *readed += has_read;
        }
    }

    return nret;
}

/**
* @brief read office stream  (ole2 files may not be a block multiple in size)
*
* @param ole
* @param ssid            read will start at this ssid
* @param buff
* @param size            buffer size. read 'size' bytes from the stream beginning。
*                        if stream_size == -1, read the stream until ENDOFCHAIN
* @param readed         if buff == nullptr, return the size needed。otherwise，return the size readed。
*/
int read_short_stream(const olefile_t* ole, uint32_t ssid, uint8_t* buff, uint32_t size, uint32_t* readed) {
    if (ole == nullptr || ole->file == nullptr || ole->header.sector_shift == 0 || ole->header.short_sector_shift == 0) {
        return BQ::INVALID_ARG;
    }
    if (size && buff == nullptr) {
        return BQ::INVALID_ARG;
    }
    if (OLE_ENDOFCHAIN == ssid) {
        if (readed) {
            *readed = 0;
        }
        return BQ::OK;
    }
    if (is_special_ole_sid(ssid) || is_special_ole_sid(ole->ssector_entry_sid)) {
        return BQ::ERR_OLE_SID;
    }

    uint32_t sector_size = (uint32_t)1 << ole->header.sector_shift;
    uint32_t short_sector_size = (uint32_t)1 << ole->header.short_sector_shift;
    uint32_t short_per_sector = (uint32_t)1 << (ole->header.sector_shift - ole->header.short_sector_shift);
    bool little_endian = OleUtils::little_endian(&ole->header);

    std::vector<uint32_t> ssid_chain;
    OleUtils::make_sid_chain(ole->ssat_table, ole->ssat_table_size, ssid, ssid_chain, &little_endian);

    std::vector<uint32_t> sid_chain;
    OleUtils::make_sid_chain(ole->sat_table, ole->sat_table_size, ole->ssector_entry_sid, sid_chain, &little_endian);

    if (buff == nullptr) {
        if (readed) {
            *readed = ssid_chain.size() * short_sector_size;
        }
        return BQ::OK;
    }

    uint8_t* bytes_buff = buff;
    uint32_t bytes_size = std::min<uint32_t>(size, ssid_chain.size() * short_sector_size);
    memset(bytes_buff, 0, bytes_size);

    int nret = BQ::OK;
    for (uint32_t i = 0, sid = 0, offset = 0, has_read = 0, left = bytes_size; left && i < ssid_chain.size(); i++, left -= has_read) {
        sid = ssid_chain[i] / short_per_sector;
        offset = ssid_chain[i] % short_per_sector;
        if (sid >= sid_chain.size()) {
            nret = BQ::ERR_OLE_SID;
            break;
        }
        sid = sid_chain[sid];

        uint32_t byte_start = (sid + 1) * sector_size + offset * short_sector_size;
        nret = ole->file->read(byte_start, bytes_buff + bytes_size - left, std::min(left, short_sector_size), &has_read);
        if (nret != 0) {
            nret = BQ::ERR_READ;
            break;
        }
        if (readed) {
            *readed += has_read;
        }
    }

    return nret;
}

int write_norm_stream(olefile_t* ole, uint32_t sid, uint32_t start, const uint8_t* buf, uint32_t& writted) {
    if (ole == nullptr || ole->file == nullptr || ole->header.sector_shift == 0 || buf == nullptr) {
        return BQ::INVALID_ARG;
    }
    if (OLE_ENDOFCHAIN == sid) {
        writted = 0;
        return BQ::OK;
    }
    if (is_special_ole_sid(sid)) {
        return BQ::ERR_OLE_SID;
    }
    uint32_t sector_size = (uint32_t)1 << ole->header.sector_shift;
    bool little_endian = OleUtils::little_endian(&ole->header);

    std::vector<uint32_t> sid_chain;
    OleUtils::make_sid_chain(ole->sat_table, ole->sat_table_size, sid, sid_chain, &little_endian);

    uint32_t done = 0, nret = BQ::ERR;
    for (uint32_t i = 0, pos = 0, want = writted; i < sid_chain.size() && done < want; i++) {
        uint32_t ss_offset = 0;
        uint32_t to_prcess = std::min(want - done, sector_size);

        if (pos + to_prcess < start) {
            pos += to_prcess;
            continue;
        }
        else if (pos < start) {
            ss_offset = start - pos;
            to_prcess = to_prcess - ss_offset;
        }

        uint32_t byte_start = (sid_chain[i] + 1) * sector_size;
        nret = ole->file->write((uint64_t)byte_start + ss_offset, buf + done, to_prcess);
        if (nret != 0) {
            nret = BQ::ERR_READ;
            break;
        }
        pos += to_prcess + ss_offset;
        done += to_prcess;
    }

    if (nret == BQ::OK) {
        writted = done;
    }

    return nret;
}

int write_short_stream(olefile_t* ole, uint32_t ssid, uint32_t start, const uint8_t* buf, uint32_t& writted) {
    if (ole == nullptr || ole->file == nullptr || ole->header.sector_shift == 0 || ole->header.short_sector_shift == 0) {
        return BQ::INVALID_ARG;
    }
    if (OLE_ENDOFCHAIN == ssid) {
        writted = 0;
        return BQ::OK;
    }
    if (is_special_ole_sid(ssid) || is_special_ole_sid(ole->ssector_entry_sid)) {
        return BQ::ERR_OLE_SID;
    }

    uint32_t sector_size = (uint32_t)1 << ole->header.sector_shift;
    uint32_t short_sector_size = (uint32_t)1 << ole->header.short_sector_shift;
    uint32_t short_per_sector = (uint32_t)1 << (ole->header.sector_shift - ole->header.short_sector_shift);
    bool little_endian = OleUtils::little_endian(&ole->header);

    std::vector<uint32_t> ssid_chain;
    OleUtils::make_sid_chain(ole->ssat_table, ole->ssat_table_size, ssid, ssid_chain, &little_endian);

    std::vector<uint32_t> sid_chain;
    OleUtils::make_sid_chain(ole->sat_table, ole->sat_table_size, ole->ssector_entry_sid, sid_chain, &little_endian);

    uint32_t done = 0, nret = BQ::ERR;
    for (uint32_t i = 0, sid = 0, sid_offset = 0, pos = 0, want = writted; i < ssid_chain.size() && done < want; i++) {
        sid = ssid_chain[i] / short_per_sector;
        sid_offset = ssid_chain[i] % short_per_sector;
        if (sid >= sid_chain.size()) {
            nret = BQ::ERR_OLE_SID;
            break;
        }
        sid = sid_chain[sid];

        uint32_t ss_offset = 0;
        uint32_t to_prcess = std::min(want - done, short_sector_size);

        if (pos + to_prcess < start) {
            pos += to_prcess;
            continue;
        }
        else if (pos < start) {
            ss_offset = start - pos;
            to_prcess = to_prcess - ss_offset;
        }

        uint32_t byte_start = (sid + 1) * sector_size + sid_offset * short_sector_size;
        nret = ole->file->write((uint64_t)byte_start + ss_offset, buf + done, to_prcess);
        if (nret != 0) {
            nret = BQ::ERR_READ;
            break;
        }
        pos += to_prcess + ss_offset;
        done += to_prcess;
    }

    if (nret == BQ::OK) {
        writted = done;
    }

    return nret;
}

int ole::read_stream(const olefile_t* ole, uint32_t did, uint32_t want, uint8_t** buff, uint32_t* got) {
    if (ole == nullptr || buff == nullptr || did >= ole->entry_count) {
        return BQ::INVALID_ARG;
    }

    int nret = BQ::ERR;
    const ole2_directory_entry_t* entry = ole->entries + did;
    if (want == (uint32_t)-1) {
        want = (uint32_t)entry->stream_bytes;
    }

    if (want == 0) {
        *buff = nullptr;
        if (got) *got = 0;
        return BQ::OK;
    }

    uint8_t* bytes_buff = (uint8_t*)malloc(want);
    if (nullptr == bytes_buff) {
        return BQ::ERR_MALLOC;
    }
    memset(bytes_buff, 0, want);
    ON_SCOPE_EXIT([&]() { if (BQ::OK != nret) { if (bytes_buff) free(bytes_buff); if (got) *got = 0; } });

    if (entry->stream_bytes < ole->header.min_stream_cutoff) {
        nret = read_short_stream(ole, entry->entry_sid, bytes_buff, want, got);
    }
    else {
        nret = read_norm_stream(ole, entry->entry_sid, bytes_buff, want, got);
    }

    if (nret == BQ::OK) {
        *buff = bytes_buff;
    }
    return nret;
}

int ole::read_stream(const olefile_t* ole, uint32_t did, uint8_t* buff, uint32_t size, uint32_t* readed) {
    if (ole == nullptr || buff == nullptr || did >= ole->entry_count) {
        return BQ::INVALID_ARG;
    }

    int nret = BQ::ERR;
    const ole2_directory_entry_t* entry = ole->entries + did;

    if (entry->stream_bytes < ole->header.min_stream_cutoff) {
        nret = read_short_stream(ole, entry->entry_sid, buff, size, readed);
    }
    else {
        nret = read_norm_stream(ole, entry->entry_sid, buff, size, readed);
    }

    return nret;
}

int ole::rewrite_stream(olefile_t* ole, uint32_t did, const uint8_t* bytes, uint32_t size) {
    if (ole == nullptr || did == 0 || did >= ole->entry_count) {
        return BQ::INVALID_ARG;
    }

    int nret = 0;
    ole2_directory_entry_t* entry = ole->entries + did;

    uint8_t* new_bytes = nullptr;
    ON_SCOPE_EXIT([&]() { if (new_bytes) free(new_bytes); });

    /*
    * NOTE:
    *   if we set the stream to all zero or set the stream size to zero，it may happen that
    *   the MS-OFFICE software give a hint "macro xxx is broken". so, we should compress
    *   the all zero bytes with rle before writing or update the dir stream before setting
    *   the stream size to zero.
    */

    for (int i = 0; i == 0 && size == 0; i++) {
        /*
        * @brief  delete stream  METHOD 1
        * 1. keep the stream size and set the stream to all 0
        */
        new_bytes = (uint8_t*)malloc(entry->stream_bytes);
        if (new_bytes) {
            memset(new_bytes, 0, entry->stream_bytes);
            bytes = new_bytes;
            size = entry->stream_bytes;
        }
    }

    for (int i = 0; i == 0 && size == 0; i++) {
        /*
        * @brief  delete stream  METHOD 1
        *
        * 1. directory entry item : resize to 0
        * 2. sat\ssat chain: break the chain(eg. change a->b->c to a->END b->c)
        * 3. stream\short-stream : reset sector to 0 (eg. reset sector a to 0)
        *
        * @note: renaming will cause the reference relationship error.
        */
        const char16_t* suffix = u".del";
        std::u16string new_name = entry->name;
        if (ucs::len(suffix) + new_name.length() >= OLE_DIRECTOR_ENTRY_NAME_SIZE) {
            new_name = new_name.substr(0, OLE_DIRECTOR_ENTRY_NAME_SIZE - ucs::len(suffix) - 1);
        }
        new_name += suffix;

        nret = OleUtils::memset_sector(ole, entry->entry_sid, entry->stream_bytes < ole->header.min_stream_cutoff, 0);
        if (nret != 0) {
            break;
        }

        /* we want stream_bytes is set to 0, so just ignore whether stream or short-stream. */
        uint64_t stream_bytes = 0;
        nret = OleUtils::set_directory_entry(ole, did, 0 /*new_name.c_str()*/, &stream_bytes, 0, 0, 0);
        break;
    }

    /*
    * @brief  update stream
    */
    for (int i = 0; i == 0 && size > 0 && bytes; i++) {
        if (entry->stream_bytes < (uint64_t)size) {
            nret = BQ::INVALID_ARG;
            break;
        }

        if (entry->stream_bytes < ole->header.min_stream_cutoff) {
            nret = write_short_stream(ole, entry->entry_sid, 0, bytes, size);
        }
        else {
            nret = write_norm_stream(ole, entry->entry_sid, 0, bytes, size);
        }

        break;
    }

    return nret;
}
