#include <cassert>
#include "offcrypto.h"

bool offcrypto::is_ole_encrypted(const ctx_t* ctx, const olefile_t* ole) {
    crypto_dataspace_t data;
    return (BQ::OK == crypto_ds().is_ole_encrypted(ole, &data) && data.encrypted());
}

int offcrypto::parse_dataspace_encryption(const ctx_t* ctx, const olefile_t* ole, crypto_dataspace_t* data) {
    return crypto_ds().parse_dataspace_encryption(ole, data);
}

int offcrypto::dataspace_decrypt_stream(const ctx_t* ctx, crypto_dataspace_t* ds, const char16_t* passwd, uint8_t* data, uint32_t size, ifile* out) {
    if (ds == nullptr || data == nullptr || size == 0 || nullptr == out) {
        return BQ::INVALID_ARG;
    }

    int nret = crypto_ecma376().passwd_verify(&ds->encrypt_info, passwd);
    if (nret != BQ::OK) {
        return nret;
    }

    return crypto_ecma376().decrypt(&ds->encrypt_info, passwd, data, size, out);
}

int offcrypto::binary_passwd_verify(const ctx_t* ctx, const bin_encryption_t* encrypt, const char16_t* passwd) {
    return crypto_binary().passwd_verify(encrypt, passwd);
}

int offcrypto::binary_decrypt_inplace(const ctx_t* ctx, bin_encryption_t* encrypt, const char16_t* passwd, uint8_t* data, uint32_t size) {
    return crypto_binary().decrypt_inplace(encrypt, passwd, data, size);
}
