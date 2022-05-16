//
// Created by jiao on 2021/10/11.
//

#include "gtest/gtest.h"
#include "common/office/ms_xls.h"
#include "common/office/mhtml.h"
#include "common/scanner/scanner.h"
#include "./helper.h"

int scan(cfb_ctx_t* ctx, ifile* file, ExtractedHandler* processor) {
    if (ctx == nullptr || file == nullptr) {
        return BQ::INVALID_ARG;
    }

    file_obj_t* root_object = nullptr;
    ON_SCOPE_EXIT([&]() { if (root_object) delete root_object; });

    int nret = offscanner().extract(ctx, file, nullptr, &root_object, processor, nullptr);
    if (nret != BQ::OK) {
        return nret;
    }

    for (uint32_t i = 0, depeth = 1, size = processor->children().size(); i <= size; i++) {
        if (i == size) {
            if (size < processor->children().size()) {
                size = processor->children().size();
                depeth++;
            }
            else {
                break;
            }
        }

        extracted_file_t* curobj = processor->children()[i];
        nret = offscanner().extract(ctx, curobj->embedded.obj.file, curobj->embedded.obj.container, &curobj->realobj, processor, nullptr);
        if (nret != BQ::OK) {
            break;
        }
    }

    return nret;
}

TEST(sample, scanner) {
    std::string sample_path = std::string(dir_sample) + sample_doc_mht_activemine;

    auto ctx = new_cfb_ctx_for_testing(dir_temporary);
    ON_SCOPE_EXIT([&]() { free_cfb_ctx(ctx); });

    auto sample = openfile(sample_path.c_str(), "rb");
    ASSERT_NE(sample, nullptr);
    ON_SCOPE_EXIT([&]() { if (sample) sample->release(); });

    ExtractedHandler processor;
    int nret = scan(ctx, sample, &processor);
    ASSERT_NE(nret, BQ::OK);

    EXPECT_EQ(processor.children().size(), 3);
    for (size_t i = 0; i < processor.children().size(); i++) {
        std::string md5 = processor.md5(i);
        if (i == 0) EXPECT_EQ(ucs::icmp(md5.c_str(), "2de21140383512e25f9ca1feb0bdead6"), 0);
        if (i == 1) EXPECT_EQ(ucs::icmp(md5.c_str(), "6984bf51ff0d853ca0bf1e9a1923ffb1"), 0);
        if (i == 2) EXPECT_EQ(ucs::icmp(md5.c_str(), "c2145c512596bbe98d04de579447ee7a"), 0);
    }
}