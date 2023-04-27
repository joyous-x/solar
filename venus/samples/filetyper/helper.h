//
// Created by jiao on 2021/10/11.
//

#ifndef COMMON_TEST_HELPER_H
#define COMMON_TEST_HELPER_H

#include "common/file/simplefile.h"
#include "common/utils/scope_guard.h"
#include "common/office/errors.h"

inline ifile* openfile(const char* path, const char* mode = "rb+") {
    SimpleFile* bqFile = SimpleFile::newfile();
    int nret = bqFile->open(path, nullptr, mode);
    if (nret != BQ::OK) {
        return nullptr;
    }
    return bqFile;
}

#endif //COMMON_TEST_HELPER_H
