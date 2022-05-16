//
// Created by jiao on 2021/10/11.
//

#include "gtest/gtest.h"
#include "common/office/ms_xls.h"
#include "common/office/ms_doc.h"
#include "common/office/ms_ppt.h"
#include "./helper.h"

extern int parse_as_ole(cfb_ctx_t* ctx, const char* filepath, olefile_t** out);
extern int parse_as_xls(cfb_ctx_t* ctx, const char* filepath, msxls_t** out);
extern int parse_as_doc(cfb_ctx_t* ctx, const char* filepath, msdoc_t** out);
extern int parse_as_ppt(cfb_ctx_t* ctx, const char* filepath, msppt_t** out);

int dig_workbook_crypto_md5(const char* sample_path, const char* dir_temp, msxls_t** out, std::string* workbook_md5) {
    msxls_t* xls = nullptr;
    ON_SCOPE_EXIT([&]() { if (xls) delete xls; });

    auto ctx = new_cfb_ctx_for_testing(dir_temp);
    ON_SCOPE_EXIT([&]() { free_cfb_ctx(ctx); });
    int nret = parse_as_xls(ctx, sample_path, &xls);
    if (nret != BQ::OK) {
        return nret;
    }

    uint8_t* data = 0;
    uint32_t size = 0;
    ON_SCOPE_EXIT([&] { if (data) free(data); });

    nret = ole::read_stream(xls, xls->workbook.did, -1, &data, &size);
    if (nret != BQ::OK) {
        return nret;
    }

    if (xls->encrypted()) {
        auto cur_did = xls->workbook.did;
        auto cur_sname = xls->entries ? (xls->entries + xls->workbook.did)->name : u"";
        nret = msxls::decrypt(ctx, xls, nullptr, cur_sname, data, size);
        if (nret != BQ::OK) {
            return nret;
        }
    }    

    md5::MD5 hash_calc;
    hash_calc.Update((unsigned char*)data, size);
    hash_calc.Final();

    *workbook_md5 = hash_calc.digestChars;
    *out = xls;
    xls = nullptr;

    return BQ::OK;
}

int dig_wordocument_crypto_md5(const char* sample_path, const char* dir_temp, msdoc_t** out, std::string* md5) {
    msdoc_t* doc = nullptr;
    ON_SCOPE_EXIT([&]() { if (doc) delete doc; });

    auto ctx = new_cfb_ctx_for_testing(dir_temp);
    ON_SCOPE_EXIT([&]() { free_cfb_ctx(ctx); });
    int nret = parse_as_doc(ctx, sample_path, &doc);
    if (nret != BQ::OK) {
        return nret;
    }

    uint8_t* data = 0;
    uint32_t size = 0;
    ON_SCOPE_EXIT([&] { if (data) free(data); });

    nret = ole::read_stream(doc, doc->did_wordocument, -1, &data, &size);
    if (nret != BQ::OK) {
        return nret;
    }

    if (doc->crypt.type != encryinfo_unknown) {
        auto cur_did = doc->did_wordocument;
        auto cur_sname = doc->entries ? (doc->entries + doc->did_wordocument)->name : u"";
        nret = msdoc::decrypt(ctx, doc, u"Hannes Ruescher", cur_sname, data, size);
        if (nret != BQ::OK) {
            return nret;
        }
    }

    md5::MD5 hash_calc;
    hash_calc.Update((unsigned char*)data, size);
    hash_calc.Final();

    *md5 = hash_calc.digestChars;
    *out = doc;
    doc = nullptr;    

    return BQ::OK;
}

int dig_powerpoint_crypto_md5(const char* sample_path, const char* dir_temp, msppt_t** out, std::string* md5) {
    msppt_t* ppt = nullptr;
    ON_SCOPE_EXIT([&]() { if (ppt) delete ppt; });

    auto ctx = new_cfb_ctx_for_testing(dir_temp);
    ON_SCOPE_EXIT([&]() { free_cfb_ctx(ctx); });
    int nret = parse_as_ppt(ctx, sample_path, &ppt);
    if (nret != BQ::OK) {
        return nret;
    }

    uint8_t* data = 0;
    uint32_t size = 0;
    ON_SCOPE_EXIT([&] { if (data) free(data); });

    nret = ole::read_stream(ppt, ppt->did_document, -1, &data, &size);
    if (nret != BQ::OK) {
        return nret;
    }

    if (ppt->encrypted()) {
        auto cur_did = ppt->did_document;
        auto cur_sname = ppt->entries ? (ppt->entries + ppt->did_document)->name : u"";
        nret = msppt::decrypt(ctx, ppt, u"Hannes Ruescher", cur_sname, data, size);
        if (nret != BQ::OK) {
            return nret;
        }
    }

    md5::MD5 hash_calc;
    hash_calc.Update((unsigned char*)data, size);
    hash_calc.Final();

    *md5 = hash_calc.digestChars;
    *out = ppt;
    ppt = nullptr;

    return BQ::OK;
}

TEST(crypt, xls_xor) {
    std::string sample_path = std::string(dir_sample) + sample_crypt_xls_xor;

    msxls_t* xls = nullptr;
    ON_SCOPE_EXIT([&]() { if (xls) delete xls; });

    std::string workbook_md5;
    int nret = dig_workbook_crypto_md5(sample_path.c_str(), dir_temporary, &xls, &workbook_md5);
    ASSERT_EQ(nret, BQ::OK);
    
    EXPECT_EQ(xls->encrypted(), true);
    EXPECT_EQ(xls->workbook.crypt.type, encryinfo_bin_xor_m1);
    EXPECT_EQ(ucs::icmp(workbook_md5.c_str(), "ff236313f2e342a447ab0108f4883418"), 0);
}

TEST(crypt, xls_rc4) {
    std::string sample_path = std::string(dir_sample) + sample_crypt_xls_rc4;

    msxls_t* xls = nullptr;
    ON_SCOPE_EXIT([&]() { if (xls) delete xls; });

    std::string workbook_md5;
    int nret = dig_workbook_crypto_md5(sample_path.c_str(), dir_temporary, &xls, &workbook_md5);
    ASSERT_EQ(nret, BQ::OK);

    EXPECT_EQ(xls->encrypted(), true);
    EXPECT_EQ(xls->workbook.crypt.type, encryinfo_bin_rc4);
    EXPECT_EQ(ucs::icmp(workbook_md5.c_str(), "5fbadf32c21d9025c07152c0290f4fa4"), 0);
}

TEST(crypt, xls_rc4capi) {
    std::string sample_path = std::string(dir_sample) + sample_crypt_xls_rc4capi;

    msxls_t* xls = nullptr;
    ON_SCOPE_EXIT([&]() { if (xls) delete xls; });

    std::string workbook_md5;
    int nret = dig_workbook_crypto_md5(sample_path.c_str(), dir_temporary, &xls, &workbook_md5);
    ASSERT_EQ(nret, BQ::OK);

    EXPECT_EQ(xls->encrypted(), true);
    EXPECT_EQ(xls->workbook.crypt.type, encryinfo_bin_rc4_capi);
    EXPECT_EQ(ucs::icmp(workbook_md5.c_str(), "3571cf52728f7471387384c33959fe07"), 0);
}

TEST(crypt, doc_xor) {
    std::string sample_path = std::string(dir_sample) + sample_crypt_doc_xor;

    msdoc_t* doc = nullptr;
    ON_SCOPE_EXIT([&]() { if (doc) delete doc; });

    std::string wordocument_md5;
    int nret = dig_wordocument_crypto_md5(sample_path.c_str(), dir_temporary, &doc, &wordocument_md5);
    ASSERT_EQ(nret, BQ::OK);

    EXPECT_EQ(doc->crypt.type, encryinfo_bin_xor_m2);
    EXPECT_EQ(ucs::icmp(wordocument_md5.c_str(), "4c21742458d7f684b88220be4c80084a"), 0);
}

TEST(crypt, doc_rc4) {
    std::string sample_path = std::string(dir_sample) + sample_crypt_doc_rc4;

    msdoc_t* doc = nullptr;
    ON_SCOPE_EXIT([&]() { if (doc) delete doc; });

    std::string wordocument_md5;
    int nret = dig_wordocument_crypto_md5(sample_path.c_str(), dir_temporary, &doc, &wordocument_md5);
    ASSERT_EQ(nret, BQ::OK);

    EXPECT_EQ(doc->crypt.type, encryinfo_bin_rc4);
    EXPECT_EQ(ucs::icmp(wordocument_md5.c_str(), "02845030c0d5739c9adcfe1f0cd3bcf5"), 0);
}

TEST(crypt, doc_rc4capi) {
    std::string sample_path = std::string(dir_sample) + sample_crypt_doc_rc4capi;

    msdoc_t* doc = nullptr;
    ON_SCOPE_EXIT([&]() { if (doc) delete doc; });

    std::string wordocument_md5;
    int nret = dig_wordocument_crypto_md5(sample_path.c_str(), dir_temporary, &doc, &wordocument_md5);
    ASSERT_EQ(nret, BQ::OK);

    EXPECT_EQ(doc->crypt.type, encryinfo_bin_rc4_capi);
    EXPECT_EQ(ucs::icmp(wordocument_md5.c_str(), "4a6925e8d58cc2a32226d6f4220bc486"), 0);
}

TEST(crypt, ppt_rc4capi) {
    std::string sample_path = std::string(dir_sample) + sample_crypt_ppt_rc4capi;

    msppt_t* ppt = nullptr;
    ON_SCOPE_EXIT([&]() { if (ppt) delete ppt; });

    std::string powerpoint_md5;
    int nret = dig_powerpoint_crypto_md5(sample_path.c_str(), dir_temporary, &ppt, &powerpoint_md5);
    ASSERT_EQ(nret, BQ::OK);

    EXPECT_EQ(ppt->encrypted(), true);
    EXPECT_EQ(ucs::icmp(powerpoint_md5.c_str(), "df55dcb07dfe9990253dece55cadc03d"), 0);
}

TEST(crypt, ecma_std) {
    olefile_t* ole = nullptr;
    std::string sample_path = std::string(dir_sample) + sample_crypt_ecma_std;

    auto ctx = new_cfb_ctx_for_testing(dir_temporary);
    ON_SCOPE_EXIT([&]() { free_cfb_ctx(ctx); });
    int nret = parse_as_ole(ctx, sample_path.c_str(), &ole);
    ASSERT_EQ(nret, BQ::OK);

    auto filetype = ole::filetype(ctx, ole);
    EXPECT_EQ(filetype, ft_ooxml_encrypted);

    ifile* extracted = nullptr;
    nret = Ooxml().decrypt_from_ole(ctx, ole, nullptr, nullptr, &extracted);
    ASSERT_EQ(nret, BQ::OK);

    md5::MD5 hash_calc;
    hash_calc.Update((unsigned char*)extracted->mapping(), extracted->size());
    hash_calc.Final();
    EXPECT_EQ(ucs::icmp(hash_calc.digestChars, "63efc3ba8f2338146634f3044f66ab5b"), 0);
}

TEST(crypt, ecma_agile) {
    olefile_t* ole = nullptr;
    std::string sample_path = std::string(dir_sample) + sample_crypt_ecma_agile;

    auto ctx = new_cfb_ctx_for_testing(dir_temporary);
    ON_SCOPE_EXIT([&]() { free_cfb_ctx(ctx); });
    int nret = parse_as_ole(ctx, sample_path.c_str(), &ole);
    ASSERT_EQ(nret, BQ::OK);

    auto filetype = ole::filetype(ctx, ole);
    EXPECT_EQ(filetype, ft_ooxml_encrypted);

    ifile* extracted = nullptr;
    nret = Ooxml().decrypt_from_ole(ctx, ole, nullptr, nullptr, &extracted);
    ASSERT_EQ(nret, BQ::OK);

    md5::MD5 hash_calc;
    hash_calc.Update((unsigned char*)extracted->mapping(), extracted->size());
    hash_calc.Final();
    EXPECT_EQ(ucs::icmp(hash_calc.digestChars, "4a5f793e6ecbffde86461a7c1cf49677"), 0);
}