//
// Created by jiao on 2021/10/11.
//

#include "gtest/gtest.h"
#include "common/office/ms_xls.h"
#include "common/office/mhtml.h"
#include "./helper.h"

TEST(mht, structure) {
    std::string sample_path = std::string(dir_sample) + sample_doc_mht_activemine;

    auto ctx = new_cfb_ctx_for_testing(dir_temporary);
    ON_SCOPE_EXIT([&]() { free_cfb_ctx(ctx); });

    auto sample = openfile(sample_path.c_str(), "rb");
    ASSERT_NE(sample, nullptr);
    ON_SCOPE_EXIT([&]() { if (sample) sample->release(); });

    uint8_t buffer[512] = { 0 };
    uint32_t buffsize = std::min<uint32_t>(sizeof(buffer), sample->size());
    int nret = sample->read(0, buffer, buffsize, 0);
    EXPECT_EQ(nret >= BQ::OK, true);

    auto filetype = FiletypeDetector().detect_mem_fast(buffer, buffsize);
    ASSERT_EQ(filetype, ft_email);

    mht_t mht_obj;
    ExtractedHandler processor;
    nret = mhtml().extract(ctx, sample, nullptr, &mht_obj, &processor);
    ASSERT_EQ(nret, BQ::OK);

    auto subfiles = processor.children();
    EXPECT_EQ(subfiles.size(), 1);
    if (subfiles.size() > 0) {
        EXPECT_EQ(ucs::icmp(processor.md5(0).c_str(), "2de21140383512e25f9ca1feb0bdead6"), 0);
    }
}