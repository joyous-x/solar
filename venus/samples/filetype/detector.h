#pragma once
#include "../common/file/ifile.h"

namespace filetype {
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
    int detect(ifile* reader, zip_creator_t z_reader, void* user_data = nullptr);

    int is_archive_bind(ifile* reader);

    int is_archive_swc(ifile* reader);

    /*
    * @desc translate the filetype_t value to string 
    */
    const char* ftype2str(uint32_t ftype);
};
