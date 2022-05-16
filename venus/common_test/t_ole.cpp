//
// Created by jiao on 2021/10/11.
//

#include "gtest/gtest.h"
#include "common/office/ms_xls.h"
#include "common/office/ms_vba.h"
#include "./helper.h"


int parse_as_ole(cfb_ctx_t* ctx, const char* filepath, olefile_t** out) {
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
    else {
        *out = olefile;
        olefile = nullptr;
    }

    return BQ::OK;
}


TEST(Ole, structure) {
    std::string sample_path = std::string(dir_sample) + sample_xls_xlm;

    olefile_t* olefile = nullptr;
    ON_SCOPE_EXIT([&]() { if (olefile) delete olefile; });

    auto ctx = new_cfb_ctx_for_testing(dir_temporary);
    ON_SCOPE_EXIT([&]() { free_cfb_ctx(ctx); });
    int nret = parse_as_ole(ctx, sample_path.c_str(), &olefile);
    ASSERT_EQ(nret, BQ::OK);
    EXPECT_NE(olefile, nullptr);

    if (olefile) {
        EXPECT_EQ(olefile->entry_count, 4);
        EXPECT_EQ(olefile->ssector_entry_sid, 0x14C);
        EXPECT_EQ(olefile->msat_table_size, 3);
        EXPECT_EQ(olefile->sat_table_size, 0x180);
        EXPECT_EQ(olefile->ssat_table_size, 0x100);
        EXPECT_EQ(olefile->header.byte_order, 0xFFFE); /* 字节序标识: FE FF = Little-Endian  FF FE = Big-Endian */
        EXPECT_EQ(olefile->header.minor_version, 0x3E);
        EXPECT_EQ(olefile->header.major_version, 3);
        EXPECT_EQ(olefile->header.sector_shift, 9);
        EXPECT_EQ(olefile->header.short_sector_shift, 6);
        EXPECT_EQ(olefile->header.directory_sector_count, 0);
        EXPECT_EQ(olefile->header.sat_sector_count, 3);
        EXPECT_EQ(olefile->header.directory_entry_sid, 0x14A);
        EXPECT_EQ(olefile->header.min_stream_cutoff, 4096);
        EXPECT_EQ(olefile->header.ssat_entry_sid, 0x14B);
        EXPECT_EQ(olefile->header.ssat_sector_count, 1);
        EXPECT_EQ(olefile->header.msat_entry_sid, 0xFFFFFFFE);
        EXPECT_EQ(olefile->header.msat_sector_count, 0);
    }
}

TEST(ole, vba_project) {
    std::string sample_path = std::string(dir_sample) + sample_doc_vba_objpool;

    olefile_t* olefile = nullptr;
    ON_SCOPE_EXIT([&]() { if (olefile) delete olefile; });

    auto ctx = new_cfb_ctx_for_testing(dir_temporary);
    ON_SCOPE_EXIT([&]() { free_cfb_ctx(ctx); });
    int nret = parse_as_ole(ctx, sample_path.c_str(), &olefile);
    ASSERT_EQ(nret, BQ::OK);
    EXPECT_NE(olefile, nullptr);

    auto filetype = ole::filetype(ctx, olefile);
    ASSERT_EQ(filetype >= ft_ole && filetype <= ft_ooxml_encrypted, true);

    ExtractedHandler processor;
    msvba().extract_vba_project(ctx, olefile, &processor);

    auto subfiles = processor.children();
    EXPECT_EQ(subfiles.size(), 1);
    if (subfiles.size() > 0) {
        /* ThisDocument */
        std::string md5 = processor.md5(0);
        EXPECT_EQ(ucs::icmp(md5.c_str(), "6a858a161684c897105157464d63bc72"), 0);
    }
}