#include <assert.h>
#include <string.h>
#include "ole.h"

#pragma pack(push, 1)

/*
* VBA (Visual Basic for Applications), versions 5 and 6
* The first seven bytes of the stream are version-independent.
*/
typedef struct __st__VBA_PROJECT {
    uint8_t       magic[2];  /* MUST be 0x61CC */
    uint16_t      version;   /* the version of VBA used to create the VBA project */
    uint8_t       reserved2; /* MUST be 0x00 */
    uint16_t      reserved3; /* Undefined */
    uint8_t       performance_cache[1]; /* implementation-specific and version-dependent, MUST be ignored on read */
} vba_project_header_t;

#pragma pack(pop)

typedef struct __vba_project_nodes {
    entry_tree_item_t* vba_project{ nullptr };
    entry_tree_item_t* dir{ nullptr };
    entry_tree_item_t* project{ nullptr };
} vba_project_nodes_t;


/* TODO:
# According to MS-OFORMS section 2.1.2 Control Streams:
        # - A parent control, that is, a control that can contain embedded controls,
        #   MUST be persisted as a storage that contains multiple streams.
        # - All parent controls MUST contain a FormControl. The FormControl
        #   properties are persisted to a stream (1) as specified in section 2.1.1.2.
        #   The name of this stream (1) MUST be "f".
        # - Embedded controls that cannot themselves contain other embedded
        #   controls are persisted sequentially as FormEmbeddedActiveXControls
        #   to a stream (1) contained in the same storage as the parent control.
        #   The name of this stream (1) MUST be "o".
        # - all names are case-insensitive
*/

typedef struct __st_vba_version {
    uint32_t sig;
    const char* ver;
    int big_endian;        /* e.g. MAC Office */
} vba_version_t;

typedef struct __st_vba_project_module {
    std::u16string name;
    uint32_t     offset;        /* script's offset in the stream */
    uint32_t     directory_id;  /* the directory entry id in office, should be greater than 0 */

    __st_vba_project_module() { reset(); }
    void reset() {
        name.clear();
        offset = 0;
        directory_id = 0;
    }
} vba_project_module_t;

typedef struct __st_vba_project_props {
    bool     has_passwd;    /* todo */
    uint16_t codepage;
    uint16_t version_minor;
    uint32_t version_major;
    uint32_t syskind;
    std::vector<vba_project_module_t> modules;

    __st_vba_project_props() { reset(); }
    void reset() {
        has_passwd = false;
        codepage = 0;
        syskind = 0;
        version_major = 0;
        version_minor = 0;
        modules.clear();
    }
} vba_project_props_t;

class msvba {
public:
    /*
    * @desc extract vba from _vba_project
    */
    int extract_vba_project(const cfb_ctx_t* ctx, const olefile_t* ole, ifilehandler* cb_file, bool strict_mode = false);

    /*
    * @desc check vba content (reference: oledump.py)
    *        1. 所有 非空 行 是以 'Attribute ' 开头
    *        2. 非空 行的内容为 'Option Explicit'
    * 当 vba scripts 的内容只有以上两种时，可以认为是：只有 Attribute
    */
    int vba_attribute_only(const uint8_t* script_utf8, uint32_t size);

public:
    /*
    * @desc search for vba projects in the olefile
    * 
    * @param ole
    * @param strict_mode       if true, the MUST files in ms-docs should not be missed.
    * @param[out] projects     some continuous vba_project_nodes_t blocks
    * @param[out] count        if projects is null, count will be filled with the number of vba_project_nodes_t
    * @return int
    */
    static int find_vba_projects(const olefile_t* ole, bool strict_mode, vba_project_nodes_t* projects, uint32_t* count);

    /*
    * @desc search for a vba script in stream's bytes。
    *       if exists，try to unpack it and get a vba script.
    */
    static int search_stream_for_vba(const uint8_t* data, uint32_t size, uint32_t* offset, uint8_t** script, uint32_t* script_size);

    /**
     * @brief extract dir
     *
     * @param stream_unpacked     the unpacked dir stream bytes
     * @param size                the stream size
     * @param[out] props          
     * @param ole_is_little_endian  office whether is little endian or not
     * @return int
     */
    static int parse_dir_stream(const uint8_t* stream_unpacked, uint32_t size, vba_project_props_t* props, bool ole_is_little_endian = true);

    /**
     * @brief extract _vba_project
     *
     * @param stream_unpacked     the unpacked vba_project stream bytes 
     * @param size                the stream size
     * @param[out] props          
     * @param ole_is_little_endian  office whether is little endian or not
     * @return int
     */
    static int parse_vba_project_stream(const uint8_t* stream_unpacked, uint32_t size, vba_project_header_t* props, bool ole_is_little_endian = true);

    /**
     * @brief
     * @param ole
     * @param did
     * @param unpacked
     * @param unpacked_size
     * @param stream_offset     when decompressing, start from the offset of unpacked stream
     * @return int
     */
    static int unpack_stream(const olefile_t* ole, uint32_t did, uint8_t** unpacked, uint32_t* unpacked_size, uint32_t stream_offset = 0);
};