//
// Created by jiao on 2021/10/11.
//

#include "gtest/gtest.h"
#include "common/office/ms_doc.h"
#include "./helper.h"

int parse_as_doc(cfb_ctx_t* ctx, const char* filepath, msdoc_t** out) {
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

    auto doc = extend_ole_to_msoffice<msdoc_t>(&olefile);
    nret = msdoc::parse(ctx, (const olefile_t*)doc, doc);
    if (nret != BQ::OK) {
        return nret;
    }
    else {
        olefile = nullptr;
        *out = doc;
    }

    return BQ::OK;
}

TEST(doc, objectpool) {
    std::string sample_path = std::string(dir_sample) + sample_doc_vba_objpool;
    
    msdoc_t* doc = nullptr;
    ON_SCOPE_EXIT([&]() { if (doc) delete doc; });

    auto ctx = new_cfb_ctx_for_testing(dir_temporary);
    ON_SCOPE_EXIT([&]() { free_cfb_ctx(ctx); });
    int nret = parse_as_doc(ctx, sample_path.c_str(), &doc);
    EXPECT_EQ(nret, BQ::OK);

    ctx->extract_objectpool = true;
    ExtractedHandler processor;
    nret = msdoc::extract(ctx, doc, &processor, 0);
    EXPECT_EQ(nret, BQ::OK);

    auto subfiles = processor.children();
    EXPECT_EQ(subfiles.size(), 5);
    if (subfiles.size() >= 5) {
        /* _1557814583_全报表.rar */
        std::string md5 = processor.md5(0);
        EXPECT_EQ(ucs::icmp(md5.c_str(), "35f9bb764dcce094a1a3d480186476f4"), 0);
        /* _1557815063_统文件.rar */
        md5 = processor.md5(1);
        EXPECT_EQ(ucs::icmp(md5.c_str(), "40a683b5796655b483631ccdeb56a987"), 0);
        /* _1557815596_Microsoft Word 文档 */
        md5 = processor.md5(2);
        EXPECT_EQ(ucs::icmp(md5.c_str(), "e9f15938f4b7c847799a8ee66b15f535"), 0);
        /* _1557814667_Microsoft Word 文档 */
        md5 = processor.md5(3);
        EXPECT_EQ(ucs::icmp(md5.c_str(), "2bc17f40d9b5109e8760e0f13fd844ce"), 0);
        /* _1557814413_unknown */
        md5 = processor.md5(4);
        EXPECT_EQ(ucs::icmp(md5.c_str(), "c429f59cc1f66c891bb721095e3b49cd"), 0);
    }
}

