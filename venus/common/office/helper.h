#pragma once
#include "./cfb_base.h"
#include "./ole.h"
#include "../file/filetype.h"
#include "../utils/utils.h"
#include "../utils/scope_guard.h"
#include "../utils/bitmap.h"
#include "../utils/codepage.h"

#include "zlib/zlib.h"
#if (_WIN32 || _WIN64)
    #if _DEBUG
        #pragma comment(lib, "zlibstaticd")
    #else 
        #pragma comment(lib, "zlibstatic")
    #endif
#elif (_MAC)

#endif

inline
embedded_object_t* create_embedded(const ctx_t* ctx, const file_obj_t* container, const uint8_t* data, uint32_t size, uint32_t ftype, const char* alias) {
    if (ctx == nullptr || ctx->file_creator == nullptr) {
        return nullptr;
    }

    ifile* newfile = ctx->file_creator(container, ctx->temp_dirpath, 0, alias, 0);
    if (nullptr == newfile) {
        return nullptr;
    }
    if (data && size > 0 && 0 > newfile->write(0, data, size)) {
        newfile->release();
        return nullptr;
    }

    auto out = new embedded_object_t();
    out->obj.container = container;
    out->obj.file = newfile;
    out->obj.filetype = (ftype == ft_unknown || ftype >= ft_ignore) ? FiletypeDetector().detect_mem_fast(data, size) : ftype;

    return out;
}

inline
const char* read_ansi_str(const uint8_t* data, uint32_t size) {
    uint32_t ending = 0;
    for (auto p = (const char*)data; ending < size && *(p + ending) != '\0'; ending++);
    if (ending >= size) {
        return nullptr;
    }
    return (const char*)data;
}