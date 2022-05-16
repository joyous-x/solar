/////////////////////////////////////////////////////////////////
//
//    reference: [MS-OFFCRYPTO] - v20210817
//
/////////////////////////////////////////////////////////////////

#pragma once 
#include "ole.h"
#include "offcrypto_assist.h"

namespace offcrypto {
    /**
     * @brief check the office is encrypted or not
     *
     * @param ole
     * @return 
     */
    bool is_ole_encrypted(const ctx_t* ctx, const olefile_t* ole);

    /**
     * @brief parse encryption stream
     *
     * @param ctx
     * @param ole
     * @param data      out, output information about encryption
     * @return 
     */
    int parse_dataspace_encryption(const ctx_t* ctx, const olefile_t* ole, crypto_dataspace_t* data);

    /**
     * @brief decrypt the buffer of a stream bytes
     *
     * @param ctx
     * @param ds        output of parse_dataspace_encryption
     * @param passwd
     * @param data      the buffer need to be decrypted
     * @param size      the length of the buffer
     * @param out       write the decrypted data to ifile
     * @return
     */
    int dataspace_decrypt_stream(const ctx_t* ctx, crypto_dataspace_t* ds, const char16_t* passwd, uint8_t* data, uint32_t size, ifile* out);

    /**
     * @brief verify the passwd
     */
    int binary_passwd_verify(const ctx_t* ctx, const bin_encryption_t* encrypt, const char16_t* passwd);

    /**
     * @brief decrypt the xlm data in place
     */
    int binary_decrypt_inplace(const ctx_t* ctx, bin_encryption_t* encrypt, const char16_t* passwd, uint8_t* data, uint32_t size);

    /**
     * @brief the office file's byteorder
     */
    const bool is_little_endian{ true };
};