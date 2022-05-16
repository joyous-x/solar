#pragma once
#include <stdint.h>
#include <assert.h>

const uint32_t rle_chunk_size = 4096;

/*
* DecompressedBuffer
*    Chunk
*        DecompressedChunk (must be 4096)
*            ...
*        DecompressedChunk (must be less than or equal to 4096)
* 
* CompressedContainer
*    SignatureByte (1 byte)
*    Chunks (variable) 
*        CompressedChunk
*           ...
*        CompressedChunk (must be less than or equal to 4098.)
*            CompressedChunkHeader (2 bytes)
*                CompressedChunkSize (12 bits) + CompressedChunkSignature (3 bits, must be 0b011) + CompressedChunkFlag (1 bit: 0 uncompressed, 1 compressed)
*                    CompressedChunkSize + 3 = size of CompressedChunk
*            CompressedChunkData (variable, must be greater than 0 and less than or equal to 4096)
*                TokenSequence
*                    ...
*                TokenSequence
*                    FlagByte (1 byte, 0b0 LiteralToken, 0b1 CopyToken)
*                    Tokens (variable, an array of Tokens):
*                        LiteralToken or CopyToken (2 bytes, uint16_t little-endian)
*                        ...
*                        LiteralToken or CopyToken
*/

/**
* @describe  run length encoding
* @note      
*/
class RleCoding {
public:
    /**
    * @desc                            decompress
    * @param packed
    * @param size
    * @param[out] unpacked            caller should free this buffer: unpacked && free(unpacked);
    * @param[out] unpacked_size
    * @return int
    */
    int decompress(const uint8_t* packed, uint32_t size, uint8_t** unpacked, uint32_t* unpacked_size) {
        if (packed == nullptr || size < 4 || unpacked == nullptr || unpacked_size == nullptr) {
            return BQ::INVALID_ARG;
        }

        /*
        * SignatureByte
        * ！！！注意，正常来说这个标记必须为1，实际上不为1时，office 还是会提示有宏，所以应该要忽略这个标记
        */
        if (*packed == 0x01) {
            packed++;
        }
        else {
            packed++;
        }
        uint32_t nret = BQ::OK;
        uint32_t chunk_count = 0;

        for (uint32_t pos = 0, chunk_size = 0; pos + 2 < size - 1; pos += chunk_size) {
            uint16_t chunk_header = XsUtils::byteorder2host<uint16_t>(*(uint16_t*)(packed + pos), is_little_endian);
            if (((chunk_header >> 12) & 0x7) != 0x3) {
                nret = BQ::ERR_VBA_COMPRESSED_CHUNK_SIGN;
                break;
            }
            chunk_size = (chunk_header & 0x0FFF) + 3;
            chunk_count += 1;
        }

        /* try to extract all scripts */
        if (chunk_count <= 0) {
            return nret;
        }

        assert(chunk_count < 1024);
        uint8_t* buffer = (uint8_t*)malloc(chunk_count * rle_chunk_size);
        if (buffer == nullptr) {
            return BQ::ERR_MALLOC;
        }
        uint32_t buffer_pos = 0;

        for (uint32_t i = 0, pos = 0, chunk_size = 0, chunk_flag = 0; (i < chunk_count) && (pos + 2 < size - 1); i++, pos += chunk_size) {
            uint16_t chunk_header = XsUtils::byteorder2host<uint16_t>(*(uint16_t*)(packed + pos), is_little_endian);
            if (((chunk_header >> 12) & 0x7) != 0x3) {
                break;
            }
            chunk_size = (chunk_header & 0x0FFF) + 3;
            chunk_flag = (chunk_header & 0x8000) >> 15;

            //> Decompressing a CompressedChunk
            chunk_size = std::min<uint32_t>(chunk_size, size - 1 - pos);
            *unpacked_size = chunk_count * rle_chunk_size - buffer_pos;
            nret = decompress_chunkdata(packed + pos + sizeof(chunk_header), chunk_size - sizeof(chunk_header), chunk_flag, buffer + buffer_pos, unpacked_size);
            if (nret != BQ::OK) {
                break;
            }
            buffer_pos += *unpacked_size;
        }

        if (nret != BQ::OK) {
            free(buffer);
            *unpacked_size = 0;
        }
        else {
            *unpacked = buffer;
            *unpacked_size = buffer_pos;
        }

        return nret;
    }
    
    /**
    * @desc reset the compressed data and keep the size unchanged
    * @param packed
    * @param size
    * @param c          replace the every byte of the content in compressed bytes with character 'c'
    * @return int
    */
    int reset_compressed_inplace(uint8_t* packed, uint32_t size, char c = 0) {
        if (packed == nullptr || size < 4) {
            return BQ::INVALID_ARG;
        }

        uint32_t nret = BQ::OK;
        for (uint32_t cur = 1, chunk_size = 0; cur + 2 < size; cur += chunk_size) {
            uint16_t chunk_header = XsUtils::byteorder2host(*(uint16_t*)(packed + cur), is_little_endian);
            if (((chunk_header >> 12) & 0x7) != 0x3) {
                nret = BQ::ERR_VBA_COMPRESSED_CHUNK_SIGN;
                break;
            }

            chunk_size = (chunk_header & 0x0FFF) + 3;
            if (((chunk_header & 0x8000) >> 15) == 0) {
                memset(packed + cur + sizeof(chunk_header), c, chunk_size - sizeof(chunk_header));
                continue;
            }
            auto chunk_data = packed + cur;

            for (uint32_t chunk_pos = sizeof(chunk_header); chunk_pos < chunk_size; ) {
                uint8_t token_flag = *(chunk_data + chunk_pos++);
                for (uint32_t mask = 1; mask < 0x100; mask <<= 1) {
                    if ((mask & token_flag) == 0 && chunk_pos < chunk_size) {
                        *(chunk_data + chunk_pos++) = c;
                        continue;
                    }

                    if (chunk_pos + 2 > chunk_size) {
                        break;
                    }
                    chunk_pos += 2;
                }
            }
        }

        return nret;
    }

    /**
    * @desc compress
    * @param raw
    * @param size
    * @param[out] packed            caller should free this buffer: packed && free(packed);
    * @param[out] packed_size
    * @return int
    */
    int compress(const uint8_t* raw, uint32_t size, uint8_t** packed, uint32_t* packed_size) {
        if (raw == nullptr || size == 0) {
            return BQ::INVALID_ARG;
        }

        uint32_t compressed_size = 0, compressed_size_max = 1 + (size + rle_chunk_size - 1) / rle_chunk_size * (2 + rle_chunk_size);
        uint8_t* compressed = (uint8_t*)malloc(compressed_size_max);
        if (compressed == nullptr) {
            return BQ::ERR_MALLOC;
        }
        memset(compressed, 0, compressed_size_max);

        *(compressed + compressed_size++) = 0x1;
        for (uint32_t input_pos = 0; input_pos < size && compressed_size < compressed_size_max; ) {
            uint32_t cur_compressed_size = compressed_size_max - compressed_size;
            compress_chunkdata(raw + input_pos, size - input_pos, compressed + compressed_size, cur_compressed_size);
            input_pos += rle_chunk_size;
            compressed_size += cur_compressed_size;
        }

        if (packed && packed_size) {
            *packed = compressed;
            *packed_size = compressed_size;
        }
        else {
            free(compressed), compressed = nullptr;
        }
        return BQ::OK;
    }

protected:
    int matching(const uint8_t* decompressed_chunk_start, uint32_t decompressed_cur, uint32_t decompressed_end, uint32_t* p_length) {
        int32_t candidate = decompressed_cur - 1;
        int32_t best_length = 0, best_candidate = 0;

        while (candidate >= 0) {
            auto C = candidate;
            auto D = decompressed_cur;
            auto len = 0;
            while ((D < decompressed_end) && (*(decompressed_chunk_start + C) == *(decompressed_chunk_start + D))) {
                C++, D++, len++;
            }
            if (len > best_length) {
                best_length = len;
                best_candidate = candidate;
            }
            candidate--;
        }

        uint32_t length = 0, offset = 0;
        if (best_length >= 3) {
            int32_t maximum_length = copytoken_help(decompressed_cur, 0, 0, 0, 0);
            length = std::min<int32_t>(best_length, maximum_length);
            offset = decompressed_cur - best_candidate;
        }
        if (p_length) { *p_length = length; }
        return offset;
    }

    uint16_t copytoken_help(uint32_t decompressed_cur, uint32_t decompressed_chunk_start, uint16_t* p_length_mask, uint16_t* p_offset_mask, uint16_t* p_bit_count) {
        auto difference = decompressed_cur - decompressed_chunk_start;

        uint16_t bit_count = 0;
        for (; uint32_t(1 << bit_count) < difference; bit_count++);
        bit_count = (bit_count < 4) ? 4 : bit_count;

        uint16_t length_mask = uint16_t(0xFFFF) >> bit_count;
        uint16_t offset_mask = 0 ^ (length_mask);
        uint16_t maximum_len = (uint16_t(0xFFFF) >> bit_count) + 3;
        if (p_length_mask) { *p_length_mask = length_mask; }
        if (p_offset_mask) { *p_offset_mask = offset_mask; }
        if (p_bit_count) { *p_bit_count = bit_count; }
        return maximum_len;
    }

    int pack_copy_token(uint32_t decompressed_cur, uint32_t decompressed_chunk_start, uint32_t offset, uint32_t length) {
        uint16_t bit_count = 0;
        uint16_t maximum_len = copytoken_help(decompressed_cur, decompressed_chunk_start, 0, 0, &bit_count);
        uint16_t copy_token = ((offset - 1) << (16 - bit_count)) | (length - 3);
        return copy_token;
    }

    int compress_chunkdata(const uint8_t* chunk_data, uint32_t chunk_size, uint8_t* compressed_data, uint32_t& compressed_size) {
        const uint32_t compressed_chunk_start = 0, decompressed_chunk_start = 0;
        uint16_t compressed_header = 1;
        uint32_t compressed_cur = sizeof(compressed_header) + compressed_chunk_start;
        uint32_t compressed_end = compressed_size;
        assert(compressed_size >= rle_chunk_size + sizeof(uint16_t));
        uint32_t decompressed_cur = decompressed_chunk_start;
        uint32_t decompressed_end = std::min<uint32_t>(chunk_size, rle_chunk_size);

        for (; decompressed_cur < decompressed_end && compressed_cur < compressed_end; ) {
            uint32_t flagbyte_index = compressed_cur;
            uint8_t token_flags = 0;
            
            compressed_cur++;
            for (uint32_t i = 0, offset = 0, length = 0; i < 8; i++) {
                if (!(decompressed_cur < decompressed_end && compressed_cur < compressed_end)) {
                    continue;
                }
                offset = matching(chunk_data, decompressed_cur, decompressed_end, &length);
                if (offset != 0) {
                    if (compressed_cur + 1 < compressed_end) {
                        uint16_t copy_token = pack_copy_token(decompressed_cur, decompressed_chunk_start, offset, length);
                        uint16_t token_littlend = XsUtils::host2byteorder(copy_token, true);
                        *(compressed_data + compressed_cur) = (token_littlend & 0x00FF);
                        *(compressed_data + compressed_cur + 1) = ((token_littlend & 0xFF00) >> 0x8);
                        token_flags |= (1 << i);
                        compressed_cur += 2;
                        decompressed_cur += length;
                    }
                    else {
                        compressed_cur = compressed_end;
                    }
                }
                else {
                    if (compressed_cur < compressed_end) {
                        *(compressed_data + compressed_cur) = *(chunk_data + decompressed_cur);
                        compressed_cur++, decompressed_cur++;
                    }
                    else {
                        compressed_cur = compressed_end;
                    }
                }
            }

            compressed_data[flagbyte_index] = token_flags;
        }

        if (decompressed_cur < decompressed_end) {
            compressed_cur = compressed_chunk_start + 2;
            decompressed_cur = decompressed_chunk_start;

            auto pad_count = rle_chunk_size;
            for (uint32_t last_byte = decompressed_chunk_start + pad_count; decompressed_cur < last_byte; pad_count--) {
                *(compressed_data + compressed_cur) = *(chunk_data + decompressed_cur);
                compressed_cur++, decompressed_cur++;
            }

            for (uint32_t i = 0; i < pad_count; i++) {
                *(compressed_data + compressed_cur++) = 0x0;
            }

            compressed_header = 0;
        }
        else {
            compressed_header = 1;
        }

        compressed_size = compressed_cur - compressed_chunk_start;
        compressed_header = (compressed_header ? 0xB000 : 0x3000) | ((compressed_size - 3) & 0x0FFF);
        uint16_t header_littlend = XsUtils::host2byteorder(compressed_header, true);
        *(compressed_data + compressed_chunk_start) = (header_littlend & 0x00FF);
        *(compressed_data + compressed_chunk_start + 1) = ((header_littlend & 0xFF00) >> 0x8);

        return BQ::OK;
    }

    int decompress_chunkdata(const uint8_t* chunk, uint32_t chunk_size, uint32_t chunk_flag, uint8_t* unpacked, uint32_t* unpacked_size) {
        if (chunk_size > rle_chunk_size || (chunk_flag != 0 && chunk_flag != 1) || nullptr == unpacked_size) {
            return BQ::INVALID_ARG;
        }

        uint32_t unpacked_max_size = *unpacked_size;
        if (chunk_flag == 0) {
            //> chunk_size bytes of uncompressed data
            memcpy(unpacked, chunk, chunk_size);
            *unpacked_size = chunk_size;
            return 0;
        }

        for (uint32_t pos = 0, winpos = 0; pos < chunk_size; ) {
            uint32_t flag = (uint32_t) * (chunk + pos++);

            for (uint32_t mask = 1, shift = 0, length = 0, offset = 0; mask < 0x100; mask <<= 1) {
                assert(winpos < unpacked_max_size);
                if ((mask & flag) == 0 && pos < chunk_size) {
                    *(unpacked + winpos) = *(chunk + pos);
                    pos++, winpos++;
                    continue;
                }

                if (pos + 2 > chunk_size) {
                    pos += 2;
                    break;
                }

                uint16_t token = XsUtils::byteorder2host<uint16_t>(*(uint16_t*)(chunk + pos), is_little_endian);
                pos += 2;

                shift = 12 - (winpos > 0x10) - (winpos > 0x20) - (winpos > 0x40) - (winpos > 0x80) - (winpos > 0x100) - (winpos > 0x200) - (winpos > 0x400) - (winpos > 0x800);
                length = (uint16_t)((token & ((1 << shift) - 1)) + 3);
                offset = token >> shift;
                
                uint32_t winsrc = winpos - offset - 1;
                if (winpos > offset + 1 && winpos + length < rle_chunk_size && winsrc + length < winpos) {
                    memcpy(unpacked + winpos, unpacked + winsrc, length);
                    winpos += length;
                }
                else {
                    while (int32_t(length--) > 0) {
                        winsrc = (winpos - offset - 1) % rle_chunk_size;
                        *(unpacked + (winpos++ % rle_chunk_size)) = *(unpacked + winsrc);
                    }
                }                
            }

            if (unpacked_size) {
                *unpacked_size = winpos;
            }
        }

        return BQ::OK;
    }

private:
    const bool is_little_endian{ true };
};