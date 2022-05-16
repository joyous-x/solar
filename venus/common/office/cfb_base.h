#ifndef __CFB_BASE_H__
#define __CFB_BASE_H__

#include <assert.h>
#include "./errors.h"
#include "../file/ifile.h"
#include "../file/filetype_define.h"
#include "../file/struct_ole.h"

class iloger {
public:
    virtual void debug(const char* format, ...) = 0;
    virtual void warn(const char* format, ...) = 0;
    virtual void fatal(const char* format, ...) = 0;
};

typedef struct __st_ctx_base {
    iloger* loger{ nullptr };
    uint32_t cutoff_write_tmp{ 0 }; /* if file size is greater than or equal to cutoff_write_tmp, it should be saved to disk */
    const char* temp_dirpath{ nullptr };
    file_creator_t  file_creator{ nullptr };
    void* caller{ nullptr }; /* caller's data, MUST NOT be changed in workflow */
} ctx_t;

class ifilehandler {
public:
    /*
    * @description:
    *
    * @param ctx
    * @param fileobj    the implemention should not keep this pointer, because the caller created it and decides when to release.
    * @param status     reserved
    * @return           if return code is less than 0, the workflow will be interrupted.
    */
    virtual int handle(const ctx_t* ctx, const file_obj_t* fileobj, void* reserved = 0) = 0;
};

class iziper : public izip {
public:
    virtual int parse(ifile* in, const char* mode = "rb") = 0;
};

#define DEFAULT_HEADER_XLM_COL "'==="
#define DEFAULT_HEADER_XLM_VBA "'===CF.VBA.XLM==="

typedef struct __st_embedded_object {
    file_obj_t      obj;
    uint32_t        did{ 0 };
    uint32_t        offset{ 0 };
    uint32_t        reserved{ 0 };

    __st_embedded_object() {};
    __st_embedded_object(const __st_embedded_object& v)
        : obj(v.obj), did(v.did), offset(v.offset) {}
    __st_embedded_object(const file_obj_t& v, uint32_t d, uint32_t o)
        : obj(v), did(d), offset(o) {}
} embedded_object_t;

/**
*
* @name __st_cfb_ctx
* @desc	context for the ole extractor
*/
typedef struct __st_cfb_ctx : ctx_t {
    bool try_decrypt{ false };
    bool try_depcode{ false };
    bool extract_mbd{ false };
    bool extract_xlm{ true };
    bool extract_xlm_drawings{ false }; /* TODO */
    bool extract_vba_orphan{ false };
    bool xlm_deobfuscation{ true };
    bool write_subfile_header{ false };
    const char* header_vba_xlm{ nullptr };
    const char* header_xlm_col{ nullptr };
    bool extract_objectpool{ false };
    bool extract_ppt_oleobject{ false };
    uint32_t max_stream_size{ 0 };
    uint32_t max_file_size{ 0 };

    __st_cfb_ctx() {
        cutoff_write_tmp = 2 * 1024 * 1024;
        max_file_size = 20 * 1024 * 1024;
        write_subfile_header = true;
        header_vba_xlm = DEFAULT_HEADER_XLM_VBA;
        header_xlm_col = DEFAULT_HEADER_XLM_COL;
    }
} cfb_ctx_t;


#endif // __CFB_BASE_H__