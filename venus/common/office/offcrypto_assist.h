/////////////////////////////////////////////////////////////////
//
//	reference: [MS-OFFCRYPTO] - v20210817
//
/////////////////////////////////////////////////////////////////

#pragma once
#include "offcrypto_defines.h"
#include "offcrypto_ds.h"
#include "ole.h"

typedef struct __st_crypto_dataspace {
    uint32_t encrypt_info_did;
    uint32_t encrypt_pkg_did;
    uint32_t ds_did;
    uint32_t ds_tran_did;
    uint32_t ds_info_did;
    uint32_t ds_map_did;
    uint32_t ds_ver_did;

    ds_version_t	version;
    ds_map_t		map;
    std::vector<ds_definition_t> definitions;
    std::vector<ds_transform_t>  transforms;

    encryption_info_t	encrypt_info;

    __st_crypto_dataspace() { reset(); }

    void reset() {
        memset(this, 0, (uint8_t*)&map- (uint8_t*)this);
        map.entry_count = 0;
        map.header_length = 0;
        map.entries.clear();
        definitions.clear();
        transforms.clear();
        encrypt_info.reset();
    }

    bool encrypted() { return (encrypt_pkg_did && encrypt_info_did); }
} crypto_dataspace_t;


class crypto_ds {
public:
    int is_ole_encrypted(const olefile_t* ole, crypto_dataspace_t* data = nullptr);

    int parse_dataspace_encryption(const olefile_t* ole, crypto_dataspace_t* data);

    int parse_stream_ds_version(const olefile_t* ole, uint32_t version_did, ds_version_t* version);

    int parse_stream_ds_map(const olefile_t* ole, uint32_t ds_map_did, ds_map_t* map);

    int parse_stream_ds_definition(const olefile_t* ole, uint32_t ds_info_did, const char16_t* entry, ds_definition_t* definition);

    int parse_stream_ds_transform(const olefile_t* ole, uint32_t ds_tran_did, const char16_t* name, ds_transform_t* transform);

    int parse_stream_encryption_info(const olefile_t* ole, uint32_t encryption_info_did, encryption_info_t* encrypt_info);

protected:
    const bool is_little_endian{ true };
};

class crypto_ecma376 {
public:
    int passwd_verify(encryption_info_t* encrypt, const char16_t* passwd);

    int decrypt(encryption_info_t* encrypt, const char16_t* passwd, uint8_t* data, uint32_t size, ifile* out);
};

class crypto_binary {
public:
    typedef struct __st_block_info {
        bool is_no_constant{ 0 };
        uint32_t no_constant{ 0 };
        uint32_t block_size{ 0 };
        uint32_t offset_relative{ 0 };
        uint32_t xor_array_index{ 0 };
    } block_info_t;

    int parse_xlm_filepass_record(uint8_t* data, uint32_t size, bool is_little_endian, bin_encryption_t* encrypt);

    /**
     * @brief pasre the rc4 or rc4capi encryption header
     */
    int parse_rc4_encryption_header(uint8_t* data, uint32_t size, bool is_little_endian, bin_encryption_t* encrypt);

    /**
     * @brief pasre the rc4capi encryption header
     */
    int parse_rc4capi_encryption_header(const uint8_t* data, uint32_t size, bool is_little_endian, rc4_capi_encryption_header_t* capi);

    /**
     * @brief verify the password
     */
    int passwd_verify(const bin_encryption_t* encrypt, const char16_t* passwd);

    /**
     * @brief decrypt inplace
     */
    int decrypt_inplace(const bin_encryption_t* encrypt, const char16_t* passwd, uint8_t* data, uint32_t size, const block_info_t* block = 0);
};

class crypto_helper {
public:
	static int parse_unicode_lp_p4(const uint8_t* data, uint32_t size, std::u16string& val, uint32_t* raw_byte_size=nullptr, bool little_endian=true);

	static int parse_utf8_lp_p4(const uint8_t* data, uint32_t size, std::string& val, uint32_t* raw_byte_size = nullptr, bool little_endian = true);

	static int read_encryption_header(const uint8_t* data, uint32_t size, uint32_t header_size, encryption_header_t* header, bool little_endian = true);

	static int read_encryption_verifier(const uint8_t* data, uint32_t size, encryption_verifier_t* verifier, bool little_endian = true);
};
