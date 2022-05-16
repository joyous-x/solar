#pragma once
#include "ole.h"
#include "offcrypto_defines.h"

#pragma pack(push, 1)

typedef struct __st_fib_base {
    uint16_t ident;  // MUST be 0xA5EC
    uint16_t fib;
    uint16_t unused;
    uint16_t lid;
    uint16_t pn_next;
    uint16_t flags;
    uint16_t fib_back; // MUST be 0x00BF or 0x00C1
    uint32_t key;
    uint8_t envr;
    uint8_t flag_b;
    uint32_t reserved[3];

    bool is_template() const { return 0x1 == (flags & 0x1); }
    bool only_autotext() const { return 0x2 == (flags & 0x2); }
    bool last_save_is_incremental() const { return 0x4 == (flags & 0x4); }
    bool has_pic() const { return 0x8 == (flags & 0x8); }
    bool encrypted() const { return 0x100 == (flags & 0x100); }
    void reset_encrypted() { flags &= (0xFFFF ^ 0x100); }
    bool is_1table() const { return 0x200 == (flags & 0x200); }
    bool has_write_reservation() const { return 0x800 == (flags & 0x800); }
    bool obfuscated() const { return 0x8000 == (flags & 0x8000); }
} fib_base_t;

/* 744 bytes */
typedef struct FibRgFcLcb97 {
    uint32_t fcStshfOrig;
    uint32_t lcbStshfOrig;
    uint32_t fcStshf;
    uint32_t lcbStshf;
    uint32_t fcPlcffndRef;
    uint32_t lcbPlcffndRef;
    uint32_t fcPlcffndTxt;
    uint32_t lcbPlcffndTxt;
    uint32_t fcPlcfandRef;
    uint32_t lcbPlcfandRef;
    uint32_t fcPlcfandTxt;
    uint32_t lcbPlcfandTxt;
    uint32_t fcPlcfSed;
    uint32_t lcbPlcfSed;
    uint32_t fcPlcPad;
    uint32_t lcbPlcPad;
    uint32_t fcPlcfPhe;
    uint32_t lcbPlcfPhe;
    uint32_t fcSttbfGlsy;
    uint32_t lcbSttbfGlsy;
    uint32_t fcPlcfGlsy;
    uint32_t lcbPlcfGlsy;
    uint32_t fcPlcfHdd;
    uint32_t lcbPlcfHdd;
    uint32_t fcPlcfBteChpx;
    uint32_t lcbPlcfBteChpx;
    uint32_t fcPlcfBtePapx;
    uint32_t lcbPlcfBtePapx;
    uint32_t fcPlcfSea;
    uint32_t lcbPlcfSea;
    uint32_t fcSttbfFfn;
    uint32_t lcbSttbfFfn;
    uint32_t fcPlcfFldMom;
    uint32_t lcbPlcfFldMom;
    uint32_t fcPlcfFldHdr;
    uint32_t lcbPlcfFldHdr;
    uint32_t fcPlcfFldFtn;
    uint32_t lcbPlcfFldFtn;
    uint32_t fcPlcfFldAtn;
    uint32_t lcbPlcfFldAtn;
    uint32_t fcPlcfFldMcr;
    uint32_t lcbPlcfFldMcr;
    uint32_t fcSttbfBkmk;
    uint32_t lcbSttbfBkmk;
    uint32_t fcPlcfBkf;
    uint32_t lcbPlcfBkf;
    uint32_t fcPlcfBkl;
    uint32_t lcbPlcfBkl;
    uint32_t fcCmds;
    uint32_t lcbCmds;
    uint32_t fcUnused1;
    uint32_t lcbUnused1;
    uint32_t fcSttbfMcr;
    uint32_t lcbSttbfMcr;
    uint32_t fcPrDrvr;
    uint32_t lcbPrDrvr;
    uint32_t fcPrEnvPort;
    uint32_t lcbPrEnvPort;
    uint32_t fcPrEnvLand;
    uint32_t lcbPrEnvLand;
    uint32_t fcWss;
    uint32_t lcbWss;
    uint32_t fcDop;
    uint32_t lcbDop;
    uint32_t fcSttbfAssoc;
    uint32_t lcbSttbfAssoc;
    uint32_t fcClx;
    uint32_t lcbClx;
    uint32_t fcPlcfPgdFtn;
    uint32_t lcbPlcfPgdFtn;
    uint32_t fcAutosaveSource;
    uint32_t lcbAutosaveSource;
    uint32_t fcGrpXstAtnOwners;
    uint32_t lcbGrpXstAtnOwners;
    uint32_t fcSttbfAtnBkmk;
    uint32_t lcbSttbfAtnBkmk;
    uint32_t fcUnused2;
    uint32_t lcbUnused2;
    uint32_t fcUnused3;
    uint32_t lcbUnused3;
    uint32_t fcPlcSpaMom;
    uint32_t lcbPlcSpaMom;
    uint32_t fcPlcSpaHdr;
    uint32_t lcbPlcSpaHdr;
    uint32_t fcPlcfAtnBkf;
    uint32_t lcbPlcfAtnBkf;
    uint32_t fcPlcfAtnBkl;
    uint32_t lcbPlcfAtnBkl;
    uint32_t fcPms;
    uint32_t lcbPms;
    uint32_t fcFormFldSttbs;
    uint32_t lcbFormFldSttbs;
    uint32_t fcPlcfendRef;
    uint32_t lcbPlcfendRef;
    uint32_t fcPlcfendTxt;
    uint32_t lcbPlcfendTxt;
    uint32_t fcPlcfFldEdn;
    uint32_t lcbPlcfFldEdn;
    uint32_t fcUnused4;
    uint32_t lcbUnused4;
    uint32_t fcDggInfo;
    uint32_t lcbDggInfo;
    uint32_t fcSttbfRMark;
    uint32_t lcbSttbfRMark;
    uint32_t fcSttbfCaption;
    uint32_t lcbSttbfCaption;
    uint32_t fcSttbfAutoCaption;
    uint32_t lcbSttbfAutoCaption;
    uint32_t fcPlcfWkb;
    uint32_t lcbPlcfWkb;
    uint32_t fcPlcfSpl;
    uint32_t lcbPlcfSpl;
    uint32_t fcPlcftxbxTxt;
    uint32_t lcbPlcftxbxTxt;
    uint32_t fcPlcfFldTxbx;
    uint32_t lcbPlcfFldTxbx;
    uint32_t fcPlcfHdrtxbxTxt;
    uint32_t lcbPlcfHdrtxbxTxt;
    uint32_t fcPlcffldHdrTxbx;
    uint32_t lcbPlcffldHdrTxbx;
    uint32_t fcStwUser;
    uint32_t lcbStwUser;
    uint32_t fcSttbTtmbd;
    uint32_t lcbSttbTtmbd;
    uint32_t fcCookieData;
    uint32_t lcbCookieData;
    uint32_t fcPgdMotherOldOld;
    uint32_t lcbPgdMotherOldOld;
    uint32_t fcBkdMotherOldOld;
    uint32_t lcbBkdMotherOldOld;
    uint32_t fcPgdFtnOldOld;
    uint32_t lcbPgdFtnOldOld;
    uint32_t fcBkdFtnOldOld;
    uint32_t lcbBkdFtnOldOld;
    uint32_t fcPgdEdnOldOld;
    uint32_t lcbPgdEdnOldOld;
    uint32_t fcBkdEdnOldOld;
    uint32_t lcbBkdEdnOldOld;
    uint32_t fcSttbfIntlFld;
    uint32_t lcbSttbfIntlFld;
    uint32_t fcRouteSlip;
    uint32_t lcbRouteSlip;
    uint32_t fcSttbSavedBy;
    uint32_t lcbSttbSavedBy;
    uint32_t fcSttbFnm;
    uint32_t lcbSttbFnm;
    uint32_t fcPlfLst;
    uint32_t lcbPlfLst;
    uint32_t fcPlfLfo;
    uint32_t lcbPlfLfo;
    uint32_t fcPlcfTxbxBkd;
    uint32_t lcbPlcfTxbxBkd;
    uint32_t fcPlcfTxbxHdrBkd;
    uint32_t lcbPlcfTxbxHdrBkd;
    uint32_t fcDocUndoWord9;
    uint32_t lcbDocUndoWord9;
    uint32_t fcRgbUse;
    uint32_t lcbRgbUse;
    uint32_t fcUsp;
    uint32_t lcbUsp;
    uint32_t fcUskf;
    uint32_t lcbUskf;
    uint32_t fcPlcupcRgbUse;
    uint32_t lcbPlcupcRgbUse;
    uint32_t fcPlcupcUsp;
    uint32_t lcbPlcupcUsp;
    uint32_t fcSttbGlsyStyle;
    uint32_t lcbSttbGlsyStyle;
    uint32_t fcPlgosl;
    uint32_t lcbPlgosl;
    uint32_t fcPlcocx;
    uint32_t lcbPlcocx;
    uint32_t fcPlcfBteLvc;
    uint32_t lcbPlcfBteLvc;
    uint32_t dwLowDateTime;
    uint32_t dwHighDateTime;
    uint32_t fcPlcfLvcPre10;
    uint32_t lcbPlcfLvcPre10;
    uint32_t fcPlcfAsumy;
    uint32_t lcbPlcfAsumy;
    uint32_t fcPlcfGram;
    uint32_t lcbPlcfGram;
    uint32_t fcSttbListNames;
    uint32_t lcbSttbListNames;
    uint32_t fcSttbfUssr;
    uint32_t lcbSttbfUssr;
} fib_rgfclcb97_t;

/* 744 + 4*30 bytes */
typedef struct FibRgFcLcb2000 : fib_rgfclcb97_t {
    uint32_t fcPlcfTch;
    uint32_t lcbPlcfTch;
    uint32_t fcRmdThreading;
    uint32_t lcbRmdThreading;
    uint32_t fcMid;
    uint32_t lcbMid;
    uint32_t fcSttbRgtplc;
    uint32_t lcbSttbRgtplc;
    uint32_t fcMsoEnvelope;
    uint32_t lcbMsoEnvelope;
    uint32_t fcPlcfLad;
    uint32_t lcbPlcfLad;;
    uint32_t fcRgDofr;
    uint32_t lcbRgDofr;
    uint32_t fcPlcosl;
    uint32_t lcbPlcosl;
    uint32_t fcPlcfCookieOld;
    uint32_t lcbPlcfCookieOld;
    uint32_t fcPgdMotherOld;
    uint32_t lcbPgdMotherOld;
    uint32_t fcBkdMotherOld;
    uint32_t lcbBkdMotherOld;
    uint32_t fcPgdFtnOld;
    uint32_t lcbPgdFtnOld;
    uint32_t fcBkdFtnOld;
    uint32_t lcbBkdFtnOld;
    uint32_t fcPgdEdnOld;
    uint32_t lcbPgdEdnOld;
    uint32_t fcBkdEdnOld;
    uint32_t lcbBkdEdnOld;
} fib_rgfclcb2000_t;

/* 1088(744 + 120 + 4*56) bytes */
typedef struct FibRgFcLcb2002 : fib_rgfclcb2000_t {
    uint32_t          fields[56];
} fib_rgfclcb2002_t;

/* 1312(744 + 120 + 4*56 + 4*56) bytes */
typedef struct FibRgFcLcb2003 : fib_rgfclcb2002_t {
    uint32_t          fields[56];
} fib_rgfclcb2003_t;

/* 1464(744 + 120 + 4*56 + 4*56 + 4*38) bytes */
typedef struct FibRgFcLcb2007 : fib_rgfclcb2003_t {
    uint32_t          fields[38];
} fib_rgfclcb2007_t;

typedef struct __st_fib {
    fib_base_t base;
    uint16_t   csw;
    struct FibRgW97 {
        uint16_t reserved[13];
        uint16_t lidfe;
    } rgw;
    uint16_t   cslw;
    struct FibRgLw97 {
        uint32_t cb_mac;
        uint32_t reserved[2];
        uint32_t ccpText;
        uint32_t ccpFtn;
        uint32_t ccpHdd;
        uint32_t reserved2;
        uint32_t ccpAtn;
        uint32_t ccpEdn;
        uint32_t ccpTxbx;
        uint32_t ccpHdrTxbx;
        uint32_t reserved3[11];
    } rglw;
    /*
    * cb_rgfclcb specifies the count of 64-bit values corresponding to fibRgFcLcbBlob that follow
    * 
    * nFib = 0x00C1£¬then cbRgFcLcb = 0x005D
    * nFib = 0x00D9£¬then cbRgFcLcb = 0x006C
    * nFib = 0x0101£¬then cbRgFcLcb = 0x0088
    * nFib = 0x010C£¬then cbRgFcLcb = 0x00A4
    * nFib = 0x0112£¬then cbRgFcLcb = 0x00B7
    */
    uint16_t    cb_rgfclcb;
    union {
        fib_rgfclcb97_t   _97;
        fib_rgfclcb2000_t _2000;
        fib_rgfclcb2002_t _2002;
        fib_rgfclcb2003_t _2003;
        fib_rgfclcb2007_t _2007;
    } rgfclcb; /* variable */
    /*
    * csw_new specifies the count of 16-bit values corresponding to fibRgCswNew that follow
    * 
    * nFib = 0x00C1£¬then csw_new = 0
    * nFib = 0x00D9£¬then csw_new = 0x0002
    * nFib = 0x0101£¬then csw_new = 0x0002
    * nFib = 0x010C£¬then csw_new = 0x0002
    * nFib = 0x0112£¬then csw_new = 0x0005
    */
    uint16_t    csw_new;
    struct FibRgCswNew {
        uint16_t fib_new;
        union RgCswNewData {
            struct FibRgCswNewData2000 {
                uint16_t cQuickSavesNew;
            } _2000;
            struct FibRgCswNewData2007 : FibRgCswNewData2000 {
                uint16_t lidThemeOther;
                uint16_t lidThemeFE;
                uint16_t lidThemeCS;
            } _2007;
        } rgcsw_newdata;
    } rgcsw_new; /* variable, may not exist */

    __st_fib() { memset((void*)this, 0, sizeof(__st_fib)); }
} fib_t;

typedef struct __st_ODT {
    uint16_t ODTPersist1{ 0 };
    uint16_t cf{ 0 };
    uint16_t ODTPersist2{ 0 }; /* maybe does not exist according to the ObjInfo stream size */

    /*
    * If fDefHandler is 1, then the application MUST assume that this OLE object¡¯s 
    * class identifier (CLSID) is {00020907-0000-0000-C000-000000000046}.
    */
    bool fDefHandler() { return ODTPersist1 & 0x2; }
    /* specifies whether this OLE object is a link */
    bool fLink() { return ODTPersist1 & 0x10; }
    /* specifies whether this OLE object is being represented by an icon. */
    bool fIcon() { return ODTPersist1 & 0x40; }
    /*
    * fIsOle1 specifies whether this OLE object is only compatible with OLE 1.
    * If this bit is zero, then the object is compatible with OLE 2.
    */
    bool fIsOle1() { return ODTPersist1 & 0x80; }
    /* 
    * fManual specifies whether the user has requested that this OLE object 
    * only be updated in response to a user action
    */
    bool fManual() { return ODTPersist1 & 0x100; }
    /* 
    * specifies whether this OLE object has requested to be notified 
    * when it is resized by its container
    */
    bool fRecomposeOnResize() { return ODTPersist1 & 0x200; }
    /* fOCX specifies whether this object is an OLE control. */
    bool fOCX() { return ODTPersist1 & 0x1000; }
    /* 
    * If fOCX is zero, then this bit MUST be zero. If fOCX is 1, then fStream is a bit
    * that specifies whether this OLE control stores its data in a single stream instead of a storage.
    */
    bool fStream() { return ODTPersist1 & 0x2000; }
    /* fViewObject specifies whether this OLE object supports the IViewObject interface */
    bool fViewObject() { return ODTPersist1 & 0x8000; }
} odt_t;

#pragma pack(pop)

typedef struct __st_msdoc_t : olefile_t {
    bool             big_endian{0};
    uint32_t         did_wordocument{ 0 };
    uint32_t         did_table{ 0 };
    uint32_t         did_objectpool{ 0 };
    uint32_t         did_encryption{ 0 };
    fib_t            fib;
    bin_encryption_t crypt;

    ~__st_msdoc_t() { release(); }

    void release() {
        olefile_t::release();
    }

    __st_msdoc_t& operator =(const __st_msdoc_t& v) = delete;
} msdoc_t;

class msdoc {
public:
    /**
     * @brief pasre an olefile for msdoc_t
     *
     * @param ctx                   optional, the workflow context
     * @param olefile
     * @param[in out] doc
     * @return int
     */
    static int parse(const cfb_ctx_t* ctx, const olefile_t* olefile, msdoc_t* doc);

    /**
     * @brief decrypt a stream in ole in place
     *
     * @param ctx                   optional, the workflow context
     * @param ppt                   the parsed ppt information
     * @param passwd
     * @param name                  stream name
     * @param[in out] data			stream
     * @param size				    stream size
     * @return int
     */
    static int decrypt(const cfb_ctx_t* ctx, const msdoc_t* doc, const char16_t * passwd, const char16_t* name, uint8_t* data, uint32_t size);

    /**
     * @brief extract embedded objects
     *
     * @param ctx                   optional, the workflow context
     * @param [in out]doc           the parsed doc information. it will record some fields when extracting.
     * @param cb_file
     * @param passwd                optional
     * @return int
     */
    static int extract(const cfb_ctx_t* ctx, msdoc_t* doc, ifilehandler* cb_file, const char16_t* passwd);

protected:
    static const bool is_little_endian{ true };
};