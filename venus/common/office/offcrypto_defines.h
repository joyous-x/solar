/////////////////////////////////////////////////////////////////
//
//	reference: [MS-OFFCRYPTO] - v20210817
//
/////////////////////////////////////////////////////////////////

#pragma once
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#pragma pack(push, 1)

#define DOC_RC4_BLOCK_SIZE      (512)
#define DOC_RC4CAPI_BLOCK_SIZE  (512)
#define DOC_FIB_NOT_ENCRYPTED   (68)

#define RC4_KEY_LEN             (128)
#define RC4_CAPI_KEY_LEN        (128)
#define RC4_BLOCK_SIZE          (1024)
#define RC4_CAPI_BLOCK_SIZE     (1024)
#define XOR_M1_BLOCK_SIZE       (1024)
#define SHA1_VAl_BYTES          (20)
#define MD5_VAl_BYTES           (16)
#define AES_BLOCK_SIZE          (0x10)
#define AGILE_CB_HASH_MAX       (0x40)

/* default password */
#define excel_passwd_default    (u"VelvetSweatshop")
#define ppt_passwd_default      (u"\001Hannes Ruescher\001")

/* Password MUST NOT be longer than 15 characters */
const int xor_m1_passwd_char_size_max = 15;

enum encryption_type {
    encryinfo_unknown = 0,

    encryinfo_ecma376_std,
    encryinfo_ecma376_agile,
    encryinfo_ecma376_extensible,
    encryinfo_bin_xor_m1,
    encryinfo_bin_xor_m2,
    encryinfo_bin_rc4,
    encryinfo_bin_rc4_capi,

    encryinfo_ignore,
};

/*
* Length-Prefixed Padded Unicode String (UNICODE-LP-P4)
*
* Length-Prefixed UTF-8 String (UTF-8-LP-P4)
*/
typedef struct __st_mbcs_lp_p4 {
	uint32_t length;	/* data bytes，UNICODE-LP-P4：must be MUST be a multiple of 2 bytes */
	uint8_t  data[1];	/* variable， it MUST NOT be null-terminated */
	uint8_t  padding[1]; /* variable， a set of bytes that MUST be of the correct size such that the size of the UNICODE-LP-P4 structure is a multiple of 4 bytes. */
}unicode_lp_p4_t, utf8_lp_p4_t;

typedef struct __st_version_b4 {
    uint16_t major{ 0 };
    uint16_t minor{ 0 };
} version_b4_t;

/*
* The EncryptionHeader structure is used by ECMA-376 document encryption and Office binary document RC4 CryptoAPI encryption.
*/
typedef struct __st_encryption_header {
	uint32_t        flags;
	uint32_t        size_extra;
	uint32_t        algid;
	uint32_t        algid_hash;
	uint32_t        key_size;
	uint32_t        provider_type;
	uint32_t        reserved1;
	uint32_t        reserved2;
	/*
	* variable, null-terminated Unicode string that specifies the CSP name. CSP, cryptographic service provider
	* simply, only gived 128 bytes
	*/
	uint8_t			csp_name[128];

	/* A flag that specifies whether CryptoAPI RC4 or ECMA-376 encryption is used */
	bool cryptoapi() { return (flags & 0x04); }
	/* MUST be 1 if extensible encryption is used. */
	bool external() { return (flags & 0x10); }
	/*
	* MUST be 1 if the protected content is an ECMA-376 document. otherwise, it MUST be 0.
	* If the fAES bit is 1, the fCryptoAPI bit MUST also be 1.
	*/
	bool aes() { return (flags & 0x20); }
} encryption_header_t;

/*
* The EncryptionVerifier structure is used by
* Office Binary Document RC4 CryptoAPI Encryption and ECMA-376 Document Encryption
*/
typedef struct __st_encryption_verifier {
	uint32_t	salt_size; /* must be 0x00000010 */
	uint8_t		salt[16];
	uint8_t		encrypted_verifier[16];
	uint32_t	verifier_hash_size;
	/*
	* variable, should be 32 or others.
	*	If the encryption algorithm is RC4, the length MUST be 20 bytes.
	*	If the encryptionalgorithm is AES, the length MUST be 32 bytes.
	*/
	uint8_t		encrypted_verifier_hash[64];
} encryption_verifier_t;

/* 
* 40-bit RC4 Encryption Header
*/
typedef struct __st_rc4_norm_encryption_header {
    version_b4_t version; /* Version.vMajor MUST be 0x0001 and Version.vMinor MUST be 0x0001*/
    uint8_t		salt[16];
    uint8_t		encrypted_verifier[16];
    uint8_t		encrypted_verifier_hash[16];
} rc4_norm_encryption_header_t;

/* 
* RC4 CryptoAPI Encryption Header
*/
typedef struct __st_rc4_capi_encryption_header {
    version_b4_t			version; /* Version.vMajor MUST be 0x0002, 0x0003, or 0x0004 and Version.vMinor MUST be 0x0002. */
    uint32_t				flags;
    uint32_t				header_size;
	encryption_header_t		header;
	encryption_verifier_t	verifier;
} rc4_capi_encryption_header_t;

typedef struct __st_xor_obfuscation {
	uint16_t key;
	uint16_t verification_bytes;
} xlm_xor_obfuscation_t;

typedef struct __st_bin_encryption {
    uint16_t                         type{ 0 };

    union un_bin_encryption_t {
        xlm_xor_obfuscation_t        xor_;
        uint32_t                     xor_m2;
        version_b4_t                 rc4_base;
        rc4_norm_encryption_header_t rc4_norm;
        rc4_capi_encryption_header_t rc4_capi;

        un_bin_encryption_t() { reset(); }

        void reset() {
            memset((void*)this, 0, sizeof(un_bin_encryption_t));
        }
    } pass;

    int rc4_type() const {
        auto& rc4 = pass.rc4_base;
        if (rc4.major == 0x0001 && rc4.minor == 0x0001) {
            return encryinfo_bin_rc4;
        }
        if (rc4.major >= 0x0002 && rc4.major <= 0x0004 && rc4.minor == 0x0002) {
            return encryinfo_bin_rc4_capi;
        }
        return encryinfo_unknown;
    }
} bin_encryption_t;

typedef struct __st_encryption_info_base {
    version_b4_t		version;
    uint32_t			flags;
} encryption_info_base_t;

/*
* information in the EncryptionInfo stream (Standard Encryption)
*/
typedef struct __st_encryption_info_std : encryption_info_base_t {
    uint32_t			header_size;
    encryption_header_t	header;	 /* variable */
    encryption_verifier_t verifier;

    void reset() {
        memset((void*)&header_size, 0, sizeof(__st_encryption_info_std) - sizeof(encryption_info_base_t));
    }
} encryption_info_std_t;

/*
* information in the EncryptionInfo stream (Agile Encryption)
*
* flags field : reserved, MUST be 0x00000040
*/
typedef struct __st_encryption_info_agile : encryption_info_base_t {
    struct st_agile_keydata {
        uint32_t        saltsize;
        uint32_t        blocksize;
        uint32_t        keybits;
        uint32_t        hashsize;
        char            cipher_algor[16];
        char            cipher_chain[32];
        char            hash_algor[16];
        uint8_t*        salt;
    } keydata;

    struct st_agile_dataintegrity {
        char*           encrypted_hmac_key;
        char*           encrypted_hmac_val;
    } integrity;

    struct st_agile_keyencryptor {
        uint32_t        spin_cnt;
        uint32_t        saltsize;
        uint32_t        blocksize;
        uint32_t        keybits;
        uint32_t        hashsize;
        char            cipher_algor[16];
        char            cipher_chain[32];
        char            hash_algor[16];
        uint8_t*        salt;
        char*           encrypted_verifier_hash_input;
        char*           encrypted_verifier_hash_value;
        char*           encrypted_keyval;
    } encryptor;

    void reset() { 
        memset((void*)&keydata, 0, sizeof(__st_encryption_info_agile) - sizeof(encryption_info_base_t)); 
    }

    void release() {
        auto free_buffer = [](char** v) { if (v && *v) { free(*v); *v = nullptr; } };
        free_buffer((char**)&keydata.salt);
        free_buffer((char**)&encryptor.salt);
        free_buffer(&encryptor.encrypted_verifier_hash_input);
        free_buffer(&encryptor.encrypted_verifier_hash_value);
        free_buffer(&encryptor.encrypted_keyval);
        free_buffer(&integrity.encrypted_hmac_key);
        free_buffer(&integrity.encrypted_hmac_val);
        reset();
    }

    void move_to(__st_encryption_info_agile& v) {
        memcpy((void*)&v, (void*)this, sizeof(__st_encryption_info_agile));
        release();
    }
protected:
    __st_encryption_info_agile(const __st_encryption_info_agile& v) { reset(); };
    __st_encryption_info_agile& operator =(const __st_encryption_info_agile& v) { return *this; };
} encryption_info_agile_t;

/*
* information in the EncryptionInfo stream (Extensible Encryption)
*
* flags field : MUST have the fExternal bit set to 1. All other bits MUST be set to 0.
*/
typedef struct __st_encryption_info_ext : encryption_info_base_t {
    uint32_t            header_size;
    encryption_header_t header; /* variable */
    uint8_t             xml_data[1]; /* EncryptionData, variable, The first Unicode code point MUST be 0xFEFF*/
    encryption_verifier_t verifier;

    void reset() {
        memset((void*)&header_size, 0, sizeof(__st_encryption_info_ext) - sizeof(encryption_info_base_t));
    }
} encryption_info_ext_t;

union encryption_info_t {
    encryption_info_base_t base;
    encryption_info_std_t std;
    encryption_info_agile_t agile;
    encryption_info_ext_t ext;

    encryption_info_t() { reset(); }
    ~encryption_info_t() { release(); }

    int type() const {
        auto& version = base.version;
        if (version.minor == 0x4 && version.major == 0x4) {
            if (base.flags == 0x0040) return encryinfo_ecma376_agile;
        }
        if (version.minor == 0x3 && (version.major == 0x3 || version.major == 0x4)) {
            return encryinfo_ecma376_extensible;
        }
        if (version.minor == 0x2 && (version.major >= 0x2 && version.major <= 0x4)) {
            return encryinfo_ecma376_std;
        }
        return encryinfo_unknown;
    }

    void reset() {
        memset((void*)this, 0, sizeof(encryption_info_t));
    }

    void release() {
        switch (type()) {
            case encryinfo_ecma376_agile: {
                agile.release();
                break;
            }
            case encryinfo_ecma376_extensible: {
                ext.reset();
                break;
            }
            case encryinfo_ecma376_std: {
                std.reset();
                break;
            }
            default: {
                reset();
            }
        }
    }

protected:
    encryption_info_t(const __st_encryption_info_agile& v);
    encryption_info_t& operator =(const encryption_info_t& v);
};

#pragma pack(pop)

