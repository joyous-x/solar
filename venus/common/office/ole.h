#pragma once
#include "./cfb_base.h"
#include "../file/ifile.h"
#include "../file/struct_ole.h"

#pragma pack(push, 1)

typedef struct __st_hwp5_header {
    uint8_t signature[32];
    uint32_t version;
    uint32_t flags;

    /* uint8_t reserved[216] */
    __st_hwp5_header() { reset(); }

    ~__st_hwp5_header() { reset(); }

    void reset() {
        memset(this, 0, sizeof(__st_hwp5_header));
    }
} hwp5_header_t;

#pragma pack(pop)

typedef struct __st_entry_tree_item {
    uint32_t                depth;
    ole2_directory_entry_t* item;
    __st_entry_tree_item* parent;
    __st_entry_tree_item* child_first;
    __st_entry_tree_item* slibing_next;

    ~__st_entry_tree_item() { reset(); }
    void reset() {
        memset(this, 0, sizeof(__st_entry_tree_item));
    }
} entry_tree_item_t;

typedef struct __ST_OLE_FILE : file_obj_t {
    ole2_header_t   header;
    uint32_t        ssector_entry_sid{ OLE_FREE_SECTOR };

    /* directory_entry */
    uint32_t                       entry_count{ 0 };
    ole2_directory_entry_t* entries{ 0 };
    entry_tree_item_t* entry_tree{ 0 };

    /* sat & ssat */
    uint32_t* msat_table{ 0 };        /* host byte order*/
    uint32_t        msat_table_size{ 0 };
    uint32_t* sat_table{ 0 };            /* raw byte order*/
    uint32_t        sat_table_size{ 0 };
    uint32_t* ssat_table{ 0 };        /* raw byte order*/
    uint32_t        ssat_table_size{ 0 };

    virtual ~__ST_OLE_FILE() { release(); }

    void reset() {
        uint32_t offset_start = (uint32_t)((uint8_t*)&((__ST_OLE_FILE*)0)->header - (uint8_t*)0);
        memset((void*)&header, 0, sizeof(__ST_OLE_FILE) - offset_start);
        ssector_entry_sid = OLE_FREE_SECTOR;
        file = 0, filetype = 0, attr =0, container = 0;
    }

    void release() {
        container = nullptr;
        filetype = 0;
        ssector_entry_sid = OLE_FREE_SECTOR;

        header.reset();

        if (sat_table) {
            free(sat_table);
            sat_table = 0;
        }
        sat_table_size = 0;

        if (msat_table) {
            free(msat_table);
            msat_table = 0;
        }
        msat_table_size = 0;

        if (ssat_table) {
            free(ssat_table);
            ssat_table = 0;
        }
        ssat_table_size = 0;

        if (entry_tree) {
            free(entry_tree);
            entry_tree = 0;
        }
        if (entries) {
            free(entries);
            entries = 0;
        }
        entry_count = 0;
    }
} olefile_t;


typedef int ole_walk_prop_tree_handler(const cfb_ctx_t* ctx, olefile_t* ole, uint32_t directory_id, ifilehandler* handler);


class ole {
public:
    /**
     * @brief parse an ole2 file: header、directory entries and sat、ssat
     *
     * @param ctx                   optional, the workflow context
     * @param pfile                 the file interface need to be parsed
     * @param[out] pole                olefile structure informations
     * @return int
     */
    static int parse(const cfb_ctx_t* ctx, ifile* pfile, olefile_t* pole);

    /**
     * @brief dig the more detailed filetype of olefile_t
     *
     * @param ctx                   optional, the workflow context
     * @param ole                    the file interface need to be parsed
     * @return the filetype if successed, otherwise return BQ::ERR
     */
    static int filetype(const cfb_ctx_t* ctx, const olefile_t* ole);

    /**
     * @brief walk an ole2 property tree, calling the handler for each file found.
     *          Max recursion level is 100.
     *
     * @param ctx                   optional, the workflow context
     * @param pole                  olefile structure
     * @param index                 index of the property being walked, to be recorded with a pointer to the root node in an ole2 node list.
     * @param handler               the file handler to call when a file is found.
     * @return int
     */
    static int walk_property_tree(const cfb_ctx_t* ctx, olefile_t* olefile, uint32_t index, ifilehandler* file_handler, ole_walk_prop_tree_handler handler);

public:
    /**
     * @brief read office full stream  (ole2 files may not be a block multiple in size)
     * @param ole
     * @param did                    read this did
     * @param want                  read 'size' bytes from the stream beginning。if size == -1, read the stream until ENDOFCHAIN
     * @param [out]buf              bytes readed, it is caller's responsibility to release the memory
     * @param [out]got              size readed
     * @return int
     */
    static int read_stream(const olefile_t* ole, uint32_t did, uint32_t want, uint8_t** buf, uint32_t* got);

    /**
     * @brief read office full stream  (ole2 files may not be a block multiple in size)
     * @param ole
     * @param did                    read this did
     * @param want                  read 'size' bytes from the stream beginning。if size == -1, read the stream until ENDOFCHAIN
     * @param [out]buf              bytes readed
     * @param [out]size             size readed
     * @return int
     */
    static int read_stream(const olefile_t* ole, uint32_t did, uint8_t* buff, uint32_t size, uint32_t* readed);

    /**
     * @brief modify the office stream
     *
     * !!! DO NOT CHANGE THE STREAM SIZE
     * 
     * @param ole
     * @param item
     * @param bytes
     * @param size        must be 0 or the same as the stream size. if size is 0, try to reset stream;
     * @return int
     */
    static int rewrite_stream(olefile_t* ole, uint32_t did, const uint8_t* bytes, uint32_t size);
};
