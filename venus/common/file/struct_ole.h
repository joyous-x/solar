#pragma once
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#pragma pack(push, 1)

/*
* MS-CFB Structure
*
* Reference: [MS-CFB]-210407.pdf (https://winprotocoldoc.blob.core.windows.net/productionwindowsarchives/MS-CFB/%5bMS-CFB%5d-210407.pdf)
*/

#define OLE_MIN_SIZE (0x100)
#define OLE_MAX_LOCAL_MSAT_SIZE (109)
#define OLE_HEADER_SIZE (512)
#define OLE_DIRECTOR_ENTRY_SIZE (128)
#define OLE_DIRECTOR_ENTRY_NAME_SIZE (32)

#define OLE_ENDOFCHAIN (0xFFFFFFFE)
#define OLE_MSAT_SECTOR (0xFFFFFFFC)
#define OLE_SAT_SECTOR (0xFFFFFFFD)
#define OLE_FREE_SECTOR (0xFFFFFFFF)
#define OLE_NO_STREAM (0xFFFFFFFF)

inline bool is_special_ole_sid(uint32_t sid) {
    return sid == OLE_ENDOFCHAIN || sid == OLE_MSAT_SECTOR || sid == OLE_SAT_SECTOR || sid == OLE_FREE_SECTOR;
}

/**
*
* @name ole2_header_t
* @desc	ole2 header structure
*
*/
typedef struct __st_ole2_header {
    unsigned char magic[8];             /* should be: 0xd0cf11e0a1b11ae1 */
    unsigned char clsid[16];            /* unique flag */
    uint16_t minor_version;             /* 文件格式修订号(一般为0x003E) */
    uint16_t major_version;             /* 文件格式版本号(一般为0x0003 或 0x0004) */
    uint16_t byte_order;                /* 字节序标识: FE FF = Little-Endian  FF FE = Big-Endian */
    uint16_t sector_shift;              /* usually 9 (2^9 = 512) */
    uint16_t short_sector_shift;        /* usually 6 (2^6 = 64) */
    uint8_t reserved[6];
    uint32_t directory_sector_count;    /* Number of Directory Sectors. If Major Version is 3, it MUST be zero. */
                                        /* This field is not supported for version 3 compound files. */
    uint32_t sat_sector_count;
    uint32_t directory_entry_sid;
    uint32_t transaction_signature;
    uint32_t min_stream_cutoff;         /* cutoff for files held in small blocks(4096) */
                                        /* 标准流的最小大小(一般为4096 bytes), 小于此值的流即为短流 */
    uint32_t ssat_entry_sid;            /* SSAT 的第一个sector的SID,或为–2 (End Of Chain SID) */
    uint32_t ssat_sector_count;         /* SSAT 的sector总数 */
    uint32_t msat_entry_sid;            /* MSAT 的第一个sector的SID,或为–2 (End Of Chain SID) */
    uint32_t msat_sector_count;
    uint32_t msat_array[OLE_MAX_LOCAL_MSAT_SIZE];

    __st_ole2_header() { reset(); }

    ~__st_ole2_header() { reset(); }

    void reset() {
        memset(this, 0, sizeof(__st_ole2_header));
    }
} ole2_header_t;


typedef enum em_ole2_dir_entry_type {
    ole_dir_entry_unknown = 0x00, /* Unknown or unallocated */
    ole_dir_entry_storage = 0x01,
    ole_dir_entry_stream = 0x02,
    ole_dir_entry_property = 0x04, /* unknown */
    ole_dir_entry_root_storage = 0x05,
} ole2_dir_entry_type;

typedef enum em_ole2_dir_entry_color {
    ole_dir_entry_red = 0x00,
    ole_dir_entry_black = 0x01,
} ole2_dir_entry_color;

/**
*
* @name __ST_OleCtx
* @desc	context for the ole extractor, fixed size 128 bytes.
*
* @note: Each storage object or stream object within a compound file is represented by a single directory entry
*
*/
typedef struct __st_ole2_directory_entry {
    char16_t name[OLE_DIRECTOR_ENTRY_NAME_SIZE]; /* The name is limited to 32 Unicode UTF-16 code points, including the required Unicode terminating null character */
    uint16_t name_bytes;
    uint8_t obj_type; /* defines in em_ole_dir_entry_type */
    uint8_t color; /* defines in em_ole2_dir_entry_color */
    uint32_t l_sib_did; /* Left Sibling ID, DID: directory entry identifier */
    uint32_t r_sib_did; /* Right Sibling ID, DID: directory entry identifier */
    uint32_t child_did; /* Child ID, DID: directory entry identifier */
    uint8_t clsid[16];
    uint32_t user_flags;
    uint32_t create_time[2]; /* for storage objects. The Windows FILETIME structure is used to represent this field in UTC. */
    uint32_t modify_time[2];
    uint32_t entry_sid; /* the first sector location for stream object; the first sector of the mini stream for a root storage object */
    uint64_t stream_bytes;

    __st_ole2_directory_entry() { reset(); }

    ~__st_ole2_directory_entry() { reset(); }

    void reset() {
        memset(this, 0, sizeof(__st_ole2_directory_entry));
    }
} ole2_directory_entry_t;

#pragma pack(pop)