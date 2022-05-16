#ifndef __XLM_DATA_H__
#define __XLM_DATA_H__

#include "./xlm.h"

typedef struct __st_xl_bin_cell : public xlm_cell_t {
    uint32_t        sheet_id{ 0 };
    uint32_t        record_type{ 0 };
    bool            is_shrfmla{ 0 };

    struct val_t {
        var_field_t     raw;
        bool            blank_str;
        std::string     evaluation;

        bool is_str() const {
            return blank_str || !evaluation.empty();
        }

        val_t() { reset(); }
        val_t(const val_t& v) { __copy(v); }
        val_t(const std::string& v) { reset(); evaluation = v; blank_str = evaluation.empty(); }
        val_t(const char* v) { reset(); if (v) evaluation = v; blank_str = evaluation.empty(); }
        val_t(const uint8_t* data, uint32_t size) { reset(); raw.data = data; raw.size = size; }
        val_t& operator=(const val_t& v) { __copy(v); return *this; }

        void reset() {
            evaluation = "";
            raw.data = 0;
            raw.size = 0;
            blank_str = false;
        }

        void __copy(const val_t& v) {
            evaluation = v.evaluation;
            raw.data = v.raw.data;
            raw.size = v.raw.size;
            blank_str = v.blank_str;
        }
    } val;
} xl_bin_cell_t;

typedef struct __st_shrfmla_item {
    uint32_t             sheet_id;
    record_shrfmla_t     data;
    xl_bin_cell_t::val_t val;
} shrfmla_item_t;

/*
* @brief
*  reference
*/
typedef struct __st_xl_bin_sheet {
    uint32_t sheet_id{ 0 };
    uint16_t sheet_type{ 0 };
    xl_bin_boundsheet_t bs;
    std::vector<xl_bin_cell_t> cells;
} xl_bin_sheet_t;

/*
* @brief the information of a binary xl
*  reference
*/
typedef struct __st_xl_bin_datas {
    std::vector<xti_record_t> xtis;
    std::vector<supbook_record_t> supbooks;
    std::vector<lbl_record_t> lbls;
    std::vector<std::string> sst;
    std::vector<shrfmla_item_t> shareds;
    std::vector<xl_bin_sheet_t> sheets;
    const uint8_t* data{ 0 };
    uint32_t size{ 0 };
    bool little_endian{ true };
} xl_bin_datas_t;

class xlm_records {
protected:
    bool little_endian{ true };
public:
    xlm_records(bool little_endian = true) : little_endian(little_endian) {};

    int read_header(const uint8_t* data, uint32_t size, biff::record_header_t& rcd) {
        if (size < sizeof(biff::record_header_t)) {
            return 0;
        }
        rcd.opcode = XsUtils::byteorder2host<uint16_t>(*(uint16_t*)data, little_endian);
        rcd.length = XsUtils::byteorder2host<uint16_t>(*(uint16_t*)(data + sizeof(rcd.opcode)), little_endian);
        return sizeof(rcd);
    }

    int read_Cell(const uint8_t* data, uint32_t size, xlm_cell_t& cell) {
        uint32_t size_min = sizeof(cell);
        if (size < size_min) {
            return 0;
        }
        memcpy(&cell, data, size_min);
        cell.row = XsUtils::byteorder2host<uint16_t>(cell.row, little_endian);
        cell.col = XsUtils::byteorder2host<uint16_t>(cell.col, little_endian);
        cell.ixfe = XsUtils::byteorder2host<uint16_t>(cell.ixfe, little_endian);
        return size;
    }

    int read_RefU(const uint8_t* data, uint32_t size, xlm_refu_t& refu) {
        uint32_t size_min = sizeof(refu);
        if (size < size_min) {
            return 0;
        }
        memcpy(&refu, data, size_min);
        refu.rwFirst = XsUtils::byteorder2host<uint16_t>(refu.rwFirst, little_endian);
        refu.rwLast = XsUtils::byteorder2host<uint16_t>(refu.rwLast, little_endian);
        return size;
    }

    int read_ShrFmla(const uint8_t* data, uint32_t size, record_shrfmla_t& rcd) {
        uint32_t size_min = (uint8_t*)&rcd.formula.rgce_rgcb - (uint8_t*)&rcd;
        if (size < size_min) {
            return 0;
        }
        assert(size_min == 10);

        memcpy(&rcd, data, size_min);
        rcd.ref.rwFirst = XsUtils::byteorder2host<uint16_t>(rcd.ref.rwFirst, little_endian);
        rcd.ref.rwLast = XsUtils::byteorder2host<uint16_t>(rcd.ref.rwLast, little_endian);
        (void)rcd.ref.colFirst;
        (void)rcd.ref.colLast;
        (void)rcd.cuse;
        rcd.formula.cce = XsUtils::byteorder2host<uint16_t>(rcd.formula.cce, little_endian);
        rcd.formula.rgce_rgcb.data = data + size_min;
        rcd.formula.rgce_rgcb.size = size - size_min;
        return size;
    }

    int read_Array(const uint8_t* data, uint32_t size, record_array_t& rcd) {
        uint32_t size_min = (uint8_t*)&rcd.formula.rgce_rgcb - (uint8_t*)&rcd;
        if (size < size_min) {
            return 0;
        }
        assert(size_min == 14);

        memcpy(&rcd, data, size_min);
        rcd.ref.rwFirst = XsUtils::byteorder2host<uint16_t>(rcd.ref.rwFirst, little_endian);
        rcd.ref.rwLast = XsUtils::byteorder2host<uint16_t>(rcd.ref.rwLast, little_endian);
        rcd.formula.cce = XsUtils::byteorder2host<uint16_t>(rcd.formula.cce, little_endian);
        rcd.formula.rgce_rgcb.data = data + size_min;
        rcd.formula.rgce_rgcb.size = size - size_min;
        return size;
    }

    int read_Formula(const uint8_t* data, uint32_t size, record_formula_t& rcd) {
        uint32_t size_min = (uint8_t*)&rcd.formula.rgce_rgcb - (uint8_t*)&rcd;
        if (size < size_min) {
            return 0;
        }
        assert(size_min == 22);

        memcpy(&rcd, data, size_min);
        rcd.row = XsUtils::byteorder2host<uint16_t>(rcd.row, little_endian);
        rcd.col = XsUtils::byteorder2host<uint16_t>(rcd.col, little_endian);
        rcd.ixfe = XsUtils::byteorder2host<uint16_t>(rcd.ixfe, little_endian);
        rcd.val.fExprO = XsUtils::byteorder2host<uint16_t>(rcd.val.fExprO, little_endian);
        rcd.flags = XsUtils::byteorder2host<uint16_t>(rcd.flags, little_endian);
        rcd.chn = XsUtils::byteorder2host<uint32_t>(rcd.chn, little_endian);
        rcd.formula.cce = XsUtils::byteorder2host<uint16_t>(rcd.formula.cce, little_endian);
        rcd.formula.rgce_rgcb.data = data + size_min;
        rcd.formula.rgce_rgcb.size = size - size_min;
        return size;
    }

    int read_uni_str_rgb(const uint8_t* data, uint32_t size, uint16_t cch, bool fHighByte, std::u16string& val) {
        if (fHighByte) {
            cch *= 2;
            cch = cch > size ? size / 2 * 2 : cch;
            for (uint32_t i = 0; i < cch; i += 2) {
                val += std::u16string(1, *(const char16_t*)(data + i));
            }
        }
        else {
            cch = cch > size ? size : cch;
            for (uint32_t i = 0; i < cch; i += 1) {
                char16_t c = (0x00FF & (*(const char*)(data + i)));
                val += std::u16string(1, c);
            }
        }
        return cch;
    }

    int read_XLUnicodeString(const uint8_t* data, uint32_t size, std::string& val_utf, bool replace_crlf) {
        if (size < 3) {
            return 0;
        }
        uint16_t cch = data[0] | (data[1] << 8);
        uint8_t flags = data[2];

        std::u16string u16str;
        read_uni_str_rgb(data + 3, size - 3, cch, (flags & 0x1), u16str);

        if (replace_crlf) {
            replace(u16str.begin(), u16str.end(), u'\r', u' ');
            replace(u16str.begin(), u16str.end(), u'\n', u' ');
        }

        val_utf = XsUtils::u16s2utf(u16str);
        if (!u16str.empty() && val_utf.empty()) {
            val_utf = "<Failed to decode UTF16LE string>";
        }
        return size;
    }

    int read_String(const cfb_ctx_t* ctx, const uint8_t* data, uint32_t size, std::string& rst, bool replace_crlf) {
        /*
        * XLUnicodeString
        *    Documented in Microsoft Office Excel97-2007Binary File Format (.xls) Specification
        *    Page 426: Unicode Strings in BIFF8
        */
        return read_XLUnicodeString(data, size, rst, replace_crlf);
    }
};

class xl_bin_utils {
public:
    static int copy_boundsheets(const std::vector<xl_bin_sheet_t>& boundsheets, xl_bin_boundsheet_t** out, uint32_t* count) {
        if (out == nullptr || count == nullptr) {
            return BQ::INVALID_ARG;
        }
        xl_bin_boundsheet_t* data = 0;
        uint32_t data_cnt = 0;
        if (boundsheets.size()) {
            data = (xl_bin_boundsheet_t*)malloc(sizeof(xl_bin_boundsheet_t) * boundsheets.size());
            data_cnt = boundsheets.size();
        }

        for (size_t i = 0; i < boundsheets.size(); i++) {
            memcpy((uint8_t*)(data + i), (void*)&boundsheets[i].bs, sizeof(xl_bin_boundsheet_t));
        }

        *out = data;
        *count = data_cnt;
        return BQ::OK;
    };
};


#endif //> __XLM_DATA_H__