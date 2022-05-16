#pragma once
#include "cfb_base.h"

/*
* buffer:
*   [char*]{count}...[str]{count}...
*/
typedef struct __st_string_list {
    const char** items{ 0 };
    uint32_t     count{ 0 };
    uint8_t*     buffer{ 0 };
    uint32_t     bytes{ 0 };

    void release() {
        if (buffer) { free(buffer); buffer = nullptr; }
        count = bytes = 0; 
        items = 0;
    }
} string_list_t;

int parse_performance_cache(const uint8_t* data, uint32_t size, string_list_t* items, bool little_endian);

int decode_pcode(const uint8_t* data, uint32_t size, string_list_t* identifiers, uint32_t syskind, uint16_t vba_version, uint8_t** out, uint32_t* out_size);