#pragma once
#include "ifile.h"
#include "filetype_define.h"

namespace common_ftype {
    /*
    * @desc detect the filetype fastly
    *
    * [return] return the detected filetype_t value
    */
    int detect_mem(const uint8_t* buffer, uint32_t size);

    /*
    * @desc detect the filetype fastly
    *
    * [return] return the detected filetype_t value
    */
    int detect(ifile* reader);
};

