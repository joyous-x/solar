#pragma once
#include "ole.h"
#include "cfb_base.h"
#include "offcrypto_defines.h"

/*
*
* the Microsoft Office Excel 97 - 2007 Binary File Format (.xls) Specification:
*    : http://download.microsoft.com/download/5/0/1/501ED102-E53F-4CE0-AA6B-B0F93629DDC6/Office/Excel97-2007BinaryFileFormat(xls)Specification.pdf
*
*
* workbook stream 包含一系列 substreams. 每个 substream 以 BOF record (0x0809)开头，以 EOF record (0x000A)结尾.
*
*/

#pragma pack(push, 1)

typedef struct __st_ms_boundsheet_t {
    ifile*      file;
    char        name[64];
    uint32_t    lbPlyPos;
    uint16_t    flags;

    __st_ms_boundsheet_t() { memset((void*)this, 0, sizeof(__st_ms_boundsheet_t)); }
} xl_bin_boundsheet_t;

#pragma pack(pop)

typedef struct __st_ms_workbook_t {
    uint32_t            did{ 0 };
    uint16_t            biff_version{ 0 };
    uint16_t            codepage{ 0 };
    bool                has_macrosheet{ 0 };
    bool                has_drawing{ 0 };
    bool                encrypted{ 0 };
    bool                passwd_verified{ 0 };
    uint32_t            boundsheet_cnt{ 0 };
    xl_bin_boundsheet_t*    boundsheets{ 0 };
    bin_encryption_t    crypt;

    void release() {
        if (boundsheets && boundsheet_cnt) {
            for (uint32_t i = 0; i < boundsheet_cnt; i++) {
                auto bs = boundsheets + i;
                bs->file&& bs->file->release();
            }
            free(boundsheets);
            boundsheets = nullptr;
            boundsheet_cnt = 0;
        }
    }
} ms_workbook_t;

typedef struct __st_msxls_t : olefile_t {
    ms_workbook_t   workbook;

    virtual ~__st_msxls_t() { release(); }

    bool encrypted() const {
        return workbook.encrypted;
    }

    void release() {
        olefile_t::release();
        workbook.release();
    }

    __st_msxls_t& operator =(const __st_msxls_t& v) = delete;
} msxls_t;

class msxls {
public:
    /**
     * @brief pasre an olefile for msxls_t
     *
     * @param ctx                   optional, the workflow context
     * @param olefile
     * @param[in out] xls
     * @return int
     */
    static int parse(const cfb_ctx_t* ctx, const olefile_t* olefile, msxls_t* xls);

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
    static int decrypt(const cfb_ctx_t* ctx, const msxls_t* xls, const char16_t* passwd, const char16_t* name, uint8_t* data, uint32_t size);

    /**
     * @brief extract embedded objects
     *
     * @param ctx                   optional, the workflow context
     * @param [in out]xls           the parsed xls information. it will record some fields when extracting.
     * @param cb_file
     * @param passwd                optional
     * @return int
     */
    static int extract(const cfb_ctx_t* ctx, msxls_t* xls, ifilehandler* cb_file, const char16_t* passwd);

protected:
    static const bool is_little_endian{ true };
};