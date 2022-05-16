#pragma once
#include "ole.h"
#include "offcrypto_assist.h"

typedef struct __st_ooxml_file : file_obj_t {
    iziper*                unzip;
    int32_t                xl_workbook; /* sheets and definedNames */
    int32_t                xl_sharedstring;
    int32_t                setting_rels;
    int32_t                workbook_rels;
    int32_t                vbaproject_id; /* if there is more than one vbaProject.bin, we only process the first one; */ 
                                          /* vbaproject_id should be less than 0xFFFF; */
    int32_t                macrosheet_cnt;
    int32_t                macrosheets[64];

    int32_t zipitem_cnt() const {
        return unzip ? unzip->item_cnt() : 0;
    }

    const char* zipitem_path(int32_t id) const {
        return unzip ? unzip->item_utf8path(id) : 0;
    }

    __st_ooxml_file() { reset(); }
    void reset() {
        file = nullptr;
        unzip = nullptr;
        container = nullptr;
        filetype = vbaproject_id = xl_workbook = setting_rels = 0;
        xl_sharedstring = workbook_rels = 0;
        macrosheet_cnt = 0;
        memset(macrosheets, 0, sizeof(macrosheets));
    }
} ooxml_file_t;

class Ooxml {
public:
    /**
     * @brief check zip package for ooxml type and other informations
     *
     * @param zip_items                the full path of items in the zip package, eg. word/_rels/document.xml.rels
     * @param count                    zip_items count
     * @param[in out] ooxml_datas        
     * @return 
     */
    int parse_ooxml_catalog(const cfb_ctx_t* ctx, const char** zip_items, uint32_t count, ooxml_file_t* ooxml);

    /**
     * @brief extract embedded objects
     *
     * @param ctx                   optional, the workflow context
     * @param ooxml                 the parsed ooxml information
     * @param cb_file
     * @param passwd                reserved
     * @return int
     */
    int extract(const cfb_ctx_t* ctx, const ooxml_file_t* ooxml, ifilehandler* cb_file, const char16_t* passwd);

    /**
     * @brief decrypt a ooxml from an ole file
     *
     * @param ctx                   the workflow context
     * @param ole 
     * @param passwd
     * @param [out]crypto           optional, encryption information
     * @param [out]out
     * @return int
     */
    int decrypt_from_ole(const cfb_ctx_t* ctx, const olefile_t* ole, const char16_t* passwd, crypto_dataspace_t* crypto, ifile** out);

protected:
    static const bool is_little_endian{ true };
};
