
#include "ole_utils.h"
#include "../utils/bitmap.h"

const uint8_t OLE_MAGIC[] = { 0xD0, 0xCF, 0x11, 0xE0, 0xA1, 0xB1, 0x1A, 0xE1 };

bool OleUtils::check_ole_header(const ole2_header_t* header) {
    if (header == nullptr) {
        return false;
    }

    if (memcmp(OLE_MAGIC, header->magic, sizeof(OLE_MAGIC)) != 0) {
        return false;
    }

    if (!(header->byte_order == 0xFEFF || header->byte_order == 0xFFFE)) {
        return false;
    }

    if (header->sector_shift < 0x06 || header->sector_shift > 0x1D
        || header->short_sector_shift < 0x03
        || header->short_sector_shift > header->sector_shift) {
        return false;
    }

    if (header->major_version != 3 && header->major_version != 4) {
        return false;
    }

    if (header->major_version == 3 && header->directory_sector_count != 0) {
        return false;
    }

    return (sizeof(*header) == OLE_HEADER_SIZE);
}

int OleUtils::make_sid_chain(const uint32_t* sat_table, uint32_t sat_count, uint32_t start_sid, std::vector<uint32_t>& chain, bool* little_endian) {
    if (sat_table == nullptr || sat_count == 0 || start_sid >= sat_count) {
        return BQ::INVALID_ARG;
    }

    Bitmap walked;
    for (; false == is_special_ole_sid(start_sid) && start_sid <= sat_count && false == walked.test(start_sid); ) {
        walked.set(start_sid);
        chain.push_back(start_sid);
        start_sid = *(sat_table + start_sid);
        if (little_endian != nullptr) {
            start_sid = XsUtils::byteorder2host<uint32_t>(start_sid, *little_endian);
        }
    }

    return BQ::OK;
}


///////////////////////////////////////////////////////////////////
/// <summary>
/// 
/// </summary>
///////////////////////////////////////////////////////////////////

int OleUtils::correct_byte_order(ole2_header_t* header, bool is_input_little_endian) {
    if (is_input_little_endian == XsUtils::is_host_little_endian()) {
        return BQ::OK;
    }
    (void)header->magic;
    (void)header->clsid;
    header->minor_version = XsUtils::byteorder2host<uint16_t>(header->minor_version, is_input_little_endian);
    header->major_version = XsUtils::byteorder2host<uint16_t>(header->major_version, is_input_little_endian);
    (void)header->byte_order;
    header->sector_shift = XsUtils::byteorder2host<uint16_t>(header->sector_shift, is_input_little_endian);
    header->short_sector_shift = XsUtils::byteorder2host<uint16_t>(header->short_sector_shift, is_input_little_endian);
    (void)header->reserved;
    header->directory_sector_count = XsUtils::byteorder2host<int32_t>(header->directory_sector_count, is_input_little_endian);
    header->sat_sector_count = XsUtils::byteorder2host<int32_t>(header->sat_sector_count, is_input_little_endian);
    header->directory_entry_sid = XsUtils::byteorder2host<int32_t>(header->directory_entry_sid, is_input_little_endian);
    header->transaction_signature = XsUtils::byteorder2host<uint32_t>(header->transaction_signature, is_input_little_endian);
    header->min_stream_cutoff = XsUtils::byteorder2host<uint32_t>(header->min_stream_cutoff, is_input_little_endian);
    header->ssat_entry_sid = XsUtils::byteorder2host<int32_t>(header->ssat_entry_sid, is_input_little_endian);
    header->ssat_sector_count = XsUtils::byteorder2host<int32_t>(header->ssat_sector_count, is_input_little_endian);
    header->msat_entry_sid = XsUtils::byteorder2host<int32_t>(header->msat_entry_sid, is_input_little_endian);
    header->msat_sector_count = XsUtils::byteorder2host<int32_t>(header->msat_sector_count, is_input_little_endian);
    for (int i = 0; i < OLE_MAX_LOCAL_MSAT_SIZE; i++) {
        header->msat_array[i] = XsUtils::byteorder2host<int32_t>(header->msat_array[i], is_input_little_endian);
    }
    return BQ::OK;
}

int OleUtils::correct_byte_order(ole2_directory_entry_t* entry, bool is_input_little_endian) {
    if (is_input_little_endian == XsUtils::is_host_little_endian()) {
        return BQ::OK;
    }
    for (int32_t i = 0; i < OLE_DIRECTOR_ENTRY_NAME_SIZE; i++) {
        entry->name[i] = XsUtils::byteorder2host<char16_t>(entry->name[i], is_input_little_endian);
    }
    entry->name_bytes = XsUtils::byteorder2host<uint16_t>(entry->name_bytes, is_input_little_endian);
    (void)entry->obj_type;
    (void)entry->color;
    entry->l_sib_did = XsUtils::byteorder2host<uint32_t>(entry->l_sib_did, is_input_little_endian);
    entry->r_sib_did = XsUtils::byteorder2host<uint32_t>(entry->r_sib_did, is_input_little_endian);
    entry->child_did = XsUtils::byteorder2host<uint32_t>(entry->child_did, is_input_little_endian);
    (void)entry->clsid;
    entry->user_flags = XsUtils::byteorder2host<uint32_t>(entry->user_flags, is_input_little_endian);
    entry->create_time[0] = XsUtils::byteorder2host<uint32_t>(entry->create_time[0], is_input_little_endian);
    entry->create_time[1] = XsUtils::byteorder2host<uint32_t>(entry->create_time[1], is_input_little_endian);
    entry->modify_time[0] = XsUtils::byteorder2host<uint32_t>(entry->modify_time[0], is_input_little_endian);
    entry->modify_time[1] = XsUtils::byteorder2host<uint32_t>(entry->modify_time[1], is_input_little_endian);
    entry->entry_sid = XsUtils::byteorder2host<uint32_t>(entry->entry_sid, is_input_little_endian);
    entry->stream_bytes = XsUtils::byteorder2host<uint64_t>(entry->stream_bytes, is_input_little_endian);
    return BQ::OK;
}

int OleUtils::correct_byte_order(int32_t* entry, int32_t size, bool is_input_little_endian) {
    if (is_input_little_endian == XsUtils::is_host_little_endian()) {
        return BQ::OK;
    }
    for (int32_t i = 0; i < size; i++) {
        entry[i] = XsUtils::byteorder2host<int32_t>(entry[i], is_input_little_endian);
    }
    return BQ::OK;
}

///////////////////////////////////////////////////////////////////
/// <summary>
/// 
/// </summary>
///////////////////////////////////////////////////////////////////

int OleUtils::make_msat_table(ifile* file, ole2_header_t* header, uint32_t** table, uint32_t* count) {
    if (file == nullptr || header == nullptr) {
        return BQ::INVALID_ARG;
    }

    uint32_t sector_size = (uint32_t)1 << header->sector_shift;
    std::vector<uint32_t> msat;

    Bitmap walked;
    for (uint32_t i = 0, cur_sid = 0; i < OLE_MAX_LOCAL_MSAT_SIZE; i++) {
        cur_sid = header->msat_array[i];
        if (is_special_ole_sid(cur_sid)) {
            continue;
        }
        if (walked.test(cur_sid)) {
            continue;
        }
        walked.set(cur_sid);
        msat.push_back(cur_sid);
    }

    if (header->msat_sector_count != 0) {
        uint32_t msat_size_addition = header->msat_sector_count * (sector_size / sizeof(uint32_t));
        uint32_t* msat_table_addition = (uint32_t*)malloc(msat_size_addition * sizeof(uint32_t));
        if (nullptr == msat_table_addition) {
            return BQ::ERR_MALLOC;
        }
        ON_SCOPE_EXIT([&]() { free(msat_table_addition); });

        memset(msat_table_addition, 0, msat_size_addition * sizeof(uint32_t));
        for (uint32_t i = 0, cur_sid = header->msat_entry_sid, offset = 0; i < header->msat_sector_count; i++) {
            if (is_special_ole_sid(cur_sid)) {
                break;
            }
            if (walked.test(cur_sid)) {
                continue;
            }
            walked.set(cur_sid);

            int nret = file->read(((int64_t)cur_sid + 1) * sector_size, (uint8_t*)msat_table_addition + offset, sector_size, 0);
            if (nret != 0) {
                break;
            }

            offset += sector_size;
            cur_sid = *(msat_table_addition + (offset / sizeof(uint32_t)) - 1);
            cur_sid = XsUtils::byteorder2host<uint32_t>(cur_sid, OleUtils::little_endian(header));
        }

        walked.clear();
        correct_byte_order((int32_t*)msat_table_addition, msat_size_addition, OleUtils::little_endian(header));
        for (uint32_t i = 0, cur_sid = 0; i < msat_size_addition; i++) {
            cur_sid = *(msat_table_addition + i);
            if (is_special_ole_sid(cur_sid)) {
                continue;
            }
            if (walked.test(cur_sid)) {
                continue;
            }
            walked.set(cur_sid);
            msat.push_back(cur_sid);
        }
    }

    *count = msat.size();
    *table = nullptr;
    if (msat.size() > 0) {
        *table = (uint32_t*)malloc(msat.size() * sizeof(msat[0]));
        if (nullptr == *table) {
            return BQ::ERR_MALLOC;
        }
        memcpy(*table, &msat[0], msat.size() * sizeof(uint32_t));
    }
    return BQ::OK;
}

int OleUtils::make_sat_table(ifile* file, ole2_header_t* header, const uint32_t* msat_table, uint32_t msat_count, uint32_t** table, uint32_t* count) {
    if (file == nullptr || header == nullptr || msat_table == nullptr || table == nullptr || count == nullptr) {
        return BQ::INVALID_ARG;
    }

    uint32_t sector_size = (uint32_t)1 << header->sector_shift;
    uint32_t sat_size_expected = msat_count * (sector_size / sizeof(uint32_t));
    uint32_t* sat_table = (uint32_t*)malloc(sat_size_expected * sizeof(uint32_t));
    if (nullptr == sat_table) {
        return BQ::ERR_MALLOC;
    }
    memset(sat_table, 0, sat_size_expected * sizeof(uint32_t));

    Bitmap walked;
    for (uint32_t i = 0, cur_sid = 0; i < msat_count; i++) {
        cur_sid = *(msat_table + i);
        if (is_special_ole_sid(cur_sid)) {
            continue;
        }
        if (walked.test(cur_sid)) {
            continue;
        }
        walked.set(cur_sid);

        int nret = file->read(((int64_t)cur_sid + 1) * sector_size, (uint8_t*)sat_table + i * sector_size, sector_size, 0);
        if (nret != 0) {
            continue;
        }
    }

    *table = sat_table;
    *count = sat_size_expected;
    return BQ::OK;
}

extern int read_norm_stream(const olefile_t* ole, uint32_t sid, uint8_t* buff, uint32_t size, uint32_t* readed);
int OleUtils::make_ssat_table(olefile_t* ole, uint32_t** table, uint32_t* count) {
    if (ole == nullptr || ole->file == nullptr || ole->sat_table == nullptr
        || 0 == ole->sat_table_size || table == nullptr || count == nullptr) {
        return BQ::INVALID_ARG;
    }

    uint8_t* buffer = nullptr;
    uint32_t readed = 0;
    int32_t nret = read_norm_stream(ole, ole->header.ssat_entry_sid, buffer, 0, &readed);
    if (BQ::OK != nret) {
        return nret;
    }

    buffer = (uint8_t*)malloc(readed);
    if (nullptr == buffer) {
        return BQ::ERR_MALLOC;
    }
    memset(buffer, 0, readed);
    ON_SCOPE_EXIT([&]() { if (buffer && BQ::OK != nret) free(buffer); });

    nret = read_norm_stream(ole, ole->header.ssat_entry_sid, buffer, readed, &readed);
    if (BQ::OK == nret) {
        *table = (uint32_t*)buffer;
        *count = readed / sizeof(uint32_t);
    }

    return nret;
}

int OleUtils::make_directory_entries(ifile* file, ole2_header_t* header, const uint32_t* sat_table, uint32_t sat_count
    , ole2_directory_entry_t** entries, uint32_t* count) {
    if (file == nullptr || header == nullptr || sat_table == nullptr || sat_count == 0) {
        return BQ::INVALID_ARG;
    }
    ole2_directory_entry_t tmp;
    auto size2 = sizeof(tmp);
    auto size = sizeof(ole2_directory_entry_t);
    assert(sizeof(ole2_directory_entry_t) == OLE_DIRECTOR_ENTRY_SIZE);
    uint32_t sector_size = (uint32_t)1 << header->sector_shift;
    bool little_endian = OleUtils::little_endian(header);

    std::vector<uint32_t> directory_entry_chain;
    OleUtils::make_sid_chain(sat_table, sat_count, header->directory_entry_sid, directory_entry_chain, &little_endian);
    assert(header->major_version <= 4);
    if (header->directory_sector_count && directory_entry_chain.size() != header->directory_sector_count) {
        header->directory_sector_count = std::min<uint32_t>(directory_entry_chain.size(), header->directory_sector_count);
        directory_entry_chain.resize(header->directory_sector_count);
    }

    uint32_t entries_count = directory_entry_chain.size() * (sector_size / sizeof(ole2_directory_entry_t));
    ole2_directory_entry_t* entries_list = (ole2_directory_entry_t*)malloc(entries_count * sizeof(ole2_directory_entry_t));
    if (nullptr == entries_list) {
        return BQ::ERR_MALLOC;
    }
    memset((void*)entries_list, 0, entries_count * sizeof(ole2_directory_entry_t));

    for (uint32_t i = 0, cur_sid = 0; i < directory_entry_chain.size(); i++) {
        cur_sid = directory_entry_chain[i];
        if (is_special_ole_sid(cur_sid)) {
            break;
        }
        int nret = file->read(((int64_t)cur_sid + 1) * sector_size, (uint8_t*)entries_list + i * sector_size, sector_size, 0);
        if (nret != 0) {
            break;
        }
    }

    for (uint32_t i = 0; i < entries_count; i++) {
        auto tmp_entry = (ole2_directory_entry_t*)(entries_list + i);
        correct_byte_order(tmp_entry, little_endian);
        /*
        * @note:
        *    in version 3 compound files, it is recommended to ignore the most significant 32 bits of this field.
        *    because: implementers should be aware that some older implementations did not initialize the most significant 32 bits of this field,
        *             and these bits might therefore be nonzero in files that are otherwise valid version 3 compound files.
        *
        *    we won't process big file( >2GB ), so we just ignore the most significant 32 bits of this field.
        */
        tmp_entry->stream_bytes &= 0x00000000FFFFFFFF;
    }

    *entries = entries_list;
    *count = entries_count;
    return BQ::OK;
}

///////////////////////////////////////////////////////////////////
/// <summary>
/// 
/// </summary>
///////////////////////////////////////////////////////////////////

int OleUtils::memset_sector(olefile_t* ole, uint32_t sid, bool mini_sector, uint8_t val) {
    if (ole == nullptr) {
        return BQ::INVALID_ARG;
    }
    if (is_special_ole_sid(sid)) {
        return BQ::ERR_OLE_SID;
    }

    uint32_t sector_size = (uint32_t)1 << ole->header.sector_shift;
    uint32_t short_sector_size = (uint32_t)1 << ole->header.short_sector_shift;
    uint32_t short_per_sector = (uint32_t)1 << (ole->header.sector_shift - ole->header.short_sector_shift);
    bool little_endian = OleUtils::little_endian(&ole->header);

    /* update sat/ssat table */
    uint32_t table_sid = 0, table_offset = 0;
    /* update stream content */
    uint32_t stream_sid = 0, stream_offset = 0, stream_size = 0;

    if (!mini_sector && sid < ole->sat_table_size) {
        stream_sid = sid;
        stream_size = sector_size;
        stream_offset = 0;

        /* olefile_t: update memory */
        *(ole->sat_table + sid) = OLE_ENDOFCHAIN;

        /* olefile: update file */
        uint32_t msat_sec_index = sid / (sector_size / sizeof(uint32_t));
        if (msat_sec_index < ole->msat_table_size) {
            table_sid = ole->msat_table[msat_sec_index];
            table_offset = (sid % (sector_size / sizeof(uint32_t))) * sizeof(uint32_t);;
        }
    }
    else if (mini_sector && sid < ole->ssat_table_size) {
        uint32_t ssid = sid;

        std::vector<uint32_t> ssec_sec_chain;
        OleUtils::make_sid_chain(ole->sat_table, ole->sat_table_size, ole->ssector_entry_sid, ssec_sec_chain, &little_endian);

        uint32_t ssec_sec_index = ssid / short_per_sector;
        if (ssec_sec_index < ssec_sec_chain.size()) {
            stream_offset = (ssid % short_per_sector) * short_sector_size;
            stream_size = short_sector_size;
            stream_sid = ssec_sec_chain[ssec_sec_index];
        }

        /* olefile_t: update memory  */
        *(ole->ssat_table + ssid) = OLE_ENDOFCHAIN;

        /* olefile: update file  */
        std::vector<uint32_t> ssat_sec_chain;
        OleUtils::make_sid_chain(ole->sat_table, ole->sat_table_size, ole->header.ssat_entry_sid, ssat_sec_chain, &little_endian);

        uint32_t ssat_sec_index = ssid / (sector_size / sizeof(uint32_t));
        if (ssat_sec_index < ssat_sec_chain.size()) {
            table_sid = ssat_sec_chain[ssat_sec_index];
            table_offset = (ssid % (sector_size / sizeof(uint32_t))) * sizeof(uint32_t);
        }
    }

    int nret = 0;
    do {
        uint32_t size = stream_size > sizeof(uint32_t) ? stream_size : sizeof(uint32_t);
        uint8_t* buffer = (uint8_t*)malloc(size);
        if (buffer == nullptr) {
            nret = BQ::ERR_MALLOC;
            break;
        }
        ON_SCOPE_EXIT([&] {if (buffer) free(buffer); });
        memset(buffer, val, size);

        uint32_t offset = (table_sid + 1) * sector_size + table_offset;
        nret = ole->file->write(offset, (const unsigned char*)buffer, sizeof(uint32_t));
        if (nret < 0) {
            nret = BQ::ERR_WRITE;
            break;
        }

        offset = (stream_sid + 1) * sector_size + stream_offset;
        nret = ole->file->write(offset, (const unsigned char*)&buffer, stream_size);
        if (nret < 0) {
            nret = BQ::ERR_WRITE;
            break;
        }

        nret = BQ::OK;
    } while (false);

    return nret;
}

int OleUtils::set_directory_entry(olefile_t* ole, uint32_t did, const char16_t* name, uint64_t* stream_bytes, uint32_t* child_did, uint32_t* l_sib_did, uint32_t* r_sib_did) {
    if (ole == nullptr || did >= ole->entry_count) {
        return BQ::INVALID_ARG;
    }

    int nret = BQ::OK;
    uint32_t sector_size = (uint32_t)1 << ole->header.sector_shift;
    uint32_t entriy_count_per_sector = sector_size / sizeof(ole2_directory_entry_t);

    ole2_directory_entry_t* entry = ole->entries + did;
    if (stream_bytes) {
        entry->stream_bytes = *stream_bytes;
    }
    if (l_sib_did) {
        entry->l_sib_did = *l_sib_did;
    }
    if (r_sib_did) {
        entry->r_sib_did = *r_sib_did;
    }
    if (child_did) {
        entry->child_did = *child_did;
    }
    if (name) {
        entry->name_bytes = std::min((int)ucs::len<char16_t>(name) + 1, OLE_DIRECTOR_ENTRY_NAME_SIZE) * 2;
        memcpy(entry->name, name, entry->name_bytes);
    }

    bool little_endian = OleUtils::little_endian(&ole->header);
    std::vector<uint32_t> directory_entry_chain;
    OleUtils::make_sid_chain(ole->sat_table, ole->sat_table_size, ole->header.directory_entry_sid, directory_entry_chain, &little_endian);

    uint32_t dir_sid_index = did / entriy_count_per_sector;
    uint32_t dir_sid_offset = (did % entriy_count_per_sector) * sizeof(ole2_directory_entry_t);
    if (dir_sid_index < directory_entry_chain.size()) {
        uint32_t offset = (directory_entry_chain[dir_sid_index] + 1) * sector_size + dir_sid_offset;

        ole2_directory_entry_t entry_tmp;
        memcpy((void*)&entry_tmp, (const void*)entry, sizeof(ole2_directory_entry_t));
        OleUtils::correct_byte_order(&entry_tmp, little_endian);

        nret = ole->file->write(offset, (const unsigned char*)&entry_tmp, sizeof(entry_tmp));
        if (nret < 0) {
            nret = BQ::ERR_WRITE;
        }
        else {
            nret = BQ::OK;
        }
    }

    return nret;
}
