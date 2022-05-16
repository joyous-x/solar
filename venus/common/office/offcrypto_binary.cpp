#include <cassert>
#include "offcrypto_assist.h"
#include "../3rds/sha1.h"
#include "../3rds/md5.h"
#include "../3rds/rc4.h"
#include "../utils/utils.h"


class RC4Helper {
public:
    RC4Helper(uint8_t* key, int key_len) {
        setup(key, key_len);
    }

    void setup(uint8_t* key, int key_len) {
        rc4_setup(&rc4, key, key_len);
    }

    void update(uint8_t* buffer, int size) {
        rc4_crypt(&rc4, buffer, size);
    }

private:
    rc4_state rc4;
};

int crypto_binary::parse_rc4capi_encryption_header(const uint8_t* data, uint32_t size, bool is_little_endian, rc4_capi_encryption_header_t* capi) {
    if (data == nullptr || size == 0 || capi == nullptr) {
        return BQ::INVALID_ARG;
    }

    uint32_t header_size_min = sizeof(version_b4_t) + sizeof(uint32_t) * 2;
    if (size <= header_size_min) {
        return BQ::ERR_OFFICE_CRYPT_INFO_FMT;
    }

    int nret = BQ::OK;
    do {
        memcpy(&capi->version, data, header_size_min);
        capi->version.major = XsUtils::byteorder2host<uint16_t>(capi->version.major, is_little_endian);
        capi->version.minor = XsUtils::byteorder2host<uint16_t>(capi->version.minor, is_little_endian);
        assert(capi->version.major >= 0x0002 && capi->version.major <= 0x0004 && capi->version.minor == 0x0002);
        capi->flags = XsUtils::byteorder2host<uint32_t>(capi->flags, is_little_endian);
        capi->header_size = XsUtils::byteorder2host<uint32_t>(capi->header_size, is_little_endian);

        auto cur_offset = header_size_min;
        nret = crypto_helper::read_encryption_header(data + cur_offset, size - cur_offset, capi->header_size, &capi->header, is_little_endian);
        if (nret != BQ::OK) {
            break;
        }
        cur_offset += capi->header_size;

        nret = crypto_helper::read_encryption_verifier(data + cur_offset, size - cur_offset, &capi->verifier, is_little_endian);
        if (nret != BQ::OK) {
            break;
        }

        if (capi->header.key_size == 0) {
            capi->header.key_size = 40;
        }

        nret = BQ::OK;
    } while (false);

    return nret;
}

int crypto_binary::parse_rc4_encryption_header(uint8_t* data, uint32_t size, bool is_little_endian, bin_encryption_t* encrypt) {
    if (data == nullptr || size < 4 || nullptr == encrypt) {
        return BQ::INVALID_ARG;
    }

    int nret = BQ::ERR_OFFICE_CRYPT_INFO_FMT;
    encrypt->pass.rc4_base.major = XsUtils::byteorder2host<uint16_t>(*(uint16_t*)(data), is_little_endian);
    encrypt->pass.rc4_base.minor = XsUtils::byteorder2host<uint16_t>(*(uint16_t*)(data + 2), is_little_endian);

    encrypt->type = encrypt->rc4_type();
    if (encrypt->type == encryinfo_bin_rc4) {
        if (size < sizeof(rc4_norm_encryption_header_t)) {
            nret = BQ::ERR_OFFICE_CRYPT_INFO_FMT;
        }
        else {
            memcpy(&encrypt->pass.rc4_norm, data, sizeof(rc4_norm_encryption_header_t));
            auto& norm = encrypt->pass.rc4_norm;
            norm.version.major = XsUtils::byteorder2host<uint16_t>(norm.version.major, is_little_endian);
            norm.version.minor = XsUtils::byteorder2host<uint16_t>(norm.version.minor, is_little_endian);
            nret = BQ::OK;
        }
    }

    if (encrypt->type == encryinfo_bin_rc4_capi) {
        auto& capi = encrypt->pass.rc4_capi;
        nret = parse_rc4capi_encryption_header(data, size, is_little_endian, &capi);
    }

    return nret;
}

uint16_t create_passwd_verifier_m1(const char* passwd) {
    uint16_t verifier = 0;
    do {
        std::vector<uint8_t> pwd_array;
        pwd_array.push_back((uint8_t)(strlen(passwd) & 0xF));

        for (uint8_t i = 0; i < pwd_array[0]; i++) {
            pwd_array.push_back((uint8_t)passwd[i]);
        }

        for (int i = (int)pwd_array.size() - 1; i >= 0; i--) {
            verifier = ((verifier & 0x4000) == 0 ? 0 : 1) | ((verifier * 2) & 0x7FFF);
            verifier ^= pwd_array[i];
        }
        verifier ^= 0xCE4B;
    } while (false);

    return verifier;
}

int binary_xor_m1_passwd_verify(const bin_encryption_t* encrypt, const char16_t* passwd) {
    if (nullptr == encrypt || nullptr == passwd) {
        return BQ::INVALID_ARG;
    }

    std::string passwd_ansi = XsUtils::unicode2ansi(XsUtils::utf2uni(XsUtils::u16s2utf(passwd)));
    if (passwd_ansi.length() > 0xFF) {
        return BQ::ERR_PASSWD;
    }
    if (passwd_ansi.size() > 0xF) {
        passwd_ansi = passwd_ansi.substr(0, 0xF);
    }

    uint16_t verifier = create_passwd_verifier_m1(passwd_ansi.c_str());

    if (verifier != encrypt->pass.xor_.verification_bytes) {
        return BQ::ERR_PASSWD;
    }
    
    return BQ::OK;
}

uint16_t create_xor_key_m1(const char* passwd) {
    uint16_t initial_code[] = { 0xE1F0, 0x1D0F, 0xCC9C, 0x84C0, 0x110C,  0x0E10, 0xF1CE,
            0x313E, 0x1872, 0xE139, 0xD40F, 0x84F9, 0x280C, 0xA96A, 0x4EC3 };
    uint16_t xor_matrix[] = { 0xAEFC, 0x4DD9, 0x9BB2, 0x2745, 0x4E8A, 0x9D14, 0x2A09,
        0x7B61, 0xF6C2, 0xFDA5, 0xEB6B, 0xC6F7, 0x9DCF, 0x2BBF,
        0x4563, 0x8AC6, 0x05AD, 0x0B5A, 0x16B4, 0x2D68, 0x5AD0,
        0x0375, 0x06EA, 0x0DD4, 0x1BA8, 0x3750, 0x6EA0, 0xDD40,
        0xD849, 0xA0B3, 0x5147, 0xA28E, 0x553D, 0xAA7A, 0x44D5,
        0x6F45, 0xDE8A, 0xAD35, 0x4A4B, 0x9496, 0x390D, 0x721A,
        0xEB23, 0xC667, 0x9CEF, 0x29FF, 0x53FE, 0xA7FC, 0x5FD9,
        0x47D3, 0x8FA6, 0x0F6D, 0x1EDA, 0x3DB4, 0x7B68, 0xF6D0,
        0xB861, 0x60E3, 0xC1C6, 0x93AD, 0x377B, 0x6EF6, 0xDDEC,
        0x45A0, 0x8B40, 0x06A1, 0x0D42, 0x1A84, 0x3508, 0x6A10,
        0xAA51, 0x4483, 0x8906, 0x022D, 0x045A, 0x08B4, 0x1168,
        0x76B4, 0xED68, 0xCAF1, 0x85C3, 0x1BA7, 0x374E, 0x6E9C,
        0x3730, 0x6E60, 0xDCC0, 0xA9A1, 0x4363, 0x86C6, 0x1DAD,
        0x3331, 0x6662, 0xCCC4, 0x89A9, 0x0373, 0x06E6, 0x0DCC,
        0x1021, 0x2042, 0x4084, 0x8108, 0x1231, 0x2462, 0x48C4 };

    uint16_t passwd_size = (uint16_t)std::min<size_t>(strlen(passwd), sizeof(initial_code) / sizeof(uint16_t));
    uint16_t xor_key = initial_code[passwd_size - 1];
    for (int i = (int)passwd_size - 1, cur_elem = 0x68; i >= 0; i--) {
        uint8_t c = passwd[i];
        for (int j = 0; j < 7; j++) {
            if (c & 0x40) {
                xor_key ^= xor_matrix[cur_elem];
            }
            c *= 2;
            cur_elem--;
        }
    }
    return xor_key;
}

int binary_xor_m1_array_init(const bin_encryption_t* encrypt, const char16_t* passwd, uint8_t* out, uint32_t& size) {
    if (nullptr == encrypt || nullptr == passwd || nullptr == out || size == 0) {
        return BQ::INVALID_ARG;
    }

    std::string passwd_ansi = XsUtils::unicode2ansi(XsUtils::utf2uni(XsUtils::u16s2utf(passwd)));
    if (passwd_ansi.length() > 0xFF) {
        return BQ::ERR_PASSWD;
    }
    if (passwd_ansi.size() > 0xF) {
        passwd_ansi = passwd_ansi.substr(0, 0xF);
    }    

    int nret = BQ::OK;
    do {
        uint8_t pad_array[] = { 0xBB, 0xFF, 0xFF, 0xBA, 0xFF, 0xFF, 0xB9, 0x80, 0x00,
            0xBE, 0x0F, 0x00, 0xBF, 0x0F, 0x00 };

        uint8_t obfuscation_array[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

        auto xor_ror = [&](uint8_t a, uint8_t b) -> uint8_t {
            uint8_t byte = a ^ b;
            return ((byte >> 1) | (byte << 7)) % 0x100;
        };

        uint16_t xor_key = create_xor_key_m1(passwd_ansi.c_str());
        assert(xor_key == encrypt->pass.xor_.key);
        uint16_t index = (uint16_t)passwd_ansi.size();
        if (index % 2 == 1) {
            obfuscation_array[index] = xor_ror(pad_array[0], (xor_key & 0xFF00) >> 8);
            index--;
            obfuscation_array[index] = xor_ror(passwd_ansi.at(passwd_ansi.size() - 1), (xor_key & 0x00FF));
        }

        while (index > 0) {
            index--;
            obfuscation_array[index] = xor_ror(passwd_ansi.at(index), (xor_key & 0xFF00) >> 8);
            index--;
            obfuscation_array[index] = xor_ror(passwd_ansi.at(index), (xor_key & 0x00FF));
        }

        index = 15;
        for (int pad_index = 15 - passwd_ansi.size(); pad_index > 0;) {
            obfuscation_array[index] = xor_ror(pad_array[pad_index], (xor_key & 0xFF00) >> 8);
            index--;
            pad_index--;
            obfuscation_array[index] = xor_ror(pad_array[pad_index], (xor_key & 0x00FF));
            index--;
            pad_index--;
        }

        /* now, we get the obfuscation_array */
        if (size < sizeof(obfuscation_array)) {
            nret = BQ::INVALID_ARG;
            break;
        }
        size = sizeof(obfuscation_array);
        memcpy(out, obfuscation_array, size);
        nret = BQ::OK;
    } while (false);

    return nret;
}

int binary_xor_m1_encrypt_inplace(const uint8_t* xor_array, uint32_t xor_array_index, uint8_t* data, uint32_t size, uint32_t xor_array_size=16) {
    if (xor_array_index >= xor_array_size || data == nullptr) {
        return BQ::INVALID_ARG;
    }

    for (uint32_t i = 0; i < size; i++) {
        uint8_t v = *(data + i);
        *(data + i) = ((v << 5) | (v >> 3)) ^ xor_array[xor_array_index];
        xor_array_index = (xor_array_index + 1) % xor_array_size;
    }
    return BQ::OK;
}

int binary_xor_m1_decrypt_inplace(const uint8_t* xor_array, uint32_t xor_array_index, uint8_t* data, uint32_t size, uint32_t xor_array_size=16) {
    if (data == nullptr) {
        return BQ::INVALID_ARG;
    }
    xor_array_index %= xor_array_size;

    for (uint32_t i = 0; i < size; i++) {
        uint8_t v = *(data + i) ^ xor_array[xor_array_index];
        *(data + i) = (v >> 5) | (v << 3);
        xor_array_index = (xor_array_index + 1) % xor_array_size;
    }
    return BQ::OK;
}

uint32_t create_passwd_verifier_m2(const char* passwd) {
    uint32_t verifier = 0;

    do {
        uint16_t key_high = create_xor_key_m1(passwd);
        uint16_t key_low = create_passwd_verifier_m1(passwd);
        verifier = key_high << 16 | key_low;
    } while (false);

    return verifier;
}

int binary_xor_m2_passwd_verify(const bin_encryption_t* encrypt, const char16_t* passwd) {
    if (nullptr == encrypt || nullptr == passwd) {
        return BQ::INVALID_ARG;
    }

    std::string passwd_ansi = XsUtils::unicode2ansi(XsUtils::utf2uni(XsUtils::u16s2utf(passwd)));
    if (passwd_ansi.length() > 0xFF) {
        return BQ::ERR_PASSWD;
    }   
    if (passwd_ansi.size() > 0xF) {
        passwd_ansi = passwd_ansi.substr(0, 0xF);
    }

    if (create_passwd_verifier_m2(passwd_ansi.c_str()) != encrypt->pass.xor_m2) {
        return BQ::ERR_PASSWD;
    }

    return BQ::OK;
}

int binary_xor_m2_array_init(const bin_encryption_t* encrypt, const char16_t* passwd, uint8_t* out, uint32_t& size) {
    if (nullptr == encrypt || nullptr == passwd || nullptr == out || size == 0) {
        return BQ::INVALID_ARG;
    }

    std::string passwd_ansi = XsUtils::unicode2ansi(XsUtils::utf2uni(XsUtils::u16s2utf(passwd)));
    if (passwd_ansi.length() > 0xFF) {
        return BQ::ERR_PASSWD;
    }
    if (passwd_ansi.size() > 0xF) {
        passwd_ansi = passwd_ansi.substr(0, 0xF);
    }

    int nret = BQ::OK;
    do {
        uint32_t verifier = create_passwd_verifier_m2(passwd_ansi.c_str());
        uint8_t key_h = ((verifier >> 16) & 0xFF00) >> 8;
        uint8_t key_l = ((verifier >> 16) & 0x00FF);

        auto ror = [&](uint8_t byte) -> uint8_t {
            return ((byte >> 1) | (byte << 7)) % 0x100;
        };

        uint8_t pad_array[] = { 0xBB, 0xFF, 0xFF, 0xBA, 0xFF, 0xFF, 0xB9, 0x80,
            0x00, 0xBE, 0x0F, 0x00, 0xBF, 0x0F, 0x00 };

        uint8_t obfuscation_array[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

        uint32_t index = 0;
        for (; index < passwd_ansi.size(); index++) {
            obfuscation_array[index] = passwd_ansi[index];
        }
        for (; index < 0x10; index++) {
            obfuscation_array[index] = pad_array[index - passwd_ansi.size()];
        }

        for (index = 0; index < 0x10; ) {
            obfuscation_array[index] = ror(obfuscation_array[index] ^ key_l);
            index++;
            obfuscation_array[index] = ror(obfuscation_array[index] ^ key_h);
            index++;
        }

        /* now, we get the obfuscation_array */
        if (size < sizeof(obfuscation_array)) {
            nret = BQ::INVALID_ARG;
            break;
        }
        size = sizeof(obfuscation_array);
        memcpy(out, obfuscation_array, size);
        nret = BQ::OK;
    } while (false);

    return nret;
}

int binary_xor_m2_decrypt_inplace(const uint8_t* xor_array, uint32_t xor_array_index, uint8_t* data, uint32_t size, uint32_t xor_array_size = 16) {
    if (data == nullptr) {
        return BQ::INVALID_ARG;
    }
    xor_array_index %= xor_array_size;

    uint8_t v = 0;
    for (uint32_t i = 0; i < size; i++) {
        v = *(data + i) ^ xor_array[xor_array_index];
        if (0x0 != *(data + i) && 0x0 != v) {
            *(data + i) = v;
        }
        xor_array_index = (xor_array_index + 1) % xor_array_size;
    }
    return BQ::OK;
}

int binary_rc4_capi_encryption_key(const rc4_capi_encryption_header_t* encrypt, const char16_t* passwd, uint32_t block_num, uint8_t* out, uint32_t size) {
    if (encrypt == nullptr || nullptr == passwd || out == nullptr || 0 == size) {
        return BQ::INVALID_ARG;
    }

    uint8_t hash[SHA1_VAl_BYTES] = { 0 };

    assert(encrypt->verifier.salt_size == 0x10);
    sha1 sha1_calc;
    sha1_calc.add(encrypt->verifier.salt, encrypt->verifier.salt_size).add(passwd, ucs::len<char16_t>(passwd) * sizeof(char16_t)).finalize();
    memcpy(hash, sha1_calc.digest(), sizeof(hash));

    sha1 sha1_final;
    sha1_final.add(hash, sizeof(hash)).add(&block_num, sizeof(uint32_t)).finalize();
    memcpy(hash, sha1_final.digest(), sizeof(hash));

    assert(encrypt->header.key_size == RC4_CAPI_KEY_LEN || encrypt->header.key_size == 40);
    uint32_t key_len = encrypt->header.key_size / 8;

    if (key_len > SHA1_VAl_BYTES || key_len > size) {
        return BQ::ERR;
    }

    memset(out, 0, size);
    memcpy(out, hash, key_len);

    return BQ::OK;
}

int binary_rc4_capi_passwd_verify(const bin_encryption_t* encryption, const char16_t* passwd) {
    if (encryption == nullptr || nullptr == passwd) {
        return BQ::INVALID_ARG;
    }
    auto encrypt = &encryption->pass.rc4_capi;

    /* rc4 */
    assert(0 == (encrypt->header.flags & 0x20) && (encrypt->header.algid == 0 || encrypt->header.algid == 0x6801));
    /* sha1 */
    assert(encrypt->header.algid_hash == 0 || encrypt->header.algid_hash == 0x8004);

    uint8_t encrypt_key[RC4_CAPI_KEY_LEN / 8] = {0 };
    uint8_t encrypt_key_size = sizeof(encrypt_key);

    int nret = binary_rc4_capi_encryption_key(encrypt, passwd, 0, encrypt_key, sizeof(encrypt_key));
    if (nret != BQ::OK) {
        return nret;
    }

    RC4Helper rc4(encrypt_key, encrypt_key_size);
    uint8_t verifier[16] = { 0 };
    memcpy(verifier, encrypt->verifier.encrypted_verifier, sizeof(verifier));
    rc4.update(verifier, sizeof(verifier));

    uint8_t verifier_hash[20] = { 0 };
    memcpy(verifier_hash, encrypt->verifier.encrypted_verifier_hash, sizeof(verifier_hash));
    rc4.update(verifier_hash, sizeof(verifier_hash));
    assert(encrypt->verifier.verifier_hash_size == 20);

    uint8_t verifier_sha1[SHA1_VAl_BYTES] = { 0 };
    memcpy(verifier_sha1, sha1().add(verifier, sizeof(verifier)).finalize().digest(), sizeof(verifier_sha1));

    if (0 != memcmp(verifier_hash, verifier_sha1, sizeof(verifier_sha1))) {
        nret = BQ::ERR_PASSWD;
    }
    else {
        nret = BQ::OK;
    }

    return nret;
}

int binary_rc4_norm_encryption_key(const rc4_norm_encryption_header_t* encrypt, const char16_t* passwd, uint32_t block_num, uint8_t* out, uint32_t size) {
    if (encrypt == nullptr || nullptr == passwd || out == nullptr || 0 == size) {
        return BQ::INVALID_ARG;
    }

    assert(sizeof(encrypt->salt) == 0x10);
    const uint32_t truncated_hash_size = 5;
    uint8_t hash[MD5_VAl_BYTES + truncated_hash_size] = { 0 };

    {
        md5::MD5 hash_calc;
        hash_calc.Update((unsigned char*)passwd, ucs::len<char16_t>(passwd) * sizeof(char16_t));
        hash_calc.Final();
        memcpy(hash, hash_calc.digestRaw, truncated_hash_size);
        memcpy(hash + truncated_hash_size, encrypt->salt, sizeof(encrypt->salt));
    }
    {
        md5::MD5 hash_mediate;
        for (size_t i = 0; i < 16; i++) {
            hash_mediate.Update(hash, sizeof(hash));
        }
        hash_mediate.Final();
        memcpy(hash, hash_mediate.digestRaw, MD5_VAl_BYTES);
    }
    {
        md5::MD5 hash_final;
        hash_final.Update(hash, truncated_hash_size);
        hash_final.Update((unsigned char*)&block_num, sizeof(uint32_t));
        hash_final.Final();
        memcpy(hash, hash_final.digestRaw, MD5_VAl_BYTES);
    }

    if (RC4_KEY_LEN / 8 > size || RC4_KEY_LEN / 8 > MD5_VAl_BYTES) {
        return BQ::ERR;
    }
    memset(out, 0, size);
    memcpy(out, hash, RC4_KEY_LEN / 8);

    return BQ::OK;
}

int binary_rc4_norm_passwd_verify(const bin_encryption_t* encryption, const char16_t* passwd) {
    if (encryption == nullptr || nullptr == passwd) {
        return BQ::INVALID_ARG;
    }
    auto encrypt = &encryption->pass.rc4_norm;

    uint8_t encrypt_key[RC4_KEY_LEN / 8] = { 0 };
    uint8_t encrypt_key_size = sizeof(encrypt_key);

    int nret = binary_rc4_norm_encryption_key(encrypt, passwd, 0, encrypt_key, sizeof(encrypt_key));
    if (nret != BQ::OK) {
        return nret;
    }

    RC4Helper rc4(encrypt_key, encrypt_key_size);
    uint8_t verifier[16] = { 0 };
    memcpy(verifier, encrypt->encrypted_verifier, sizeof(verifier));
    rc4.update(verifier, sizeof(verifier));

    uint8_t verifier_hash[16] = { 0 };
    memcpy(verifier_hash, encrypt->encrypted_verifier_hash, sizeof(verifier_hash));
    rc4.update(verifier_hash, sizeof(verifier_hash));

    md5::MD5 calc_hash;
    calc_hash.Update((unsigned char*)verifier, sizeof(verifier));
    calc_hash.Final();

    if (0 != memcmp(verifier_hash, calc_hash.digestRaw, sizeof(verifier_hash))) {
        nret = BQ::ERR_PASSWD;
    }
    else {
        nret = BQ::OK;
    }

    return nret;
}

int crypto_binary::passwd_verify(const bin_encryption_t* encrypt, const char16_t* passwd) {
    int nret = BQ::OK;

    switch (encrypt->type) {
    case encryinfo_bin_xor_m1: {
        nret = binary_xor_m1_passwd_verify(encrypt, passwd);
        if (nret != BQ::OK) break;

        break;
    }
    case encryinfo_bin_xor_m2: {
        nret = binary_xor_m2_passwd_verify(encrypt, passwd);
        if (nret != BQ::OK) break;

        break;
    }
    case encryinfo_bin_rc4: {
        nret = binary_rc4_norm_passwd_verify(encrypt, passwd);
        if (nret != BQ::OK) break;

        break;
    }
    case encryinfo_bin_rc4_capi: {
        nret = binary_rc4_capi_passwd_verify(encrypt, passwd);
        if (nret != BQ::OK) break;

        break;
    }
    default:
        nret = BQ::NOT_IMPLEMENT;
    }
    return nret;
}

int crypto_binary::decrypt_inplace(const bin_encryption_t* encrypt, const char16_t* passwd, uint8_t* data, uint32_t size, const block_info_t* block) {
    int nret = BQ::OK;

    auto makesure_blocksize = [](uint32_t bs, uint32_t default_bs) {
        if (bs == 0) return default_bs;
        return (0 == bs % 8) ? bs : (bs / 8 + 1) * 8;
    };
    auto makesure_blockno = [](const block_info_t* block, uint32_t block_size, uint32_t offset) -> uint32_t {
        if (block == nullptr) {
            return offset / block_size;
        }
        return block->is_no_constant ? block->no_constant : (block->offset_relative + offset) / block_size;
    };

    switch (encrypt->type) {
    case encryinfo_bin_xor_m1: {
        uint8_t xor_array[16] = { 0 };
        uint32_t xor_array_size = sizeof(xor_array);
        nret = binary_xor_m1_array_init(encrypt, passwd, xor_array, xor_array_size);
        if (nret != BQ::OK) break;

        nret = binary_xor_m1_decrypt_inplace(xor_array, block ? block->xor_array_index : 0, data, size);
        if (nret != BQ::OK) break;

        break;
    }
    case encryinfo_bin_xor_m2: {
        uint8_t xor_array[16] = { 0 };
        uint32_t xor_array_size = sizeof(xor_array);
        nret = binary_xor_m2_array_init(encrypt, passwd, xor_array, xor_array_size);
        if (nret != BQ::OK) break;

        nret = binary_xor_m2_decrypt_inplace(xor_array, block ? block->xor_array_index : 0, data, size);
        if (nret != BQ::OK) break;

        break;
    }
    case encryinfo_bin_rc4: {
        uint8_t encrypt_key[RC4_KEY_LEN / 8] = { 0 };
        uint32_t block_size = makesure_blocksize(block ? block->block_size : 0, RC4_BLOCK_SIZE);

        for (uint32_t i = 0, blockno = 0, chunksize = 0; i < size; i += chunksize) {
            chunksize = block_size - (block ? block->offset_relative + i : i) % block_size;
            blockno = makesure_blockno(block, block_size, i);

            nret = binary_rc4_norm_encryption_key(&encrypt->pass.rc4_norm, passwd, blockno, encrypt_key, sizeof(encrypt_key));
            if (nret != BQ::OK) {
                break;
            }
            RC4Helper(encrypt_key, sizeof(encrypt_key)).update(data + i, std::min<uint32_t>(chunksize, size - i));
        }
        break;
    }
    case encryinfo_bin_rc4_capi: {
        uint8_t encrypt_key[RC4_CAPI_KEY_LEN / 8] = { 0 };
        uint32_t block_size = makesure_blocksize(block ? block->block_size : 0, RC4_CAPI_BLOCK_SIZE);

        for (uint32_t i = 0, blockno = 0, chunksize = 0; i < size; i += chunksize) {
            chunksize = block_size - (block ? block->offset_relative + i : i) % block_size;
            blockno = makesure_blockno(block, block_size, i);

            nret = binary_rc4_capi_encryption_key(&encrypt->pass.rc4_capi, passwd, blockno, encrypt_key, sizeof(encrypt_key));
            if (nret != BQ::OK) {
                break;
            }
            RC4Helper(encrypt_key, sizeof(encrypt_key)).update(data + i, std::min<uint32_t>(chunksize, size - i));
        }
        break;
    }
    default:
        nret = BQ::NOT_IMPLEMENT;
    }
    return nret;
}