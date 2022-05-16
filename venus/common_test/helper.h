//
// Created by jiao on 2021/10/11.
//

#ifndef COMMON_TEST_HELPER_H
#define COMMON_TEST_HELPER_H

#include "common/office/cfb_base.h"
#include "common/file/simplefile.h"
#include "common/office/ole.h"
#include "common/office/ole_utils.h"
#include "common/utils/scope_guard.h"
#include "common/scanner/simple_helper.h"


#define dir_sample "../common_test/sample/samples/"
#define dir_temporary "../common_test/sample/extracted/"

#define sample_xls_xlm              "b_xls_xlm"
#define sample_xls_encrypted_xlm    sample_crypt_xls_rc4capi
#define sample_xls_mbd              "w_xls_mbd__ole10native_package_rawole"
#define sample_doc_vba_objpool      "b_doc__vbaproject__objectpool_ole10native_package_doc__msodatastore"
#define sample_crypt_ecma_std       "b_ecma_encrypted_pwd_default__std"
#define sample_crypt_ecma_agile     "b_ecma_encrypted_pwd_default__agile"
#define sample_crypt_xls_xor        "w_xls_encrypted_pwd_default__xor"
#define sample_crypt_xls_rc4        "w_xls_encrypted_pwd_default__rc4"
#define sample_crypt_xls_rc4capi    "b_xls_encrypted_pwd_default__rc4capi_xlm"
#define sample_crypt_doc_xor        "w_doc_encrypted_pwd_default__xor"
#define sample_crypt_doc_rc4        "w_doc_encrypted_pwd_default__rc4"
#define sample_crypt_doc_rc4capi    "w_doc_encrypted_pwd_default__rc4capi"
#define sample_crypt_ppt_rc4capi    "w_ppt_encrypted_pwd_default__rc4capi"
#define sample_doc_mht_activemine   "b_mht_activemine_doc"
#define sample_docx_vba             "b_ooxml_vba_project"
#define sample_pcode_dir            "pcodes"

inline ifile* openfile(const char* path, const char* mode = "rb+") {
    SimpleFile* bqFile = SimpleFile::newfile();
    int nret = bqFile->open(path, nullptr, mode);
    if (nret != BQ::OK) {
        return nullptr;
    }
    return bqFile;
}

inline cfb_ctx_t* new_cfb_ctx_for_testing(const char* temp_dir) {
    auto ctx = new_cfb_ctx(temp_dir);
    ctx->extract_xlm_drawings = false;
    ctx->write_subfile_header = false;
    ctx->header_xlm_col = 0;
    ctx->header_vba_xlm = 0;
    return ctx;
}

template<class T>
T* extend_ole_to_msoffice(olefile_t** olefile) {
    if (nullptr == olefile || *olefile == nullptr) {
        return nullptr;
    }

    auto src = *olefile;

    auto dst = new T();
    dst->file = src->file;
    dst->filetype = src->filetype;
    dst->attr = src->attr;
    dst->container = src->container;

    auto ole_content_size = sizeof(olefile_t) - sizeof(file_obj_t);
    memcpy((void*)&(dst->header), (void*)&(src->header), ole_content_size);

    src->reset();
    delete src;
    *olefile = (olefile_t*)dst;

    return dst;
}

#endif //COMMON_TEST_HELPER_H
