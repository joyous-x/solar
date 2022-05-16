#pragma once
#include "./cfb_base.h"
#include "../file/struct_ms_lnk.h"

typedef struct __st_ms_shellink : file_obj_t {
    ms_shellink_header_t header;
    link_targetid_list_t targetids;

    __st_ms_shellink() {  }
    virtual ~__st_ms_shellink() {  }
} ms_shellink_t;

class mshellink {
public:
    /**
     * @brief parse ms shell link
     *
     * @param ctx                   the workflow context
     * @param file
     * @param[in out] lnk
     * @return int
     */
    static int parse(const ctx_t* ctx, ifile* file, ms_shellink_t* lnk);

protected:
    static const bool is_little_endian{ true };
};