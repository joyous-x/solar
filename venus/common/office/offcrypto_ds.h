#pragma once
#include <vector>
#include <iostream>
#include "ole.h"

typedef struct __st_dataspace_version {
    version_b4_t reader;
    version_b4_t updater;
    version_b4_t writer;
} ds_version_t;

typedef struct __st_datasapace_reference_component {
    uint32_t        type; /* MUST be 0x00000000 for a stream (1) or 0x00000001 for a storage */
    std::u16string  name; /* unicode_lp_p4_t */
} ds_reference_component_t;

typedef struct __st_datasapace_mapentry {
    uint32_t        length;
    uint32_t        component_count;
    std::vector<ds_reference_component_t> components;
    std::u16string  name; /* MUST be equal to the name of a stream in the \0x06DataSpaces\DataSpaceInfo storage */
} ds_mapentry_t;

typedef struct __st_datasapace_map {
    uint32_t        header_length; /* must be 8 */
    uint32_t        entry_count;
    std::vector<ds_mapentry_t> entries;

    __st_datasapace_map() : header_length(0), entry_count(0) {};
} ds_map_t;

typedef struct __st_datasapace_definition {
    uint32_t        header_length;
    uint32_t        transform_count;
    std::vector<std::u16string> transforms; /* one or more unicode_lp_p4_t */
    std::u16string  name;

    __st_datasapace_definition() : header_length(0), transform_count(0) {}
} ds_definition_t;

/*
* TransformInfoHeader
*/
typedef struct __st_datasapace_transform_header {
    /*
    * it is described in [MS-OFFCRYPTO] - v20210817:
    *     TransformLength specifies the number of bytes in this structure before the TransformName field.
    * but, it failed !!!
    */
    uint32_t        length;
    uint32_t        type;
    std::u16string  id;        /* unicode_lp_p4_t */
    std::u16string  name;    /* unicode_lp_p4_t */
    ds_version_t    ver;
} ds_transform_header_t;

typedef struct __st_datasapace_transform {
    std::u16string  name;

    struct IRMDS_transform_info {
        ds_transform_header_t header;
        struct {
            uint32_t length;    /* MUST be 0x00000004 */
        } extensibility_header;
    } transform;

    struct encryption_tran_info {
        std::string name;
        uint32_t    block_size; /* MUST be 0x0010 as specified by the AES */
        uint32_t    cipher_mode;
        uint32_t    reserved;
    } encrypt_tran;

} ds_transform_t;
