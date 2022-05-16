#pragma once
#include <cassert>
#include "ms_biff.h"
#include "../utils/utils.h"

int biff::version(const uint8_t* data, uint32_t data_size, uint16_t* bof_type, bool little_endian) {
    if (data_size < 8) {
        return biff::unknown;
    }

    auto version = biff::unknown;
    uint16_t code = XsUtils::byteorder2host<uint16_t>(*(uint16_t*)data, little_endian);
    uint16_t size = XsUtils::byteorder2host<uint16_t>(*(uint16_t*)(data + 2), little_endian);
    uint16_t ver = XsUtils::byteorder2host<uint16_t>(*(uint16_t*)(data + 4), little_endian);
    uint16_t type = XsUtils::byteorder2host<uint16_t>(*(uint16_t*)(data + 6), little_endian);

    switch (code) {
    case biff::v2: {
        if (size == 4) {
            version = code;
            assert(type == 0x10 || type == 0x20 || type == 0x40);
        }
        break;
    }
    case biff::v3:
    case biff::v4: {
        if (size == 6) {
            version = code;
            assert(type == 0x10 || type == 0x20 || type == 0x40 || type == 0x0100);
        }
        break;
    }
    case biff::v8: {
        if ((size == 8 || size == 16) && (type == 0x05 || type == 0x06 || type == 0x10 || type == 0x20 || type == 0x40 || type == 0x0100)) {
            if (ver == 0x0500) {
                version = biff::v5;
            }
            else if (ver == 0x600) {
                version = biff::v8;
            }
        }
        break;
    }
    }

    if (bof_type) {
        *bof_type = type;
    }

    return version;
}
