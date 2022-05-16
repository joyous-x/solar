#pragma once
#include "ole.h"
#include "offcrypto_defines.h"

#pragma pack(push, 1)

typedef struct __st_record_header {
    uint16_t recVerAndInstance;
    uint16_t recType;
    uint32_t recLen;

    uint8_t recVer() { return uint8_t(recVerAndInstance & 0xF); }
    uint16_t recInstance() { return uint16_t(recVerAndInstance >> 0x4); }
    bool is_container() { return 0xF == (recVerAndInstance & 0xF); }
} recordheader_t;

typedef struct __st_UserEditAtom {
    recordheader_t header;
    uint32_t       last_slide_id_ref;
    uint16_t       version;       // should be ignored
    uint8_t        minor_version; // MUST be 0x00
    uint8_t        major_version; // MUST be 0x03
    uint32_t       offset_last_edit;
    uint32_t       offset_persist_dir; // MUST be greater than offsetLastEdit and less than the offset, in bytes, of this UserEditAtom record
    uint32_t       doc_persist_id_ref;
    uint32_t       persist_id_seed;
    uint16_t       last_view;
    uint16_t       unused;
    uint32_t       encrypt_session_persist_id_ref; // optional, 

    __st_UserEditAtom() { memset((void*)this, 0, sizeof(__st_UserEditAtom)); }
} useredit_atom_t;

typedef struct __st_PersistDirectoryEntry {
    uint32_t                persist_id_cnt;
    uint32_t                persist_offset[1]; // variable
    uint32_t id() { return persist_id_cnt & 0x000FFFFF; } // MUST be greater than or equal to 0x001
    uint32_t cnt() { return (persist_id_cnt & 0xFFF0000) >> 20; }
} persist_dir_entry_t;

/*
* @describe persist_dir_entry_item_t 
*   it is for easier access to PersistDirectoryEntry.
*   expanding a PersistDirectoryEntry can get one or more persist_dir_entry_item_t items.
*/
typedef struct __st_persist_dir_entry_item {
    uint32_t                persist_id;
    uint32_t                persist_offset;
} persist_dir_entry_item_t;

typedef struct __st_PersistDirectoryAtom {
    recordheader_t            header;
    /*
    * rgPersistDirEntry, variable
    *   An array of PersistDirectoryEntry. The size, in bytes, of the array is specified by rh.recLen
    */
    persist_dir_entry_item_t* entries;
    uint32_t                  entries_size;

    __st_PersistDirectoryAtom() { memset((void*)this, 0, sizeof(__st_PersistDirectoryAtom)); }
} persist_dir_atom_t;

typedef struct __st_CurrentUserAtom {
    recordheader_t header;
    uint32_t       size; // MUST be 0x00000014
    uint32_t       header_token;
    uint32_t       offset_currentedit; // offset of the UserEditAtom structure
    uint16_t       len_username; // MUST be less than or equal to 255
    uint16_t       file_version; // MUST be 0x03F4
    uint8_t        major_version; // MUST be 0x03
    uint8_t        minor_version; // MUST be 0x00
    uint16_t       unused;
    char           ansi_username[256]; // variable，PrintableAnsiString, the user name of the last user to modify the file
    uint32_t       rel_version;
    char16_t*       unic_username; // optional，variable, An optional PrintableUnicodeString

    __st_CurrentUserAtom() { memset((void*)this, 0, sizeof(__st_CurrentUserAtom)); }
} current_user_atom_t;

typedef struct __st_CryptSession10Container {
    recordheader_t                  header;
    rc4_capi_encryption_header_t    rc4_capi;

    __st_CryptSession10Container() { memset((void*)this, 0, sizeof(__st_CryptSession10Container)); }
} crypt_session10container_t;

#pragma pack(pop)

typedef struct __st_msppt_t : olefile_t {
    // Unless otherwise specified, all data in the PowerPoint Binary File Format is stored in little-endian format
    bool                big_endian{ 0 };
    uint32_t            did_currentuser{ 0 };
    uint32_t            did_document{ 0 };

    current_user_atom_t currentuser;
    useredit_atom_t     useredit;
    persist_dir_atom_t  persist_dirs;
    crypt_session10container_t crypt;

    ~__st_msppt_t() { release(); }

    bool encrypted() const {
        return (currentuser.header_token == 0xF3D1C4DF || useredit.encrypt_session_persist_id_ref);
    }

    void release() {
        olefile_t::release();
        if (persist_dirs.entries) { 
            free(persist_dirs.entries); 
            persist_dirs.entries = nullptr; 
        }
    }

    __st_msppt_t& operator =(const __st_msppt_t& v) = delete;
} msppt_t;

class msppt {
public:
    /**
     * @brief pasre an olefile for msppt_t
     *
     * @param ctx                   optional, the workflow context
     * @param olefile               
     * @param[in out] ppt
     * @return int
     */
    static int parse(const cfb_ctx_t* ctx, const olefile_t* olefile, msppt_t* ppt);

    /**
     * @brief decrypt a stream in ole in place
     *
     * @param ctx                   optional, the workflow context
     * @param ppt                   the parsed ppt information
     * @param passwd
     * @param name                  stream name
     * @param[in out] data			stream
     * @param size				    stream size
     * @return int
     */
    static int decrypt(const cfb_ctx_t* ctx, const msppt_t* ppt, const char16_t* passwd, const char16_t* name, uint8_t* data, uint32_t size);

    /**
     * @brief extract embedded objects 
     *
     * @param ctx                   optional, the workflow context
     * @param ppt                   the parsed ppt information
     * @param cb_file
     * @param passwd
     * @return int
     */
    static int extract(const cfb_ctx_t* ctx, const msppt_t* ppt, ifilehandler* cb_file, const char16_t* passwd);
};