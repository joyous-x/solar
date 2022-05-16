#pragma once
#include "ole_utils.h"
#include "offcrypto_defines.h"
#include "ms_biff.h"
#include "ms_xls.h"

#pragma pack(push, 1)

struct RwU {
    uint16_t rw;
};

struct ColRelU {
    uint16_t col;
};

struct ColRelNegU {
    uint16_t col;
};

typedef struct __st_xlm_cell {
    uint16_t row;
    uint16_t col;
    uint16_t ixfe;
} xlm_cell_t, Cell;

typedef struct __st_variable_field {
    const uint8_t* data; /* MUST not be freed */
    uint32_t       size;
} var_field_t;

typedef struct __st_xlm_refu {
    uint16_t rwFirst;
    uint16_t rwLast;
    uint8_t colFirst;
    uint8_t colLast;
} xlm_refu_t, xlm_ref_t, RefU, Ref;

typedef struct __st_format_run {
    uint16_t ich;
    struct FontIndex {
        uint16_t ifnt;
    } ifnt;
} format_run_t, FormatRun;

typedef struct __st_ext_rst {
    uint16_t reserved; /* must be 1 */
    uint16_t cb;
    struct Phs {
        uint16_t ifnt;
        uint16_t flags;
    } phs;
    struct RPHSSub {
        uint16_t crun;
        uint16_t cch;
        var_field_t st;
    } rphssub;
    /*
    * rgphruns : An array of PhRuns
    * 
    * struct PhRuns {
    *   int16_t ichFirst;
    *   int16_t ichMom;
    *   int16_t cchMom;
    * };
    */
    var_field_t rgphruns;
} ext_rst_t, ExtRst;

typedef struct __st_xl_uni_rich_extended_str {
    uint16_t cch;
    uint8_t flags;
    uint16_t cRun;
    int32_t cbExtRst;
    var_field_t rgb;
    var_field_t rgRun; /* An optional array of FormatRun structures that specifies the formatting for each text run. */
    ext_rst_t extRst; /* An optional ExtRst that specifies the phonetic string data. */
    bool fHighByte() { return (flags & 0x1); }
    bool fExtSt() { return (flags & 0x4); }
    bool fRichSt() { return (flags & 0x8); }
} xl_uni_rich_extended_str_t, XLUnicodeRichExtendedString;

typedef struct __st_label_sst {
    xlm_cell_t cell;
    uint32_t isst; /* zero-based index in the array of the rgb field of the SST record */
} label_sst_t, LabelSst;

/*
* The RgceLoc structure specifies a reference to a single cell where relative references
* are stored as coordinates.
*/
typedef struct __st_rgce_loc {
    RwU row;
    ColRelU column;
} rgce_loc_t, RgceLoc;

/*
* The RgceLocRel structure specifies a single cell reference where the relative portions of relative
* references are specified as offsets from the cell in which the formula (section 2.2.2) is evaluated.
*/
typedef struct __st_rgce_loc_rel {
    RwU row;
    ColRelNegU column;
} rgce_loc_rel_t, RgceLocRel;

typedef struct __st_xti_index {
    uint16_t index;
    bool validable() const { return index != 0xFFFF; }
} xti_index_t, XtiIndex;


typedef struct __st_xlm_FormulaValue {
    uint8_t bytes[6];
    uint16_t fExprO;

    bool is_xnum() { return fExprO != 0xFFFF; }
    bool is_str() { return (!is_xnum() && (bytes[0] == 0x00 || bytes[0] == 0x03)); }
    bool is_blank() { return (!is_xnum() && bytes[0] == 0x03); }
} xlm_formula_val_t;

typedef struct __st_recorder_boundsheet8 {
    uint32_t lbPlyPos;
    uint16_t flags;
    /* ShortXLUnicodeString */
    uint8_t cch;        /* MUST be greater than or equal to 1 and less than or equal to 31 */
    uint8_t cch_flag;
    uint8_t name[64];    /* variable */
} record_boundsheet8_t;

typedef struct __st_LblRecord {
    uint16_t flags;
    uint8_t chKey;
    uint8_t cch;
    uint16_t cce;
    uint16_t reserved1;
    uint16_t itab;
    uint8_t reserved2[4];
    /* 
    * variable, XLUnicodeStringNoCch or XLNameUnicodeString
    * cch is only one byte, for simplicity, make it 256 characters.
    */
    char    name[256];
    /* 
    * rgce，NameParsedFormula 
    * only some fields in the NameParsedFormula defined here
    */
    struct {
        uint8_t     ptg;
        uint16_t    ixti;
        uint16_t    row;
        uint16_t    col;
    } rgce;
} lbl_record_t;

/*
* XTI structure
*    itab : -2, Workbook-level reference that applies to the entire workbook.
*    itab : -1, Sheet-level reference. The first sheet in the reference could not be found.
*    itab : >= 0, Sheet-level reference. This specifies the first sheet in the reference.
*/
typedef struct __st_XTIRecord {
    uint16_t isupbook;
    int16_t itab_first;
    int16_t itab_last;
} xti_record_t;

/*
* SupBook record
*/
typedef struct __st_SupBookRecord {
    uint16_t ctab;
    uint16_t cch;
    /* virtPath, variable */
    /* rgst, variable */
} supbook_record_t;

/* CellParsedFormula, ArrayParsedFormula, SharedParsedFormula */
typedef struct __st_parsed_formula {
    uint16_t cce; /* specifies the length of rgce in bytes, should less than or equal to 1800 bytes */
    /* 
    * rgce, variable, a structure that specifies the formula
    * rgcb, variable, RgbExtra, specifies ancillary data for the formula
    */
    var_field_t rgce_rgcb;
} parsed_formula_t, cell_parsed_formula_t, array_parsed_formula_t, shared_parsed_formula_t;

typedef struct __st_array_record {
    xlm_refu_t    ref;
    uint16_t    reserved;
    uint32_t    unused;
    array_parsed_formula_t formula;
} record_array_t;

typedef struct __st_shrfmla_record {
    xlm_refu_t     ref;
    uint8_t        reserved;
    uint8_t        cuse;
    shared_parsed_formula_t formula;
} record_shrfmla_t;

/*
* [MS-XLS v20190618] PageNo: 949
*/
typedef struct __st_record_formula : xlm_cell_t {
    xlm_formula_val_t   val;
    uint16_t            flags;
    uint32_t            chn;
    cell_parsed_formula_t formula;

    bool fShrFmla() { return 0 != (flags & 0x8); }
} record_formula_t;

#pragma pack(pop) 

class xlm {
public:
    /**
     * @brief decrypt OLE2 Workbook stream
     * 
     * @param            ctx
     * @param[in out]    data
     * @param            size
     * @param            passwd
     * @param[out]        encryption
     * @return int
     */
    int decrypt_xlm_inplace(const bin_encryption_t* encrypt, const char16_t* passwd, uint8_t* buffer, uint32_t size, bool little_endian);

    /**
     * @brief parse OLE2 Workbook stream
     *
     * The stream should be encoded with <= BIFF8.
     * The found_macro and found_image out-params should be checked even if an error occured.
     * 
     * if this file is encrypted and passwd is given, we try to decrypt it. 
     * if this file is encrypted but passwd is not given, encryption flag is set to the encryption field of xlm_file_t. 
     *
     * @param ctx                   (optional)the workflow context
     * @param buffer
     * @param size
     * @param container             optional, used to make the object chain of extracted files
     * @param[out] xlm
     * @param macros                
     * @param drawing                drawing_group
     * @return int                    0 if successfull, else some error code
     */
    int parse_xlm(const cfb_ctx_t* ctx, uint8_t* buffer, uint32_t size, const file_obj_t* container, ms_workbook_t* xlm, ifile* drawings);

    /**
     * @brief clear the content of the boundsheet
     *
     * @param ctx                   (optional)the workflow context
     * @param[in out] buffer
     * @param[in out] size
     * @param bs
     * @return int                    0 if successfull, else some error code
     */
    int reset_boundsheet(const cfb_ctx_t* ctx, uint8_t* buffer, uint32_t& size, xl_bin_boundsheet_t* bs);

    /**
     * @brief extract images from drawing group
     *
     * @param drawing_group             drawing_group datas starting with the record header
     * @param file_handler_t            callback which will process the extracted files
     * @return int                      0 if successfull, else some error code
     */
    int extract_images_from_drawing_group(const cfb_ctx_t* ctx, file_obj_t* drawing_group, ifilehandler* handler);

protected:
    const bool is_little_endian{ true };
};