#ifndef __OOXML_DATA_HPP__
#define __OOXML_DATA_HPP__

#include <string>
#include <vector>
#include <map>
#include <stack>
#include "./cfb_base.h"
#include "../utils/utils.h"

enum cell_ref_style {
    ref_style_unknown,
    ref_style_a1,
    ref_style_r1c1,
};

/*
* @brief 
*   the field sheet_id、row、col is one-based.
*/
typedef struct __st_xl_cell_coordinate {
    int32_t sheet_id{ 0 };
    int32_t row{ 0 };
    int32_t col{ 0 };
    int16_t col_span{ 0 };
    int16_t row_span{ 0 };
} xl_cellcoor_t;

/*
* @brief
*  the field sheet_id is one-based.
*/
typedef struct __st_xl_cell : xl_cellcoor_t {
    bool            is_func{ false };
    bool            is_deobfuscated{ false };
    char            t[8];
    std::string     c_r;
    std::string     f;
    std::string     v;
    __st_xl_cell() {
        memset(t, 0, sizeof(t));
    }
    __st_xl_cell(const xl_cellcoor_t& coor) {
        memset(t, 0, sizeof(t));
        sheet_id = coor.sheet_id;
        row = coor.row;
        col = coor.col;
        col_span = coor.col_span;
        row_span = coor.row_span;
    }
} xl_cell_t;

#define sheet_type_normal (0)
#define sheet_type_macro  (1)
/*
* @brief
*  reference
*/
typedef struct __st_xl_sheet {
    uint32_t ooxml_subid{ 0 };
    uint32_t sheet_id{ 0 };
    uint32_t sheet_type{ 0 };
    std::vector<xl_cell_t> cells;
} xl_sheet_t;

/*
* @brief \xl\workbook.xml 
*  reference : [](ECMA-376-Fifth-Edition-Part-1 --- 17.3.3.2 contentPart (Content Part))
*/
typedef struct __st_xl_defined_sheet {
    std::string id;
    std::string name;
    std::string rid;
} xl_defined_sheet_t;

/*
* @brief \xl\workbook.xml
*  reference :
*/
typedef struct __st_xl_defined_name {
    std::string   name;
    std::string   cell_raw;
    xl_cellcoor_t cell;
} xl_defined_name_t;

/*
* @brief 
*  reference : \xl\_rels\workbook.xml.rels
*/
typedef struct __st_xl_rels {
    std::string id;
    std::string target;
    std::string type;
} xl_rels_t;

/*
* @brief ignore formats
*  reference : \xl\sharedStrings.xml 
*              https://docs.microsoft.com/en-us/office/open-xml/working-with-the-shared-string-table  
*/
typedef struct __st_xl_sharedstring {
    uint32_t    id;
    std::string val;
} xl_sharedstr_t;

/*
* @brief
*  reference : https://docs.microsoft.com/en-us/dotnet/api/documentformat.openxml.spreadsheet.cellvalues?view=openxml-2.8.1
*/
typedef struct __st_xl_cell_value_type {
    static constexpr char boolean[] = "b";
    static constexpr char data[] = "d";
    static constexpr char error[] = "e";
    static constexpr char inline_str[] = "inlineStr";
    static constexpr char number[] = "n";
    static constexpr char shared_str[] = "s";
    static constexpr char str[] = "str";    
} xl_cellval_type;


/***************************************************/

/*
* @brief fields from the workbook and the workbook_rels
*/
typedef struct __st_xl_sheet_table {
    std::string name;
    std::string rid;
    std::string target;
    std::string type;
} xl_sheet_table_t;

/*
* @brief the information of an ooxml
*  reference
*/
typedef struct __st_xl_ecma_datas {
    cell_ref_style ref_mode;
    std::vector<std::string> templates;
    std::vector<xl_sheet_table_t> sheet_tables;
    std::vector<xl_defined_name_t> defined_names;
    std::vector<xl_sharedstr_t> sharedstr;
    std::vector<xl_sheet_t> norm_sheets;
    std::vector<xl_sheet_t> macro_sheets;
} xl_ecma_datas_t;


class xl_ecma_utils {
public:
    static uint32_t col_str2num(const char* col) {
        uint32_t rst = 0;
        for (int size = ucs::len(col), p = 0; p < size; p++) {
            char c = std::toupper(*(col + p)) - 'A' + 1;
            rst = rst * 26 + c;
        }
        return rst;
    };

    static bool only_digits(const char* p) {
        bool rst = true;
        for (uint32_t i = 0; *(p + i) != '\0'; i++) {
            if (!isdigit(*(p + i))) { rst = false; break; }
        }
        return rst;
    };

    /*
    * only for single cell referenced,
    *  eg. [workbook-name]sheet!$D$3、sheet!D3、sheet!B5:D8、EF1E4WF!C1R2、sheet!R[2]C[2]、sheet!R[-2]C
    * reference : [ECMA-376-Fifth-Edition-Part-1] 18.17.6.1 Cell Reference Style
    */
    static int cell_str2coor(const char* txt, const cell_ref_style* style, const xl_cellcoor_t* refer, std::string* sheet, xl_cellcoor_t* cell) {
        if (txt == nullptr || cell == nullptr) {
            return BQ::INVALID_ARG;
        }

        cell_ref_style ref_mode = style ? *style : ref_style_a1;
        if (ref_mode != ref_style_a1 && ref_mode != ref_style_r1c1) {
            return BQ::ERR_FORMAT;
        }

        int nret = BQ::OK;
        std::string sheet_name, col, row, col_span, row_span;
        bool col_rel = false, row_rel = false, col_span_rel = false, row_span_rel = false;;

        const char* pos = strchr(txt, '!');
        if (pos != nullptr) {
            sheet_name = std::string(txt, pos - txt);
            pos++;
        }
        else {
            pos = txt;
        }

        auto read_a1_style = [](const char* data, std::string& col, std::string& row, bool& c_rel, bool& r_rel) {
            if (data == nullptr) return (const char*)nullptr;

            col.clear(), row.clear();
            c_rel = r_rel = true;
            for (; *data != '\0' && isspace(*data); data++);

            if (*data == '$') { c_rel = false; data++; }
            for (; *data != '\0'; data++) {
                if (isalpha(*data)) { col += std::string(1, *data); }
                else { break; }
            }

            if (*data == '$') { r_rel = false; data++; }
            for (; *data != '\0'; data++) {
                if (isdigit(*data)) { row += std::string(1, *data); }
                else { break; }
            }

            return (row.empty() && col.empty()) ? nullptr : data;
        };

        auto read_r1c1_style = [](const char* data, std::string& col, std::string& row, bool& c_rel, bool& r_rel) {
            if (data == nullptr) return (const char*)nullptr;

            col.clear(), row.clear();
            c_rel = r_rel = false;
            for (; *data != '\0' && isspace(*data); data++);

            if (toupper(*data) == 'R') {
                if (*(++data) == '[') { r_rel = true; data++; }
                for (; *data != '\0'; data++) {
                    if (isdigit(*data) || *data == '+' || *data == '-') { row += std::string(1, *data); }
                    else { break; }
                }
                if (r_rel && *data == ']') { data++; }

                if (row.empty()) {
                    // RC[-2] refers to the cell two cols left and in the same row
                    r_rel = true;
                    row = "0";
                }
            }

            if (toupper(*data) == 'C') {
                if (*(++data) == '[') { c_rel = true; data++; }
                for (; *data != '\0'; data++) {
                    if (isdigit(*data) || *data == '+' || *data == '-') { col += std::string(1, *data); }
                    else { break; }
                }
                if (c_rel && *data == ']') { data++; }

                if (col.empty()) {
                    // R[-2]C refers to the cell two rows up and in the same column
                    c_rel = true;
                    col = "0";
                }
            }

            return (row.empty() && col.empty()) ? nullptr : data;
        };

        do {
            if (ref_mode != ref_style_a1) {
                break;
            }
            auto next_pos = read_a1_style(pos, col, row, col_rel, row_rel);
            if (next_pos == nullptr) {
                nret = BQ::ERR_FORMAT;
                break;
            }
            pos = next_pos;

            for (; *pos != '\0' && isspace(*pos); pos++);
            if (*pos == ':') {
                auto span_next_pos = read_a1_style(++pos, col_span, row_span, col_span_rel, row_span_rel);
                if (span_next_pos == nullptr) {
                    nret = BQ::ERR_FORMAT;
                    break;
                }
                pos = next_pos;
            }

            if (row.empty() || col.empty()) {
                nret = BQ::ERR_FORMAT;
                break;
            }

            if ((row_rel || col_rel || col_span_rel || row_span_rel) && refer == nullptr) {
                nret = BQ::INVALID_ARG;
                break;
            }

            if (nret == BQ::OK) {
                /*
                * NOTE: 
                *   the difference between relative A1 references and  absolute A1 references
                */
                cell->row = row.empty() ? 0 : atoi(row.c_str());
                cell->col = col.empty() ? 0 : xl_ecma_utils::col_str2num(col.c_str());
                cell->row_span = row_span.empty() ? 0 : atoi(row_span.c_str());
                cell->col_span = col_span.empty() ? 0 : xl_ecma_utils::col_str2num(col_span.c_str());
                if (cell->row_span) { cell->row_span -= cell->row; }
                if (cell->col_span) { cell->col_span -= cell->col; }
            }
        } while (false);

        do {
            if (ref_mode != ref_style_r1c1) {
                break;
            }
            auto next_pos = read_r1c1_style(pos, col, row, col_rel, row_rel);
            if (next_pos == nullptr) {
                nret = BQ::ERR_FORMAT;
                break;
            }
            pos = next_pos;

            if (row.empty() || col.empty()) {
                nret = BQ::ERR_FORMAT;
                break;
            }

            if ((row_rel || col_rel) && refer == nullptr) {
                nret = BQ::INVALID_ARG;
                break;
            }

            if (nret == BQ::OK) {
                cell->row = row.empty() ? 0 : atoi(row.c_str()) + (row_rel && refer ? refer->row : 0);
                cell->col = col.empty() ? 0 : xl_ecma_utils::col_str2num(col.c_str()) + (col_rel && refer ? refer->col : 0);
            }
        } while (false);

        for (; *pos != '\0'; pos++) {
            if (isalnum(*pos)) {
                nret = BQ::ERR_FORMAT;
                break;
            }
        }

        if (nullptr != sheet) {
            *sheet = sheet_name;
        }

        return nret;
    }

    static int deobfuscate_function(const std::vector<std::string>& fields, uint32_t func_start, uint32_t func_end, std::string& result) {
        std::stack<std::string> functions;
        for (uint32_t p = func_start; p <= func_end; p++) {
            functions.push(fields[p]);
            if (fields[p] != ")") {
                continue;
            }

            std::vector<std::string> func_args;
            for (functions.pop(); !functions.empty();) {
                auto arg = functions.top(); functions.pop();
                if (arg == "(") {
                    break;
                }
                func_args.push_back(arg);
            }

            std::string func_name;
            if (!functions.empty()) {
                func_name = functions.top(); functions.pop();
            }
            if (0 == ucs::icmp(func_name.c_str(), "char") && func_args.size() == 1 && xl_ecma_utils::only_digits(func_args[0].c_str())) {
                int char_arg = atoi(func_args[0].c_str());
                if (char_arg >= 0 && char_arg <= 255) {
                    func_name = std::string(1, char(char_arg));
                    functions.push(func_name);
                    continue;
                }
            }
            func_name += "(";
            for (uint32_t i = func_args.size(); i > 0; i--) {
                if (func_args[i - 1] == "&") continue;
                func_name += func_args[i - 1];
            }
            func_name += ")";
            functions.push(func_name);
        }

        result.clear();
        for (; !functions.empty();) {
            auto arg = functions.top(); functions.pop();
            result = arg + result;
        }
        return BQ::OK;
    };

    static int split_cell_formula(const char* formula, std::vector<std::string>& splited) {
        if (nullptr == formula) {
            return BQ::INVALID_ARG;
        }

        const std::string seporators = "(),=*^/+-&";
        int nret = BQ::OK;
        auto splited_in_size = splited.size();

        for (uint32_t i = 0, s = 0, size = ucs::len(formula); i <= size; ) {
            if (i == size) {
                if (size > s) {
                    splited.push_back(std::string(formula + s, size - s));
                }
                break;
            }

            char c = *(formula + i);
            if (c == '"') {
                uint32_t cnt_s = 1;
                for (; i + cnt_s < size && *(formula + i + cnt_s) == '"'; cnt_s++);
                if (cnt_s % 2 == 0) {
                    i += cnt_s;
                    continue;
                }

                auto pos = i + cnt_s;
                for (uint32_t cnt_e = 0; pos < size; pos++) {
                    if (*(formula + pos) == '"') {
                        cnt_e++;
                    }
                    else if (cnt_e && cnt_e % 2 == 0) {
                        cnt_e = 0;
                    }
                    else if (cnt_e && cnt_e % 2 != 0) {
                        break;
                    }
                }

                if (i > s) {
                    splited.push_back(std::string(formula + s, i - s));
                }

                splited.push_back(std::string(formula + i, pos - i));
                i = pos;
                s = i;
                continue;
            }

            if (c == '(') {
                const char l_char = '(';
                const char r_char = ')';

                uint32_t r_pos = i;
                for (uint32_t l_cnt = 0, r_cnt = 0; r_pos < size; r_pos++) {
                    if (*(formula + r_pos) == l_char) {
                        l_cnt++;
                    }
                    else if (*(formula + r_pos) == r_char) {
                        r_cnt++;
                    }
                    if (l_cnt == r_cnt) {
                        break;
                    }
                }
                if (r_pos >= size) {
                    i = size;
                    continue;
                }

                if (i > s) {
                    splited.push_back(std::string(formula + s, i - s));
                }

                splited.push_back(std::string(1, c));
                if (i + 1 < r_pos) {
                    std::string args = std::string(formula + i + 1, r_pos - i - 1);
                    split_cell_formula(args.c_str(), splited);
                }
                splited.push_back(std::string(1, r_char));

                i = r_pos + 1;
                s = i;
                continue;
            }

            if (seporators.find(c) != std::string::npos) {
                if (i > s) {
                    splited.push_back(std::string(formula + s, i - s));
                }
                splited.push_back(std::string(1, c));
                s = ++i;
                continue;
            }

            i++;
        }

#ifdef _DEBUG
        do {
            std::string merged;
            for (auto i = splited_in_size; i < splited.size(); i++) merged += splited[i];
            assert(0 == ucs::icmp(merged.c_str(), formula));
        } while (false);
#endif
        return nret;
    }
};



#endif // __OOXML_DATA_HPP__