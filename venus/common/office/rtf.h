#pragma once
#include "../office/cfb_base.h"

class rtf {
public:
    /**
     * @brief extract embedded objects
     *      rtfs contain no macros and methods in here will try to extract the embedded objects
     *
     * @param ctx                   optional, the workflow context
     * @param rtf
     * @param cb_file
     * @return int
     */
    int extract(cfb_ctx_t* ctx, const file_obj_t* rtf, ifilehandler* cb_file);
};