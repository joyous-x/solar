//
// Created by jiao on 2021/10/11.
//

#include "gtest/gtest.h"
#include "common/office/ms_ppt.h"
#include "./helper.h"

int parse_as_ppt(cfb_ctx_t* ctx, const char* filepath, msppt_t** out) {
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

    auto ppt = extend_ole_to_msoffice<msppt_t>(&olefile);
    nret = msppt::parse(ctx, (const olefile_t*)ppt, ppt);
    if (nret != BQ::OK) {
        return nret;
    }
    else {
        olefile = nullptr;
        *out = ppt;
    }

    return BQ::OK;
}

TEST(ppt, todo) {
    
}

