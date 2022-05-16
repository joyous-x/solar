#include <cassert>
#include "ms_ppt.h"
#include "../utils/hash.h"
#include "../utils/scope_guard.h"
#include "../office/ole_utils.h"
#include "../office/offcrypto_assist.h"
#include "./helper.h"

enum PptRecordType {
    // Top-Level record
    RT_Document = 0x03E8,    // DocumentContainer
    MasterOrSlideContainer,  // MainMasterContainer or SlideContainer
    RT_MainMaster = 0x03F8,  // MainMasterContainer
    RT_Handout = 0x0FC9,     // HandoutContainer
    RT_Slide = 0x03EE,       // SlideContainer
    RT_Notes = 0x03F0,       // NotesContainer
    RT_ExternalOleObjectStg = 0x1011, // ExternalObjectStorage
    ExOleObjStg,             // specified by RT_ExternalOleObjectStg
    ExControlStg,            // specified by RT_ExternalOleObjectStg
    VbaProjectStg,           // specified by RT_ExternalOleObjectStg
    RT_PersistDirectoryAtom = 0x1772,
    RT_UserEditAtom = 0x0FF5,
    // a little attention 
    RT_CurrentUserAtom = 0x0ff6,
    RT_CryptSession10Container = 0x2f14,
    RT_List = 0x07D0,        // DocInfoListContainer
    RT_VbaInfo = 0x03FF,     // VbaInfoContainer
    RT_VbaInfoAtom = 0x0400, // VBAInfoAtom
    ExObjListContainer = 0x0409, // ExObjListContainer
    RT_SoundCollection = 0x07E4, // SoundCollectionContainer
    RT_DrawingGroup = 0x040B,    // DrawingGroupContainer
    // external object types
    RT_ExternalMediaAtom = 0x1004,
    RT_ExternalAviMovie = 0x1006,
    RT_ExternalMciMovie = 0x1007,
    RT_ExternalMidiAudio = 0x100d,
    RT_ExternalCdAudio = 0x100E,
    RT_ExternalWavAudioEmbedded = 0x100F,
    RT_ExternalWavAudioLink = 0x1010,
    RT_ExternalOleControl = 0x0FEE,     // ExControlContainer
    RT_ExternalHyperlink = 0x0FD7,      // ExHyperlinkContainer
    RT_MetaFile = 0x0FC1,
    RT_ExternalOleObjectAtom = 0x0FC3,  // ExOleObjAtom
    RT_ExternalOleEmbed = 0x0FCC,       // ExOleEmbedContainer
    RT_ExternalOleEmbedAtom = 0x0FCD,   // ExOleEmbedAtom
    RT_ExternalOleLink = 0x0FCE,        // ExOleLinkContainer
    RT_ExternalObjectListAtom = 0x040A, // ExObjListAtom
    // other types
    RT_DocumentAtom = 0x03e9, // DocumentAtom
    EndDocumentAtom = 0x03ea,
    MetafileBlob = 0x0fc1,
    FontEmbedDataBlob = 0x0fb8,
    RT_Sound = 0x07E6,        // SoundContainer
    RT_SoundDataBlob = 0x07E7,
    RT_BinaryTagDataBlob = 0x138B,
    RT_CString = 0x0FBA,
};


bool withinCheck(unsigned long num, unsigned long target, unsigned long range) {
    return (num >= (target - range) && num <= (target + range));
}

std::string msppt_version(unsigned short useredit_atom_version) {
    std::string ans = "Unknown";

    auto version = useredit_atom_version;
    if (version == 0) {
        ans = "Non-Office";
    }
    else if (withinCheck(version, 4569, 10)) {
        ans = "Office 2013 SP1";
    }
    else if (withinCheck(version, 4569, 10)) {
        ans = "Office 2013 SP1";
    }
    else if (withinCheck(version, 4420, 10)) {
        ans = "Office 2013 RTM";
    }
    else if (withinCheck(version, 4128, 10)) {
        ans = "Office 2013 Preview";
    }
    else if (withinCheck(version, 7015, 10)) {
        ans = "Office 2010 SP2"; 	//14.0.7015.1000
    }
    else if (withinCheck(version, 6029, 10)) {
        ans = "Office 2010 SP1"; 	//14.0.6029.1000
    }
    else if (withinCheck(version, 4763, 10)) {
        ans = "Office 2010 RTM"; 	//14.0.4763.1000
    }
    else if (withinCheck(version, 6425, 10)) {
        ans = "Office 2007 SP2";    //12.0.6425.1000
    }
    else if (withinCheck(version, 6211, 10)) {
        ans = "Office 2007 SP1";    //12.0.6211.1000
    }
    else if (withinCheck(version, 4518, 10)) {
        ans = "Office 2007 RTM";    //12.0.6425.1014
    }
    else if (withinCheck(version, 8173, 10)) {
        ans = "Office 2003 SP3"; 	// 11.0.8169.?
    }
    else if (withinCheck(version, 7969, 10)) {
        ans = "Office 2003 SP2"; 	// 11.0.6564.6568
    }
    else if (withinCheck(version, 6361, 10)) {
        ans = "Office 2003 SP1"; 	// 11.0.6361.0
    }
    else if (withinCheck(version, 5529, 10)) {
        ans = "Office 2003 RTM"; 	// 11.0.5529.0
    }
    else if (withinCheck(version, 6501, 10)) {
        ans = "Office XP SP3"; 		// 10.0.6501.6735
    }
    else if (withinCheck(version, 4205, 10)) {
        ans = "Office XP SP2"; 		// 10.0.4205.0
    }
    else if (withinCheck(version, 3506, 10)) {
        ans = "Office XP SP1"; 		// 10.0.3506.0
    }
    else if (withinCheck(version, 2623, 10)) {
        ans = "Office XP RTM"; 		// 10.0.2623.0
    }
    else if (withinCheck(version, 6620, 10)) {
        ans = "Office 2000 SP3"; 	// 9.0.6620
    }
    else if (withinCheck(version, 4527, 10)) {
        ans = "Office 2000 SP2"; 	// 9.0.4527
    }
    else if (withinCheck(version, 3821, 10)) {
        ans = "Office 2000 SP1"; 	// 9.0.3821
    }
    else if (withinCheck(version, 2716, 10)) {
        ans = "Office 2000 RTM"; 	// 9.0.2716
    }
    else if (withinCheck(version, 3516, 10)) {
        ans = "Office 97 RTM"; 		// 8.0.3516
    }
    else if (withinCheck(version, 4208, 10)) {
        ans = "Office 97 SR1"; 		// 8.0.4208
    }
    else if (withinCheck(version, 5507, 10)) {
        ans = "Office 97 SR2"; 		// 8.0.5507
    }
    return ans;
}

auto read_recordheader = [](const uint8_t* data, uint32_t size, recordheader_t& header, bool is_little_endian) {
    if (size < sizeof(header) || data == nullptr) {
        return BQ::INVALID_ARG;
    }
    memcpy(&header, data, sizeof(header));
    header.recType = XsUtils::byteorder2host<uint16_t>(header.recType, is_little_endian);
    header.recLen = XsUtils::byteorder2host<uint32_t>(header.recLen, is_little_endian);
    header.recVerAndInstance = XsUtils::byteorder2host<uint16_t>(header.recVerAndInstance, is_little_endian);
    return BQ::OK;
};

auto read_useredit_atom = [](const uint8_t* data, uint32_t size, useredit_atom_t& useredit, bool is_little_endian) {
    uint32_t useredit_atom_size_min = sizeof(useredit_atom_t) - sizeof(uint32_t);
    if (size < useredit_atom_size_min || data == nullptr) {
        return BQ::INVALID_ARG;
    }
    memcpy(&useredit, data, useredit_atom_size_min);
    useredit.header.recType = XsUtils::byteorder2host<uint16_t>(useredit.header.recType, is_little_endian);
    useredit.header.recLen = XsUtils::byteorder2host<uint32_t>(useredit.header.recLen, is_little_endian);
    useredit.header.recVerAndInstance = XsUtils::byteorder2host<uint16_t>(useredit.header.recVerAndInstance, is_little_endian);
    (void)useredit.version;
    (void)useredit.unused;
    useredit.last_view = XsUtils::byteorder2host<uint16_t>(useredit.last_view, is_little_endian);
    useredit.last_slide_id_ref = XsUtils::byteorder2host<uint32_t>(useredit.last_slide_id_ref, is_little_endian);
    useredit.offset_last_edit = XsUtils::byteorder2host<uint32_t>(useredit.offset_last_edit, is_little_endian);
    useredit.offset_persist_dir = XsUtils::byteorder2host<uint32_t>(useredit.offset_persist_dir, is_little_endian);
    useredit.doc_persist_id_ref = XsUtils::byteorder2host<uint32_t>(useredit.doc_persist_id_ref, is_little_endian);
    useredit.persist_id_seed = XsUtils::byteorder2host<uint32_t>(useredit.persist_id_seed, is_little_endian);
    if (useredit.header.recLen + sizeof(recordheader_t) == sizeof(useredit_atom_t)) {
        auto tmp = *(uint32_t*)(data + useredit_atom_size_min);
        useredit.encrypt_session_persist_id_ref = XsUtils::byteorder2host<uint32_t>(tmp, is_little_endian);
    }
    uint32_t expect_size = useredit.header.recLen + sizeof(recordheader_t);
    assert(expect_size == useredit_atom_size_min || expect_size == useredit_atom_size_min + sizeof(uint32_t));
    assert(useredit.offset_persist_dir > useredit.offset_last_edit);
    assert(useredit.header.recType == RT_UserEditAtom);
    return BQ::OK;
};

int parse_stream_currentuser(const olefile_t* olefile, uint32_t did, current_user_atom_t* atom, bool is_little_endian = true) {
    if (olefile == nullptr || did > olefile->entry_count || nullptr == atom) {
        return BQ::INVALID_ARG;
    }

    uint32_t current_user_atom_size_min = (uint8_t*)&atom->ansi_username - (uint8_t*)atom;
    if ((olefile->entries + did)->stream_bytes <= current_user_atom_size_min) {
        return BQ::ERR_FILE_FORMAT;
    }

    uint8_t* data = nullptr;
    uint32_t data_size = 0;
    ON_SCOPE_EXIT([&] { if (data) free(data); });

    int nret = ole::read_stream(olefile, did, -1, &data, &data_size);
    if (nret != BQ::OK) {
        return nret;
    }

    memcpy((void*)atom, data, current_user_atom_size_min);
    atom->header.recType = XsUtils::byteorder2host<uint16_t>(atom->header.recType, is_little_endian);
    atom->header.recLen = XsUtils::byteorder2host<uint32_t>(atom->header.recLen, is_little_endian);
    atom->header.recVerAndInstance = XsUtils::byteorder2host<uint16_t>(atom->header.recVerAndInstance, is_little_endian);
    atom->size = XsUtils::byteorder2host<uint32_t>(atom->size, is_little_endian);
    atom->header_token = XsUtils::byteorder2host<uint32_t>(atom->header_token, is_little_endian);
    atom->offset_currentedit = XsUtils::byteorder2host<uint32_t>(atom->offset_currentedit, is_little_endian);
    atom->len_username = XsUtils::byteorder2host<uint16_t>(atom->len_username, is_little_endian);
    atom->file_version = XsUtils::byteorder2host<uint16_t>(atom->file_version, is_little_endian);
    assert(atom->header.recType == RT_CurrentUserAtom);
    assert(atom->header.recLen >= current_user_atom_size_min - sizeof(atom->header));
    assert(atom->header_token == 0xE391C05F || atom->header_token == 0xF3D1C4DF);
    assert(atom->file_version == 0x03F4 && atom->major_version == 0x03 && atom->minor_version == 0x00);
    
    if (data_size < current_user_atom_size_min + atom->len_username + sizeof(uint32_t)) {
        return BQ::ERR_FILE_FORMAT;
    }
    memset(atom->ansi_username, 0, sizeof(atom->ansi_username));
    memcpy(atom->ansi_username, data + current_user_atom_size_min, atom->len_username);
    atom->rel_version = *(uint32_t*)(data + current_user_atom_size_min + atom->len_username);
    atom->rel_version = XsUtils::byteorder2host<uint16_t>(atom->rel_version, is_little_endian);

    uint8_t buffer[512] = { 0 };
    if (data_size >= current_user_atom_size_min + atom->len_username * 3 + sizeof(uint32_t)) {
        memcpy(buffer, data + current_user_atom_size_min + atom->len_username + sizeof(uint32_t), atom->len_username * 2);
    }

    return BQ::OK;
}

int parse_persist_object_directory(const uint8_t* data, uint32_t size, uint32_t offset_currentedit, bool little_endian, useredit_atom_t* fir_useredit, std::vector<persist_dir_entry_item_t>& dirs) {
    if (data == nullptr || size == 0 || offset_currentedit >= size) {
        return BQ::INVALID_ARG;
    }
    int nret = BQ::OK;

    /*
    * Part 1: Construct the persist object directory
    */
    useredit_atom_t useredit;
    for (uint32_t offset = offset_currentedit; offset && (offset + sizeof(useredit_atom_t) - sizeof(uint32_t) <= size); offset = useredit.offset_last_edit) {
        /* a. Read the UserEditAtom record */
        nret = read_useredit_atom(data + offset, size > offset ? size - offset : 0, useredit, little_endian);
        if (nret != BQ::OK) {
            break;
        }
        assert(useredit.offset_persist_dir < offset);

        if (fir_useredit != nullptr && offset == offset_currentedit) {
            memcpy((void*)fir_useredit, &useredit, sizeof(useredit));
        }

        /* b. Read the PersistDirectoryAtom record */
        if (size < useredit.offset_persist_dir + sizeof(recordheader_t)) {
            break;
        }
        persist_dir_atom_t persist_dir;
        nret = read_recordheader(data + useredit.offset_persist_dir, sizeof(recordheader_t), persist_dir.header, little_endian);
        if (nret != BQ::OK || size < useredit.offset_persist_dir + sizeof(recordheader_t) + persist_dir.header.recLen) {
            break;
        }
        assert(persist_dir.header.recType == RT_PersistDirectoryAtom);

        for (uint32_t pos = 0, offset = useredit.offset_persist_dir + sizeof(recordheader_t), max_len = persist_dir.header.recLen; pos < max_len; ) {
            persist_dir_entry_t dir_entry;
            dir_entry.persist_id_cnt = XsUtils::byteorder2host<uint32_t>(*(uint32_t*)(data + offset + pos), little_endian);
            pos += sizeof(uint32_t);

            for (uint32_t count = 0, persist_offset = 0; count < dir_entry.cnt() && pos < max_len; count++, pos += sizeof(uint32_t)) {
                persist_offset = XsUtils::byteorder2host<uint32_t>(*(uint32_t*)(data + offset + pos), little_endian);
                dirs.push_back(persist_dir_entry_item_t{ dir_entry.id() + count, persist_offset });
            }
        }
    }

    return nret;
}

int parse_cryptsession10container_record(const uint8_t* data, uint32_t size, bool little_endian, crypt_session10container_t* crypt) {
    if (data == nullptr || size == 0 || crypt == nullptr || size < sizeof(crypt->header)) {
        return BQ::INVALID_ARG;
    }

    int nret = read_recordheader(data, sizeof(recordheader_t), crypt->header, little_endian);
    if (nret != BQ::OK) {
        return nret;
    }

    if (crypt->header.recType != RT_CryptSession10Container) {
        return BQ::ERR_OFFICE_CRYPT_FORMAT;
    }

    nret = crypto_binary().parse_rc4capi_encryption_header(data + sizeof(crypt->header), size - sizeof(crypt->header), little_endian, &crypt->rc4_capi);
    if (nret != BQ::OK) {
        return nret;
    }

    return nret;
}

int parse_stream_document(const olefile_t* ole, uint32_t did, msppt_t* ppt) {
    if (ole == nullptr || ppt == nullptr) {
        return BQ::INVALID_ARG;
    }

    uint8_t* data = nullptr;
    uint32_t size = 0;
    ON_SCOPE_EXIT([&] { if (data) free(data); });

    int nret = ole::read_stream(ole, did, -1, &data, &size);
    if (nret != BQ::OK) {
        return nret;
    }

    std::vector<persist_dir_entry_item_t> dirs;
    nret = parse_persist_object_directory(data, size, ppt->currentuser.offset_currentedit, !ppt->big_endian, &ppt->useredit, dirs);
    if (nret != BQ::OK) {
        return nret;
    }
    if (dirs.size() > 0) {
        uint32_t buffer_size = dirs.size() * sizeof(persist_dir_entry_item_t);
        uint8_t* buffer = (uint8_t*)malloc(buffer_size);
        if (buffer == nullptr) {
            return BQ::ERR_MALLOC;
        }
        memset(buffer, 0, buffer_size);
        for (uint32_t i = 0, s = sizeof(persist_dir_entry_item_t); i < dirs.size(); i++) {
            memcpy((void*)(buffer + i * s), &dirs[i], s);
        }
        ppt->persist_dirs.entries = (persist_dir_entry_item_t*)buffer;
        ppt->persist_dirs.entries_size = dirs.size();
    }

    if (ppt->encrypted()) {
        uint32_t offset_CryptSession10Container = 0;
        for (auto& iter : dirs) {
            if (iter.persist_id == ppt->useredit.encrypt_session_persist_id_ref) {
                offset_CryptSession10Container = iter.persist_offset;
                break;
            }
        }
        if (offset_CryptSession10Container == 0) {
            nret = BQ::ERR_OFFICE_CRYPT_INFO;
        }
        else {
            auto data_start = data + offset_CryptSession10Container;
            auto data_size = size > offset_CryptSession10Container ? size - offset_CryptSession10Container : 0;
            nret = parse_cryptsession10container_record(data_start, data_size, !ppt->big_endian, &ppt->crypt);
        }
    }

    return nret;
}

int decrypt_powerpoint_document(const cfb_ctx_t* ctx, const msppt_t* ppt, bin_encryption_t* encryption, const char16_t* passwd, uint8_t* data, uint32_t size) {
    int nret = BQ::OK;

    recordheader_t header;
    for (uint32_t i = 0; i < ppt->persist_dirs.entries_size; i++) {
        auto persist_id = (ppt->persist_dirs.entries + i)->persist_id;
        auto persist_offset = (ppt->persist_dirs.entries + i)->persist_offset;
        if (persist_offset >= size) {
            break;
        }
        nret = read_recordheader(data + persist_offset, size - persist_offset, header, !ppt->big_endian);
        if (nret != BQ::OK) {
            break;
        }

        if (header.recType == RT_CryptSession10Container) {
            memset(data + persist_offset + sizeof(header), 0, header.recLen);
            continue;
        }

        if (header.recType == RT_UserEditAtom || header.recType == RT_PersistDirectoryAtom) {
            continue;
        }

        if (i + 1 < ppt->persist_dirs.entries_size) {
            auto next_offset = (ppt->persist_dirs.entries + i + 1)->persist_offset;
            if (next_offset < persist_offset + sizeof(header)) {
                nret = BQ::ERR_OFFICE_CRYPT_FORMAT;
                break;
            }
            header.recLen = next_offset - persist_offset - sizeof(header);
        }
        else {
            // something error
            break;
        }

        if (size - persist_offset < header.recLen + sizeof(header)) {
            nret = BQ::ERR_OFFICE_CRYPT_FORMAT;
            break;
        }

        crypto_binary::block_info_t block;
        block.block_size = header.recLen + sizeof(header);
        block.is_no_constant = true;
        block.no_constant = persist_id;
        nret = crypto_binary().decrypt_inplace(encryption, passwd, data + persist_offset, block.block_size, &block);
        if (nret != BQ::OK) {
            break;
        }

#if _DEBUG
        {
            Hash md5("md5");
            auto msg = md5.add(data + persist_offset, block.block_size).finalize().bytes();
            MESSAGE_WARN(ctx, "[decrypt_powerpoint_document] offset(%08X) len(%lu): %s \r\n", persist_offset, block.block_size, XsUtils::hex2str(msg, 16).c_str());
        }
#endif
    }

    return nret;
}

int msppt::parse(const cfb_ctx_t* ctx, const olefile_t* olefile, msppt_t* ppt) {
    if (olefile == nullptr || ppt == nullptr) {
        return BQ::INVALID_ARG;
    }

    /*
    * a ppt file MUST have the following structure:
    *   + Current User Stream
    *       - CurrentUserAtom record
    *   + PowerPoint Document Stream
    *       - DocumentContainer record (maybe encrypted)
    *       - PersistDirectoryAtom record
    *       - UserEditAtom record
    */
    const char16_t* current_user_stream = u"Current User";
    const char16_t* ppt_document_stream = u"PowerPoint Document";
    
    for (uint32_t i = 0; i < olefile->entry_count; i++) {
        auto cur_entry = (olefile->entry_tree + i);
        if (cur_entry == nullptr || cur_entry->depth > 1) {
            continue;
        }
        if (ppt->did_currentuser == 0 && 0 == ucs::icmp(cur_entry->item->name, current_user_stream)) {
            ppt->did_currentuser = i;
        }
        if (ppt->did_document == 0 && 0 == ucs::icmp(cur_entry->item->name, ppt_document_stream)) {
            ppt->did_document = i;
        }
        if (ppt->did_currentuser && ppt->did_document) {
            break;
        }
    }
    if (ppt->did_currentuser == 0 || ppt->did_document == 0) {
        return BQ::ERR_FILE_FORMAT;
    }

    int nret = parse_stream_currentuser(olefile, ppt->did_currentuser, &ppt->currentuser, !ppt->big_endian);
    if (nret != BQ::OK) {
        return nret;
    }

    nret = parse_stream_document(olefile, ppt->did_document, ppt);
    if (nret != BQ::OK) {
        return nret;
    }

    return nret;
}

int parse_stream_powerpoint_document_b(const cfb_ctx_t* ctx, const msppt_t* ppt, const uint8_t* data, uint32_t size, ifilehandler* cb_file) {
    if (data == nullptr || size == 0 || nullptr == ppt) {
        return BQ::INVALID_ARG;
    }

    int nret = BQ::OK;
    bool little_endian = !ppt->big_endian;

    recordheader_t rcd_header;
    for (uint32_t pos = 0; pos < size; pos += rcd_header.recLen) {
        nret = read_recordheader(data + pos, sizeof(recordheader_t), rcd_header, little_endian);
        if (nret != BQ::OK) {
            break;
        }
        pos += sizeof(recordheader_t);

        if (size < pos + rcd_header.recLen) {
            break;
        }

        switch (rcd_header.recType) {
        case RT_Document: {
            /* 
            * ignore, specifies information about the document. 
            */
            
            auto sub_atom_recType = 0;
            if (sub_atom_recType == RT_ExternalOleObjectAtom) {
                /* 
                * TODO: RT_ExternalOleObjectAtom
                * 
                * specifies information about OLE objects
                * Referenced by : ExControlContainer, ExOleEmbedContainer, ExOleLinkContainer 
                */
            }

            break;
        }
        case RT_MainMaster: {
            /*
            * ignore, specifies a main master slide.
            * main master slide : defines the formatting and content that can be used by presentation slides.
            */ 
            break;
        }
        case RT_Slide: {
            /*
            * ignore, specifies a presentation slide or title master slide.
            * presentation slide : contains the content that can be displayed during a slide show.
            * title master slide : defines the formatting and content that can be used by presentation slides that have a title slide layout.
            */
            break;
        }
        case RT_ExternalOleObjectStg: {
            uint32_t ole_obj_size = 0;
            const uint8_t* ole_obj_data = nullptr;

            uint8_t* decompressed_data = nullptr;
            ON_SCOPE_EXIT([&]() { if (decompressed_data) free(decompressed_data); });

            if (rcd_header.recInstance() == 0x0) {
                // uncompressed
                ole_obj_size = rcd_header.recLen;
                ole_obj_data = data + pos;
            }
            else if (rcd_header.recInstance() == 0x1) {
                // compressed
                auto decompressed_size = XsUtils::byteorder2host<uint32_t>(*(uint32_t*)(data + pos), little_endian);
                auto compressed_size = rcd_header.recLen - sizeof(decompressed_size);
                auto compressed_data = data + pos + sizeof(decompressed_size);

                decompressed_data = (uint8_t*)malloc(decompressed_size);
                if (decompressed_data == nullptr) {
                    nret = BQ::ERR_MALLOC;
                    break;
                }
                memset(decompressed_data, 0, decompressed_size);

                nret = uncompress(decompressed_data, (uLongf*)&decompressed_size, compressed_data, compressed_size);
                if (nret != Z_OK) {
                    nret = BQ::ERR_UNCOMPRESS;
                    break;
                }

                ole_obj_size = decompressed_size;
                ole_obj_data = decompressed_data;
            }

            /*
            * ExOleObjStg, 
            *   specifies a compressed structured storage (described in[MSDN-COM]) for the OLE object
            * ExControlStg, 
            *   specifies a structured storage (described in[MSDN-COM]) for the ActiveX control.
            * VbaProjectStg, 
            *   specifies a structured storage (described in[MSDN-COM]) for the VBA project([MS-OVBA] section 2.2.1).
            */
            auto filetype = FiletypeDetector().detect_mem_fast(ole_obj_data, ole_obj_size);
            if (ft_ole != filetype) {
                MESSAGE_DEBUG(ctx, "[parse_stream_powerpoint_document_b] got unexpected filetype(%lu) \r\n", filetype);
                break;
            }

            auto embedded = create_embedded(ctx, ppt, ole_obj_data, ole_obj_size, filetype, 0);
            if (embedded) {
                if (0 > cb_file->handle(ctx, (const file_obj_t*)embedded)) {
                    nret = BQ::ERR_USER_INTERRUPT;
                }
                delete embedded, embedded = nullptr;
            }
            else {
                nret = BQ::ERR_FILE_CREATOR;
            }

            break;
        }
        case RT_Handout: {
            /* ignore, specifies the handout master slide. */ 
            break;
        }
        case RT_Notes: {
            /* ignore, specifies a notes slide or a notes master slide */ 
            break;
        }
        case RT_PersistDirectoryAtom: {
            /* not here */
            break;
        }
        case RT_UserEditAtom: {
            /* not here */
            break;
        }
        default: {
            MESSAGE_DEBUG(ctx, "[parse_stream_powerpoint_document_b] unexpected record: %lu", rcd_header.recType);
        }
        }
    }

    return nret;
}

int msppt::extract(const cfb_ctx_t* ctx, const msppt_t* ppt, ifilehandler* cb_file, const char16_t* passwd) {
    if (ppt == nullptr || ppt->file == nullptr || cb_file == nullptr) {
        return BQ::INVALID_ARG;
    }

    int nret = BQ::OK;
    bool little_endian = !ppt->big_endian;
    auto ole = (const olefile_t*)ppt;

    /* PowerPoint Document */
    do {
        if (ctx->extract_ppt_oleobject == false) {
            break;
        }

        uint8_t* data = nullptr;
        uint32_t size = 0;
        ON_SCOPE_EXIT([&] { if (data) free(data); });

        nret = ole::read_stream(ole, ppt->did_document, -1, &data, &size);
        if (nret != BQ::OK) {
            return nret;
        }

        if (ppt->encrypted()) {
            nret = msppt::decrypt(ctx, ppt, passwd, (ole->entries + ppt->did_document)->name, data, size);
            if (nret != BQ::OK) {
                return nret;
            }
        }

        nret = parse_stream_powerpoint_document_b(ctx, ppt, data, size, cb_file);
    } while (false);

    /* EncryptedSummary */
    do {
        // TODO:
    } while (false);
    
    /* Pictures */
    do {
        // TODO:
    } while (false);

    return nret;
}

int msppt::decrypt(const cfb_ctx_t* ctx, const msppt_t* ppt, const char16_t* passwd, const char16_t* name, uint8_t* data, uint32_t size) {
    if (ctx == 0 || ppt == 0 || ppt->file == 0 || 0 == data || 0 == size || !ppt->encrypted()) {
        return BQ::INVALID_ARG;
    }
    bool little_endian = !ppt->big_endian;
    passwd = passwd ? passwd : ppt_passwd_default;

    bin_encryption_t encryption;
    encryption.type = encryinfo_bin_rc4_capi;
    encryption.pass.rc4_capi = ppt->crypt.rc4_capi;
    int nret = crypto_binary().passwd_verify(&encryption, passwd);
    if (nret != BQ::OK) {
        return nret;
    }

    /*
    * Part 1 : PowerPoint Document Stream
    *   a. process Encrypted Flags (flags in CurrentUser stream and PowerPoint Document Stream)
    *   b. decrypt datas
    * Part 2 : Encrypted Summary Info Stream    : TODO
    * Part 3 : Pictures Stream                  : TODO
    */
    if (0 == ucs::icmp(name, u"PowerPoint Document")) {
        nret = decrypt_powerpoint_document(ctx, ppt, &encryption, passwd, data, size);
    }

    return nret;
}
