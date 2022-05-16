//
// Created by jiao on 2021/10/11.
//

#include "gtest/gtest.h"
#include "common/office/ms_xls.h"
#include "./helper.h"

extern int parse_as_ole(cfb_ctx_t* ctx, const char* filepath, olefile_t** out);

int parse_as_xls(cfb_ctx_t* ctx, const char* filepath, msxls_t** out) {
    auto sample = openfile(filepath, "rb");
    if (sample == nullptr) {
        return BQ::ERR;
    }
    ON_SCOPE_EXIT([&]() { if (sample) sample->release(); });

    olefile_t* olefile = new olefile_t();
    ON_SCOPE_EXIT([&]() { if (olefile) delete olefile; });
    olefile->file = sample;
    olefile->file->addref();

    int nret = ole::parse(ctx, sample, olefile);
    if (nret != BQ::OK) {
        return nret;
    }

    auto xls = extend_ole_to_msoffice<msxls_t>(&olefile);
    nret = msxls::parse(ctx, (const olefile_t*)xls, xls);
    if (nret != BQ::OK) {
        return nret;
    }
    else {
        olefile = nullptr;
        *out = xls;
    }

    return BQ::OK;
}

TEST(xls, xlm_structure) {
    std::string sample_path = std::string(dir_sample) + sample_xls_xlm;

    msxls_t* xls = nullptr;
    ON_SCOPE_EXIT([&]() { if (xls) delete xls; });

    auto ctx = new_cfb_ctx_for_testing(dir_temporary);
    ON_SCOPE_EXIT([&]() { free_cfb_ctx(ctx); });
    int nret = parse_as_xls(ctx, sample_path.c_str(), &xls);
    ASSERT_EQ(nret, BQ::OK);

    ExtractedHandler processor;
    nret = msxls::extract(ctx, xls, &processor, 0);
    EXPECT_EQ(nret, BQ::OK);

    auto subfiles = processor.children();
    EXPECT_EQ(subfiles.size(), 1);
    if (subfiles.size() > 0) {
        EXPECT_EQ(ucs::icmp(processor.md5(0).c_str(), "d24543bdb0f9be29b812ddeb8a67ef54"), 0);
    }
}

TEST(xls, mbd) {
    std::string sample_path = std::string(dir_sample) + sample_xls_mbd;

    msxls_t* xls = nullptr;
    ON_SCOPE_EXIT([&]() { if (xls) delete xls; });

    auto ctx = new_cfb_ctx_for_testing(dir_temporary);
    ON_SCOPE_EXIT([&]() { free_cfb_ctx(ctx); });
    int nret = parse_as_xls(ctx, sample_path.c_str(), &xls);
    ASSERT_EQ(nret, BQ::OK);

    ExtractedHandler processor;
    nret = msxls::extract(ctx, xls, &processor, 0);
    EXPECT_EQ(nret, BQ::OK);

    auto subfiles = processor.children();
    EXPECT_EQ(subfiles.size(), 2);
    if (subfiles.size() > 1) {
        std::string md5 = processor.md5(0);
        /* w_xlsx_macrosheet */
        EXPECT_EQ(ucs::icmp(md5.c_str(), "0a27a8741be4b7cf83294af00bb58ae7"), 0);
        md5 = processor.md5(1);
        /* test.doc.xml */
        EXPECT_EQ(ucs::icmp(md5.c_str(), "290c5558a28beb965a1a5831253318f3"), 0);
    }
}

TEST(xls, encrypted_xlm) {
    std::string sample_path = std::string(dir_sample) + sample_xls_encrypted_xlm;

    msxls_t* xls = nullptr;
    ON_SCOPE_EXIT([&]() { if (xls) delete xls; });

    auto ctx = new_cfb_ctx_for_testing(dir_temporary);
    ON_SCOPE_EXIT([&]() { free_cfb_ctx(ctx); });
    int nret = parse_as_xls(ctx, sample_path.c_str(), &xls);
    ASSERT_EQ(nret, BQ::OK);

    EXPECT_EQ(xls->encrypted(), true);

    ExtractedHandler processor;
    nret = msxls::extract(ctx, xls, &processor, 0);
    EXPECT_EQ(nret, BQ::OK);

    auto subfiles = processor.children();
    EXPECT_EQ(subfiles.size(), 1);
    if (subfiles.size() > 0) {
        /* SOCWNEScLLxkLhtJp */
        std::string md5 = processor.md5(0);
        EXPECT_EQ(ucs::icmp(md5.c_str(), "313141119c42a82100f74e0771736216"), 0);
    }
}
