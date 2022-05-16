#pragma
#include "./helper.h"
#include "../3rds/base64.h"
#include "zlib/zlib.h"

#pragma pack(push, 1)

typedef struct __st_active_mine {
    uint8_t header[12];
    uint16_t unknown_a; /* memory should be : 01 f0 */
    // +14
    struct {
        uint32_t size;
        uint8_t  data[1]; /* variable, should be 0xFFFFFFFF */
    } unknown_b;
    uint32_t unknown_c;   /* memory should be : {x}0 00 0{y} f0 */

    uint32_t compressed_size;
    uint32_t field_size_d;
    uint32_t field_size_e;
    uint8_t unknown_d[1]; /* variable, should be 00000000 or 00000000 00000001 */
    uint8_t unknown_e[1]; /* variable, the first element should be vba_tail_type */
    uint32_t size;        /* uncompressed size */
    uint8_t  data[1];     /* variable, compressed, should be decompressed with zlib.decompress */
} active_mine_t;

#pragma pack(pop)

class activemine {
public:
    int extract(const ctx_t* ctx, const file_obj_t* mso, ifilehandler* cb_file) {
        if (ctx == nullptr || mso == nullptr || mso->file == nullptr || cb_file == nullptr) {
            return BQ::INVALID_ARG;
        }

        const uint8_t* data = mso->file->mapping();
        uint32_t size = (uint32_t)mso->file->size();

        uint8_t* extracted = nullptr;
        uint32_t extracted_size = 0;
        ON_SCOPE_EXIT([&]() { if (extracted) free(extracted); });

        int nret = extract_bin(ctx, data, size, &extracted, &extracted_size);
        if (nret != BQ::OK) {
            return nret;
        }

        auto embedded = create_embedded(ctx, mso, extracted, extracted_size, ft_unknown, 0);
        if (embedded) {
            if (0 > cb_file->handle(ctx, (const file_obj_t*)embedded)) {
                nret = BQ::ERR_USER_INTERRUPT;
            }
            delete embedded, embedded = nullptr;
        }
        else {
            nret = BQ::ERR_FILE_CREATOR;
        }

        return nret;
    }

    int extract_b64(const ctx_t* ctx, const uint8_t* data, uint32_t size, uint8_t** out, uint32_t* out_size) {
        if (data == 0 || size == 0 || out == 0 || out_size == 0) {
            return BQ::INVALID_ARG;
        }

        size_t raw_size = 0;
        auto raw = base64_decode((const unsigned char*)data, size, &raw_size);
        if (raw == nullptr) {
            return BQ::ERR_UNCOMPRESS;
        }
        ON_SCOPE_EXIT([&]() { if (raw) free(raw); });

        return extract_bin(ctx, raw, raw_size, out, out_size);
    }

    /*
    * @brief extract MSO/ActiveMime files
    * @reference https://github.com/idiom/activemime-format/blob/master/amime.py
    */
    int extract_bin(const ctx_t* ctx, const uint8_t* data, uint32_t size, uint8_t** out, uint32_t* out_size) {
        if (data == 0 || size == 0 || out == 0 || out_size == 0) {
            return BQ::INVALID_ARG;
        }

        auto filetype = FiletypeDetector().detect_mem_fast(data, size);
        if (ft_ms_activemine != filetype) {
            return BQ::ERR_FORMAT;
        }

        active_mine_t amine;
        if (size <= sizeof(amine)) {
            return BQ::ERR_FORMAT;
        }

        uint32_t offset = 0;
        memcpy((void*)&amine.header, data, sizeof(amine.header));
        offset += sizeof(amine.header);
        amine.unknown_a = XsUtils::byteorder2host<uint16_t>(*(uint16_t*)(data + offset), is_little_endian);
        offset += sizeof(amine.unknown_a);
        amine.unknown_b.size = XsUtils::byteorder2host<uint16_t>(*(uint16_t*)(data + offset), is_little_endian);
        offset += sizeof(amine.unknown_b.size) + amine.unknown_b.size;

        amine.unknown_c = XsUtils::byteorder2host<uint32_t>(*(uint32_t*)(data + offset), is_little_endian);
        amine.compressed_size = XsUtils::byteorder2host<uint32_t>(*(uint32_t*)(data + offset + sizeof(uint32_t) * 1), is_little_endian);
        amine.field_size_d = XsUtils::byteorder2host<uint32_t>(*(uint32_t*)(data + offset + sizeof(uint32_t) * 2), is_little_endian);
        amine.field_size_e = XsUtils::byteorder2host<uint32_t>(*(uint32_t*)(data + offset + sizeof(uint32_t) * 3), is_little_endian);
        offset += sizeof(uint32_t) * 4;

        offset += amine.field_size_d;
        if (amine.field_size_e >= 4) {
            uint32_t vba_tail_type = XsUtils::byteorder2host<uint32_t>(*(uint32_t*)(data + offset), is_little_endian);
            bool has_vba_tail = vba_tail_type == 0;
        }
        offset += amine.field_size_e;

        amine.size = XsUtils::byteorder2host<uint32_t>(*(uint32_t*)(data + offset), is_little_endian);
        offset += sizeof(amine.size);

        uint8_t* decompress = (uint8_t*)malloc(amine.size);
        if (decompress == nullptr) {
            return BQ::ERR_MALLOC;
        }
        ON_SCOPE_EXIT([&]() { if (decompress) free(decompress); });
        memset(decompress, 0, amine.size);

        unsigned long decompress_size = amine.size;
        int nret = uncompress(decompress, &decompress_size, data + offset, std::min<uint32_t>(amine.compressed_size, size - offset));
        if (nret != Z_OK) {
            return BQ::ERR_UNCOMPRESS;
        }

        *out = decompress;
        *out_size = decompress_size;
        decompress = nullptr;

        return BQ::OK;
    }

protected:
    static const bool is_little_endian{ true };
};
