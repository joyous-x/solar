//
// Created by jiao on 2021/10/11.
//

#include "gtest/gtest.h"
#include "./helper.h"
#include "common/office/rle.h"
#include "common/office/helper.h"
#include "common/office/ooxml.h"
#include "common/office/ms_vba.h"
#include "common/scanner/simple_helper.h"

int parse_as_ooxml(cfb_ctx_t* ctx, const char* filepath, iziper* ziper, ooxml_file_t** out) {
    ifile* sample = openfile(filepath, "rb");
    ON_SCOPE_EXIT([&]() { if (sample) sample->release(); });
    return offscanner().extract(ctx, sample, 0, (file_obj_t**)out, nullptr, ziper);
}

TEST(rle, uncompress) {
    std::string sample_path = std::string(dir_sample) + sample_docx_vba;

    auto ctx = new_cfb_ctx_for_testing(dir_temporary);
    ON_SCOPE_EXIT([&]() { free_cfb_ctx(ctx); });
    ooxml_file_t* ooxml = nullptr;
    ON_SCOPE_EXIT([&]() { if(ooxml) delete ooxml; });

    ZipHelper zip_helper;
    int nret = parse_as_ooxml(ctx, sample_path.c_str(), &zip_helper, &ooxml);
    ASSERT_EQ(nret, BQ::OK);
    EXPECT_EQ(ooxml->vbaproject_id >= 1, true);

    uint8_t* data = nullptr;
    uint32_t size = 0;
    ON_SCOPE_EXIT([&]() { if (data) free(data); });
    nret = ooxml->unzip->get_item(ooxml->vbaproject_id - 1, &data, &size);
    EXPECT_EQ(nret, BQ::OK);

    auto embedded = create_embedded(ctx, ooxml, data, size, ft_ole, ooxml->zipitem_path(ooxml->vbaproject_id - 1));
    EXPECT_NE(embedded, nullptr);

    olefile_t vbaproject;
    nret = ole::parse(ctx, embedded->obj.file, &vbaproject);
    EXPECT_EQ(nret, BQ::OK);

    ExtractedHandler processor;
    nret = msvba().extract_vba_project(ctx, &vbaproject, &processor);
    EXPECT_EQ(nret, BQ::OK);

    bool find_target_stream = false;
    const char* target_stream = "ThisDocument";
    for (uint32_t i = 0; i < processor.children().size(); i++) {
        if (0 != ucs::icmp(processor.children()[i]->embedded.obj.file->alias(), target_stream)) {
            continue;
        }
        find_target_stream = true;
        std::string md5 = processor.md5(i);
        EXPECT_EQ(ucs::icmp(md5.c_str(), "5cfc1947eb814065025ef223252cad5f"), 0);
    }
    ASSERT_EQ(find_target_stream, true);
}

ifile* parse_vba_from_ooxml(const char* filepath, const char* stream_name = "ThisDocument") {
    auto ctx = new_cfb_ctx_for_testing(dir_temporary);
    ON_SCOPE_EXIT([&]() { free_cfb_ctx(ctx); });
    ooxml_file_t* ooxml = nullptr;
    ON_SCOPE_EXIT([&]() { if (ooxml) delete ooxml; });

    ZipHelper zip_helper;
    int nret = parse_as_ooxml(ctx, filepath, &zip_helper, &ooxml);
    EXPECT_EQ(nret, BQ::OK);
    EXPECT_EQ(ooxml->vbaproject_id >= 1, true);

    uint8_t* data = nullptr;
    uint32_t size = 0;
    ON_SCOPE_EXIT([&]() { if (data) free(data); });
    nret = ooxml->unzip->get_item(ooxml->vbaproject_id - 1, &data, &size);
    EXPECT_EQ(nret, BQ::OK);

    auto embedded = create_embedded(ctx, ooxml, data, size, ft_ole, ooxml->zipitem_path(ooxml->vbaproject_id - 1));
    EXPECT_NE(embedded, nullptr);

    olefile_t vbaproject;
    nret = ole::parse(ctx, embedded->obj.file, &vbaproject);
    EXPECT_EQ(nret, BQ::OK);

    ExtractedHandler processor;
    nret = msvba().extract_vba_project(ctx, &vbaproject, &processor);
    EXPECT_EQ(nret, BQ::OK);

    ifile* target = nullptr;
    const char* target_stream = "ThisDocument";
    for (uint32_t i = 0; i < processor.children().size(); i++) {
        if (0 != ucs::icmp(processor.children()[i]->embedded.obj.file->alias(), target_stream)) {
            continue;
        }
        target = processor.children()[i]->embedded.obj.file;
        target->addref();
    }
    return target;
}

TEST(rle, compress) {
    std::string sample_path = std::string(dir_sample) + sample_docx_vba;

    ifile* doc = parse_vba_from_ooxml(sample_path.c_str(), "ThisDocument");
    ASSERT_NE(doc, nullptr);
    ON_SCOPE_EXIT([&]() { if (doc) doc->release(); });

    const uint8_t* data = doc->mapping();
    uint32_t size = doc->size();

    uint8_t* packed = nullptr;
    uint32_t packed_size = 0;
    ON_SCOPE_EXIT([&]() { if (packed) free(packed); });
    int nret = RleCoding().compress(data, size, &packed, &packed_size);
    ASSERT_EQ(nret, 0);

    uint8_t* unpacked = nullptr;
    uint32_t unpacked_size = 0;
    ON_SCOPE_EXIT([&]() { if (unpacked) free(unpacked); });
    nret = RleCoding().decompress(packed, packed_size, &unpacked, &unpacked_size);
    ASSERT_EQ(nret, 0);

    md5::MD5 hash_calc;
    hash_calc.Update((unsigned char*)data, size);
    hash_calc.Final();

    md5::MD5 hash_calc_ex;
    hash_calc_ex.Update((unsigned char*)unpacked, unpacked_size);
    hash_calc_ex.Final();

    ASSERT_EQ(strcmp(hash_calc.digestChars, hash_calc_ex.digestChars), 0);
}

TEST(rle, reset_compressed_inplace) {
    std::string sample_path = std::string(dir_sample) + sample_docx_vba;

    ifile* doc = parse_vba_from_ooxml(sample_path.c_str(), "ThisDocument");
    ASSERT_NE(doc, nullptr);
    ON_SCOPE_EXIT([&]() { if (doc) doc->release(); });

    const uint8_t* data = doc->mapping();
    uint32_t size = doc->size();

    uint8_t* packed = nullptr;
    uint32_t packed_size = 0;
    ON_SCOPE_EXIT([&]() { if (packed) free(packed); });
    int nret = RleCoding().compress(data, size, &packed, &packed_size);
    ASSERT_EQ(nret, 0);
    nret = RleCoding().reset_compressed_inplace(packed, packed_size);
    ASSERT_EQ(nret, 0);

    uint8_t* unpacked = nullptr;
    uint32_t unpacked_size = 0;
    ON_SCOPE_EXIT([&]() { if (unpacked) free(unpacked); });
    nret = RleCoding().decompress(packed, packed_size, &unpacked, &unpacked_size);
    ASSERT_EQ(nret, 0);

    ASSERT_EQ(unpacked_size, size);
}
