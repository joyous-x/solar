#pragma once
#include <vector>
#include <map>
#include "ms_vba_pcode.h"
#include "../utils/utils.h"

typedef struct __st_pcode_table {
    uint32_t offset;
    uint32_t size;
    const uint8_t* data;
} pcode_table_t;

typedef struct __st_pcode_datas {
    bool     little_endian;
    bool     is64bit;
    bool     verbose;
    const char** identifiers;
    uint32_t identifiers_size;
    uint16_t magic; /* should be 0xCAFE */
    uint16_t vba_ver;
    uint16_t vba_line_cnt;
    uint32_t vba_line_start;
    pcode_table_t declare;
    pcode_table_t indirect;
    pcode_table_t object;
} pcode_datas_t;

typedef struct __st_pcode_line {
    uint16_t bytes;
    uint16_t reserved;
    uint32_t offset;
} pcode_line_t;

typedef struct __st_opcode_item {
    uint16_t id;
    const char* mnem;
    const char* args[6];
    bool varg;
} opcode_item_t;

/* VBA7 opcodes; VBA3, VBA5and VBA6 will be upconverted to these. */
const opcode_item_t opcodes[] = {
    opcode_item_t{ 0, "Imp", {0}, false },
    opcode_item_t{ 1, "Eqv", {0}, false },
    opcode_item_t{  2, "Xor", {0}, false },
    opcode_item_t{  3, "Or", {0}, false },
    opcode_item_t{  4, "And", {0}, false },
    opcode_item_t{  5, "Eq", {0}, false },
    opcode_item_t{  6, "Ne", {0}, false },
    opcode_item_t{  7, "Le", {0}, false },
    opcode_item_t{  8, "Ge", {0}, false },
    opcode_item_t{  9, "Lt", {0}, false },
    opcode_item_t{ 10, "Gt", {0}, false },
    opcode_item_t{ 11, "Add", {0}, false },
    opcode_item_t{ 12, "Sub", {0}, false },
    opcode_item_t{ 13, "Mod", {0}, false },
    opcode_item_t{ 14, "IDiv", {0}, false },
    opcode_item_t{ 15, "Mul", {0}, false },
    opcode_item_t{ 16, "Div", {0}, false },
    opcode_item_t{ 17, "Concat", {0}, false },
    opcode_item_t{ 18, "Like", {0}, false },
    opcode_item_t{ 19, "Pwr", {0}, false },
    opcode_item_t{ 20, "Is", {0}, false },
    opcode_item_t{ 21, "Not", {0}, false },
    opcode_item_t{ 22, "UMi", {0}, false },
    opcode_item_t{ 23, "FnAbs", {0}, false },
    opcode_item_t{ 24, "FnFix", {0}, false },
    opcode_item_t{ 25, "FnInt", {0}, false },
    opcode_item_t{ 26, "FnSgn", {0}, false },
    opcode_item_t{ 27, "FnLen", {0}, false },
    opcode_item_t{ 28, "FnLenB", {0}, false },
    opcode_item_t{ 29, "Paren", {0}, false },
    opcode_item_t{ 30, "Sharp", {0}, false },
    opcode_item_t{ 31, "LdLHS", {"name", 0}, false },
    opcode_item_t{ 32, "Ld", {"name", 0}, false },
    opcode_item_t{ 33, "MemLd", {"name", 0}, false },
    opcode_item_t{ 34, "DictLd", {"name", 0}, false },
    opcode_item_t{ 35, "IndexLd", {"0x", 0}, false },
    opcode_item_t{ 36, "ArgsLd", {"name", "0x", 0}, false },
    opcode_item_t{ 37, "ArgsMemLd", {"name", "0x", 0}, false },
    opcode_item_t{ 38, "ArgsDictLd", {"name", "0x", 0}, false },
    opcode_item_t{ 39, "St", {"name", 0}, false },
    opcode_item_t{ 40, "MemSt", {"name", 0}, false },
    opcode_item_t{ 41, "DictSt", {"name", 0}, false },
    opcode_item_t{ 42, "IndexSt", {"0x", 0}, false },
    opcode_item_t{ 43, "ArgsSt", {"name", "0x", 0}, false },
    opcode_item_t{ 44, "ArgsMemSt", {"name", "0x", 0}, false },
    opcode_item_t{ 45, "ArgsDictSt", {"name", "0x", 0}, false },
    opcode_item_t{ 46, "Set", {"name", 0}, false },
    opcode_item_t{ 47, "Memset", {"name", 0}, false },
    opcode_item_t{ 48, "Dictset", {"name", 0}, false },
    opcode_item_t{ 49, "Indexset", {"0x", 0}, false },
    opcode_item_t{ 50, "ArgsSet", {"name", "0x", 0}, false },
    opcode_item_t{ 51, "ArgsMemSet", {"name", "0x", 0}, false },
    opcode_item_t{ 52, "ArgsDictSet", {"name", "0x", 0}, false },
    opcode_item_t{ 53, "MemLdWith", {"name", 0}, false },
    opcode_item_t{ 54, "DictLdWith", {"name", 0}, false },
    opcode_item_t{ 55, "ArgsMemLdWith", {"name", "0x", 0}, false },
    opcode_item_t{ 56, "ArgsDictLdWith", {"name", "0x", 0}, false },
    opcode_item_t{ 57, "MemStWith", {"name", 0}, false },
    opcode_item_t{ 58, "DictStWith", {"name", 0}, false },
    opcode_item_t{ 59, "ArgsMemStWith", {"name", "0x", 0}, false },
    opcode_item_t{ 60, "ArgsDictStWith", {"name", "0x", 0}, false },
    opcode_item_t{ 61, "MemSetWith", {"name", 0}, false },
    opcode_item_t{ 62, "DictSetWith", {"name", 0}, false },
    opcode_item_t{ 63, "ArgsMemSetWith", {"name", "0x", 0}, false },
    opcode_item_t{ 64, "ArgsDictSetWith", {"name", "0x", 0}, false },
    opcode_item_t{ 65, "ArgsCall", {"name", "0x", 0}, false },
    opcode_item_t{ 66, "ArgsMemCall", {"name", "0x", 0}, false },
    opcode_item_t{ 67, "ArgsMemCallWith", {"name", "0x", 0}, false },
    opcode_item_t{ 68, "ArgsArray", {"name", "0x", 0}, false },
    opcode_item_t{ 69, "Assert", {0}, false },
    opcode_item_t{ 70, "BoS", {"0x", 0}, false },
    opcode_item_t{ 71, "BoSImplicit", {0}, false },
    opcode_item_t{ 72, "BoL", {0}, false },
    opcode_item_t{ 73, "LdAddressOf", {"name", 0}, false },
    opcode_item_t{ 74, "MemAddressOf", {"name", 0}, false },
    opcode_item_t{ 75, "Case", {0}, false },
    opcode_item_t{ 76, "CaseTo", {0}, false },
    opcode_item_t{ 77, "CaseGt", {0}, false },
    opcode_item_t{ 78, "CaseLt", {0}, false },
    opcode_item_t{ 79, "CaseGe", {0}, false },
    opcode_item_t{ 80, "CaseLe", {0}, false },
    opcode_item_t{ 81, "CaseNe", {0}, false },
    opcode_item_t{ 82, "CaseEq", {0}, false },
    opcode_item_t{ 83, "CaseElse", {0}, false },
    opcode_item_t{ 84, "CaseDone", {0}, false },
    opcode_item_t{ 85, "Circle", {"0x", 0}, false },
    opcode_item_t{ 86, "Close", {"0x", 0}, false },
    opcode_item_t{ 87, "CloseAll", {0}, false },
    opcode_item_t{ 88, "Coerce", {0}, false },
    opcode_item_t{ 89, "CoerceVar", {0}, false },
    opcode_item_t{ 90, "Context", {"context_", 0}, false },
    opcode_item_t{ 91, "Debug", {0}, false },
    opcode_item_t{ 92, "DefType", {"0x", "0x", 0}, false },
    opcode_item_t{ 93, "Dim", {0}, false },
    opcode_item_t{ 94, "DimImplicit", {0}, false },
    opcode_item_t{ 95, "Do", {0}, false },
    opcode_item_t{ 96, "DoEvents", {0}, false },
    opcode_item_t{ 97, "DoUnitil", {0}, false },
    opcode_item_t{ 98, "DoWhile", {0}, false },
    opcode_item_t{ 99, "Else", {0}, false },
    opcode_item_t{100, "ElseBlock", {0}, false },
    opcode_item_t{101, "ElseIfBlock", {0}, false },
    opcode_item_t{102, "ElseIfTypeBlock", {"imp_", 0}, false },
    opcode_item_t{103, "End", {0}, false },
    opcode_item_t{104, "EndContext", {0}, false },
    opcode_item_t{105, "EndFunc", {0}, false },
    opcode_item_t{106, "EndIf", {0}, false },
    opcode_item_t{107, "EndIfBlock", {0}, false },
    opcode_item_t{108, "EndImmediate", {0}, false },
    opcode_item_t{109, "EndProp", {0}, false },
    opcode_item_t{110, "EndSelect", {0}, false },
    opcode_item_t{111, "EndSub", {0}, false },
    opcode_item_t{112, "EndType", {0}, false },
    opcode_item_t{113, "EndWith", {0}, false },
    opcode_item_t{114, "Erase", {"0x", 0}, false },
    opcode_item_t{115, "Error", {0}, false },
    opcode_item_t{116, "EventDecl", {"func_", 0}, false },
    opcode_item_t{117, "RaiseEvent", {"name", "0x", 0}, false },
    opcode_item_t{118, "ArgsMemRaiseEvent", {"name", "0x", 0}, false },
    opcode_item_t{119, "ArgsMemRaiseEventWith", {"name", "0x", 0}, false },
    opcode_item_t{120, "ExitDo", {0}, false },
    opcode_item_t{121, "ExitFor", {0}, false },
    opcode_item_t{122, "ExitFunc", {0}, false },
    opcode_item_t{123, "ExitProp", {0}, false },
    opcode_item_t{124, "ExitSub", {0}, false },
    opcode_item_t{125, "FnCurDir", {0}, false },
    opcode_item_t{126, "FnDir", {0}, false },
    opcode_item_t{127, "Empty0", {0}, false },
    opcode_item_t{128, "Empty1", {0}, false },
    opcode_item_t{129, "FnError", {0}, false },
    opcode_item_t{130, "FnFormat", {0}, false },
    opcode_item_t{131, "FnFreeFile", {0}, false },
    opcode_item_t{132, "FnInStr", {0}, false },
    opcode_item_t{133, "FnInStr3", {0}, false },
    opcode_item_t{134, "FnInStr4", {0}, false },
    opcode_item_t{135, "FnInStrB", {0}, false },
    opcode_item_t{136, "FnInStrB3", {0}, false },
    opcode_item_t{137, "FnInStrB4", {0}, false },
    opcode_item_t{138, "FnLBound", {"0x", 0}, false },
    opcode_item_t{139, "FnMid", {0}, false },
    opcode_item_t{140, "FnMidB", {0}, false },
    opcode_item_t{141, "FnStrComp", {0}, false },
    opcode_item_t{142, "FnStrComp3", {0}, false },
    opcode_item_t{143, "FnStringVar", {0}, false },
    opcode_item_t{144, "FnStringStr", {0}, false },
    opcode_item_t{145, "FnUBound", {"0x", 0}, false },
    opcode_item_t{146, "For", {0}, false },
    opcode_item_t{147, "ForEach", {0}, false },
    opcode_item_t{148, "ForEachAs", {"imp_", 0}, false },
    opcode_item_t{149, "ForStep", {0}, false },
    opcode_item_t{150, "FuncDefn", {"func_", 0}, false },
    opcode_item_t{151, "FuncDefnSave", {"func_", 0}, false },
    opcode_item_t{152, "GetRec", {0}, false },
    opcode_item_t{153, "GoSub", {"name", 0}, false },
    opcode_item_t{154, "GoTo", {"name", 0}, false },
    opcode_item_t{155, "If", {0}, false },
    opcode_item_t{156, "IfBlock", {0}, false },
    opcode_item_t{157, "TypeOf", {"imp_", 0}, false },
    opcode_item_t{158, "IfTypeBlock", {"imp_", 0}, false },
    opcode_item_t{159, "Implements", {"0x", "0x", "0x", "0x", 0}, false },
    opcode_item_t{160, "Input", {0}, false },
    opcode_item_t{161, "InputDone", {0}, false },
    opcode_item_t{162, "InputItem", {0}, false },
    opcode_item_t{163, "Label", {"name", 0}, false },
    opcode_item_t{164, "Let", {0}, false },
    opcode_item_t{165, "Line", {"0x", 0}, false },
    opcode_item_t{166, "LineCont", {0}, true },
    opcode_item_t{167, "LineInput", {0}, false },
    opcode_item_t{168, "LineNum", {"name", 0}, false },
    opcode_item_t{169, "LitCy", {"0x", "0x", "0x", "0x", 0}, false },
    opcode_item_t{170, "LitDate", {"0x", "0x", "0x", "0x", 0}, false },
    opcode_item_t{171, "LitDefault", {0}, false },
    opcode_item_t{172, "LitDI2", {"0x", 0}, false },
    opcode_item_t{173, "LitDI4", {"0x", "0x", 0}, false },
    opcode_item_t{174, "LitDI8", {"0x", "0x", "0x", "0x", 0}, false },
    opcode_item_t{175, "LitHI2", {"0x", 0}, false },
    opcode_item_t{176, "LitHI4", {"0x", "0x", 0}, false },
    opcode_item_t{177, "LitHI8", {"0x", "0x", "0x", "0x", 0}, false },
    opcode_item_t{178, "LitNothing", {0}, false },
    opcode_item_t{179, "LitOI2", {"0x", 0}, false },
    opcode_item_t{180, "LitOI4", {"0x", "0x", 0}, false },
    opcode_item_t{181, "LitOI8", {"0x", "0x", "0x", "0x", 0}, false },
    opcode_item_t{182, "LitR4", {"0x", "0x", 0}, false },
    opcode_item_t{183, "LitR8", {"0x", "0x", "0x", "0x", 0}, false },
    opcode_item_t{184, "LitSmallI2", {0}, false },
    opcode_item_t{185, "LitStr", {0}, true },
    opcode_item_t{186, "LitVarSpecial", {0}, false },
    opcode_item_t{187, "Lock", {0}, false },
    opcode_item_t{188, "Loop", {0}, false },
    opcode_item_t{189, "LoopUntil", {0}, false },
    opcode_item_t{190, "LoopWhile", {0}, false },
    opcode_item_t{191, "LSet", {0}, false },
    opcode_item_t{192, "Me", {0}, false },
    opcode_item_t{193, "MeImplicit", {0}, false },
    opcode_item_t{194, "MemRedim", {"name", "0x", "type_", 0}, false },
    opcode_item_t{195, "MemRedimWith", {"name", "0x", "type_", 0}, false },
    opcode_item_t{196, "MemRedimAs", {"name", "0x", "type_", 0}, false },
    opcode_item_t{197, "MemRedimAsWith", {"name", "0x", "type_", 0}, false },
    opcode_item_t{198, "Mid", {0}, false },
    opcode_item_t{199, "MidB", {0}, false },
    opcode_item_t{200, "Name", {0}, false },
    opcode_item_t{201, "New", {"imp_", 0}, false },
    opcode_item_t{202, "Next", {0}, false },
    opcode_item_t{203, "NextVar", {0}, false },
    opcode_item_t{204, "OnError", {"name", 0}, false },
    opcode_item_t{205, "OnGosub", {0}, true },
    opcode_item_t{206, "OnGoto", {0}, true },
    opcode_item_t{207, "Open", {"0x", 0}, false },
    opcode_item_t{208, "Option", {0}, false },
    opcode_item_t{209, "OptionBase", {0}, false },
    opcode_item_t{210, "ParamByVal", {0}, false },
    opcode_item_t{211, "ParamOmitted", {0}, false },
    opcode_item_t{212, "ParamNamed", {"name", 0}, false },
    opcode_item_t{213, "PrintChan", {0}, false },
    opcode_item_t{214, "PrintComma", {0}, false },
    opcode_item_t{215, "PrintEoS", {0}, false },
    opcode_item_t{216, "PrintItemComma", {0}, false },
    opcode_item_t{217, "PrintItemNL", {0}, false },
    opcode_item_t{218, "PrintItemSemi", {0}, false },
    opcode_item_t{219, "PrintNL", {0}, false },
    opcode_item_t{220, "PrintObj", {0}, false },
    opcode_item_t{221, "PrintSemi", {0}, false },
    opcode_item_t{222, "PrintSpc", {0}, false },
    opcode_item_t{223, "PrintTab", {0}, false },
    opcode_item_t{224, "PrintTabComma", {0}, false },
    opcode_item_t{225, "PSet", {"0x", 0}, false },
    opcode_item_t{226, "PutRec", {0}, false },
    opcode_item_t{227, "QuoteRem", {"0x", 0}, true },
    opcode_item_t{228, "Redim", {"name", "0x", "type_", 0}, false },
    opcode_item_t{229, "RedimAs", {"name", "0x", "type_", 0}, false },
    opcode_item_t{230, "Reparse", {0}, true },
    opcode_item_t{231, "Rem", {0}, true },
    opcode_item_t{232, "Resume", {"name", 0}, false },
    opcode_item_t{233, "Return", {0}, false },
    opcode_item_t{234, "RSet", {0}, false },
    opcode_item_t{235, "Scale", {"0x", 0}, false },
    opcode_item_t{236, "Seek", {0}, false },
    opcode_item_t{237, "SelectCase", {0}, false },
    opcode_item_t{238, "SelectIs", {"imp_", 0}, false },
    opcode_item_t{239, "SelectType", {0}, false },
    opcode_item_t{240, "SetStmt", {0}, false },
    opcode_item_t{241, "Stack", {"0x", "0x", 0}, false },
    opcode_item_t{242, "Stop", {0}, false },
    opcode_item_t{243, "Type", {"rec_", 0}, false },
    opcode_item_t{244, "Unlock", {0}, false },
    opcode_item_t{245, "VarDefn", {"var_", 0}, false },
    opcode_item_t{246, "Wend", {0}, false },
    opcode_item_t{247, "While", {0}, false },
    opcode_item_t{248, "With", {0}, false },
    opcode_item_t{249, "WriteChan", {0}, false },
    opcode_item_t{250, "ConstFuncExpr", {0}, false },
    opcode_item_t{251, "LbConst", {"name", 0}, false },
    opcode_item_t{252, "LbIf", {0}, false },
    opcode_item_t{253, "LbElse", {0}, false },
    opcode_item_t{254, "LbElseIf", {0}, false },
    opcode_item_t{255, "LbEndIf", {0}, false },
    opcode_item_t{256, "LbMark", {0}, false },
    opcode_item_t{257, "EndForVariable", {0}, false },
    opcode_item_t{258, "StartForVariable", {0}, false },
    opcode_item_t{259, "NewRedim", {0}, false },
    opcode_item_t{260, "StartWithExpr", {0}, false },
    opcode_item_t{261, "SetOrSt", {"name", 0}, false },
    opcode_item_t{262, "EndEnum", {0}, false },
    opcode_item_t{263, "Illegal", {0}, false },
};

const char* pcode_types[] = { "Var", "?", "Int", "Lng", "Sng", "Dbl", "Cur", "Date", "Str", "Obj", "Err", "Bool", "Var" };
const char* pcode_specials[] = { "False", "True", "Null", "Empty" };
const char* pcode_options[] = { "Base 0", "Base 1", "Compare Text", "Compare Binary", "Explicit", "Private Module" };

#define stream_fmt_0nX(n) std::hex<<std::uppercase<<std::setw(n)<<std::setfill('0')

int vector2strlist(const std::vector<std::string>& in, string_list_t* out) {
    if (in.size() && out == nullptr) {
        return BQ::INVALID_ARG;
    }

    if (in.size() == 0) {
        if (out) { out->release(); }
        return BQ::OK;
    }

    uint32_t str_start = in.size() * sizeof(char*);
    uint8_t* data = nullptr;
    uint32_t size = str_start;
    for (auto& iter : in) { size += iter.size() + 1; }

    data = (uint8_t*)malloc(size);
    if (data == nullptr) {
        return BQ::ERR_MALLOC;
    }

    memset(data, 0, size);
    for (uint32_t i = 0, pos = str_start; i < in.size(); i++) {
        *((char**)data + i) = (char*)(data + pos);
        memcpy(data + pos, in[i].c_str(), in[i].size());
        pos += in[i].size() + 1;
    }
    out->buffer = data;
    out->bytes = size;
    out->count = in.size();
    out->items = (const char**)data;
    return BQ::OK;
}

auto get_uint16 = [](const uint8_t* data, uint32_t size, uint32_t offset, bool is_little_endian, bool* ok = 0) -> uint16_t {
    bool bad = (data == nullptr || size < offset + 2 || offset + 2 < offset);
    if (ok) { *ok = !bad; }
    return bad ? 0 : XsUtils::byteorder2host<uint16_t>(*(const uint16_t*)(data + offset), is_little_endian);
};

auto get_uint32 = [](const uint8_t* data, uint32_t size, uint32_t offset, bool is_little_endian, bool* ok = 0) -> uint32_t {
    bool bad = (data == nullptr || size < offset + 4 || offset + 4 < offset);
    if (ok) { *ok = !bad; }
    return bad ? 0 : XsUtils::byteorder2host<uint32_t>(*(const uint32_t*)(data + offset), is_little_endian);
};

template<typename T>
uint32_t skip_structure(const uint8_t* data, uint32_t size, uint32_t offset, bool little_endian, uint32_t elem_size, bool check_ffff) {
    T len = 0;

    bool get_ok = false;
    if (sizeof(T) == 4) {
        len = get_uint32(data, size, offset, little_endian, &get_ok);
    }
    else if (sizeof(T) == 2) {
        len = get_uint16(data, size, offset, little_endian, &get_ok);
    }

    if (!get_ok) {
        return -1;
    }

    offset += sizeof(len);
    if (!(check_ffff && len == T(-1))) {
        offset += len * elem_size;
    }
    return offset;
}

uint16_t translate_opcode(uint16_t opcode, uint16_t vba_ver, bool is64bit) {
    if (vba_ver == 3) {
        if (opcode <= 67) {

        }
        else if (opcode <= 70) {
            opcode += 2;
        }
        else if (opcode <= 111) {
            opcode += 4;
        }
        else if (opcode <= 150) {
            opcode += 8;
        }
        else if (opcode <= 164) {
            opcode += 9;
        }
        else if (opcode <= 166) {
            opcode += 10;
        }
        else if (opcode <= 169) {
            opcode += 11;
        }
        else if (opcode <= 238) {
            opcode += 12;
        }
        else {
            opcode += 24;
        }
    }
    else if (vba_ver == 5) {
        if (opcode <= 68) {

        }
        else if (opcode <= 71) {
            opcode += 1;
        }
        else if (opcode <= 112) {
            opcode += 3;
        }
        else if (opcode <= 151) {
            opcode += 7;
        }
        else if (opcode <= 165) {
            opcode += 8;
        }
        else if (opcode <= 167) {
            opcode += 9;
        }
        else if (opcode <= 170) {
            opcode += 10;
        }
        else {
            opcode += 11;
        }
    }
    else if (!is64bit) {
        if (opcode <= 173) {

        }
        else if (opcode <= 175) {
            opcode += 1;
        }
        else if (opcode <= 178) {
            opcode += 2;
        }
        else {
            opcode += 3;
        }
    }

    return opcode;
}

std::string get_id(uint16_t code, const char** identifiers, uint32_t identifiers_size, uint16_t vba_ver, bool is64bit) {
    constexpr const char* internal_names[] = {
        "<crash>", "0", "Abs", "Access", "AddressOf", "Alias", "And", "Any",
        "Append", "Array", "As", "Assert", "B", "Base", "BF", "Binary",
        "Boolean", "ByRef", "Byte", "ByVal", "Call", "Case", "CBool", "CByte",
        "CCur", "CDate", "CDec", "CDbl", "CDecl", "ChDir", "CInt", "Circle",
        "CLng", "Close", "Compare", "Const", "CSng", "CStr", "CurDir", "CurDir$",
        "CVar", "CVDate", "CVErr", "Currency", "Database", "Date", "Date$", "Debug",
        "Decimal", "Declare", "DefBool", "DefByte", "DefCur", "DefDate", "DefDec", "DefDbl",
        "DefInt", "DefLng", "DefObj", "DefSng", "DefStr", "DefVar", "Dim", "Dir",
        "Dir$", "Do", "DoEvents", "Double", "Each", "Else", "ElseIf", "Empty",
        "End", "EndIf", "Enum", "Eqv", "Erase", "Error", "Error$", "Event",
        "WithEvents", "Explicit", "F", "False", "Fix", "For", "Format",
        "Format$", "FreeFile", "Friend", "Function", "Get", "Global", "Go", "GoSub",
        "Goto", "If", "Imp", "Implements", "In", "Input", "Input$", "InputB",
        "InputB", "InStr", "InputB$", "Int", "InStrB", "Is", "Integer", "Left",
        "LBound", "LenB", "Len", "Lib", "Let", "Line", "Like", "Load",
        "Local", "Lock", "Long", "Loop", "LSet", "Me", "Mid", "Mid$",
        "MidB", "MidB$", "Mod", "Module", "Name", "New", "Next", "Not",
        "Nothing", "Null", "Object", "On", "Open", "Option", "Optional", "Or",
        "Output", "ParamArray", "Preserve", "Print", "Private", "Property", "PSet", "Public",
        "Put", "RaiseEvent", "Random", "Randomize", "Read", "ReDim", "Rem", "Resume",
        "Return", "RGB", "RSet", "Scale", "Seek", "Select", "Set", "Sgn",
        "Shared", "Single", "Spc", "Static", "Step", "Stop", "StrComp", "String",
        "String$", "Sub", "Tab", "Text", "Then", "To", "True", "Type",
        "TypeOf", "UBound", "Unload", "Unlock", "Unknown", "Until", "Variant", "WEnd",
        "While", "Width", "With", "Write", "Xor", "#Const", "#Else", "#ElseIf",
        "#End", "#If", "Attribute", "VB_Base", "VB_Control", "VB_Creatable", "VB_Customizable", "VB_Description",
        "VB_Exposed", "VB_Ext_Key", "VB_HelpID", "VB_Invoke_Func", "VB_Invoke_Property", "VB_Invoke_PropertyPut", "VB_Invoke_PropertyPutRef", "VB_MemberFlags",
        "VB_Name", "VB_PredecraredID", "VB_ProcData", "VB_TemplateDerived", "VB_VarDescription", "VB_VarHelpID", "VB_VarMemberFlags", "VB_VarProcData",
        "VB_UserMemID", "VB_VarUserMemID", "VB_GlobalNameSpace", ",", ".", "\"", "_", "!",
        "#", "&", "'", "(", ")", "*", "+", "-", " /", ":", ";", "<", "<=", "<>", "=", "=<",
        "=>", ">", "><", ">=", "?", "\\", "^", ":="
    };
    constexpr uint32_t internal_names_size = sizeof(internal_names) / sizeof(internal_names[0]);

    std::string result;
    uint16_t id_code_raw = code;

    code >>= 1;
    if (code >= 0x100) {
        code -= 0x100;
        if (vba_ver >= 7) {
            code -= 4;
            code -= (is64bit ? 3 : 0);
            code -= (code > 0xBE ? 1 : 0);
        }
        if (identifiers && code < identifiers_size) {
            result = identifiers[code];
        }
    }
    else {
        if (vba_ver >= 7 && code >= 0xC3) {
            code -= 1;
        }
        if (code < internal_names_size) {
            result = internal_names[code];
        }
    }

    if (result.empty()) {
        std::stringstream temp;
        temp << "id_" << stream_fmt_0nX(4) << id_code_raw;
        result = temp.str();
    }
    return result;
}

std::string get_name(const pcode_table_t* table, uint16_t offset, const char** identifiers, uint32_t identifiers_cnt, uint16_t vba_ver, bool little_endian, bool is64bit) {
    if (table == nullptr || table->data == nullptr || table->size <= offset || table->size <= offset+2) {
        return "";
    }
    uint16_t object_id = get_uint16(table->data, table->size, offset, little_endian);
    return get_id(object_id, identifiers, identifiers_cnt, vba_ver, is64bit);
}

std::string get_typename(uint8_t type_id) {
    constexpr const char* dim_types[] = {
        "", "Null", "Integer", "Long", "Single", "Double", "Currency", "Date", "String", "Object", "Error", "Boolean", "Variant", "", "Decimal", "", "", "Byte"
    };
    constexpr auto dim_types_size = sizeof(dim_types) / sizeof(dim_types[0]);

    auto flags = type_id & 0xE0;
    type_id &= ~0xE0;

    std::string type_name;
    if (type_id < dim_types_size) {
        type_name = dim_types[type_id];
    }
    if (flags & 0x80) {
        type_name += "Ptr";
    }
    return type_name;
}

std::string disasm_var_arg(const pcode_datas_t* pcode, const uint8_t* data, uint32_t size, uint32_t arg_off, uint16_t arg_len, const char* mnem) {
    if (pcode == nullptr || data == nullptr || arg_off == 0 || arg_off + arg_len > size) {
        return "";
    }
    bool little_endian = pcode->little_endian;

    std::stringstream result;
    result << "0x" << stream_fmt_0nX(4) << arg_len << " ";

    if (0 == ucs::cmp(mnem, "LitStr")
        || 0 == ucs::cmp(mnem, "QuoteRem")
        || 0 == ucs::cmp(mnem, "Rem")
        || 0 == ucs::cmp(mnem, "Reparse")) {
        result << "\"" << std::string((const char*)data + arg_off, arg_len) << "\"";
    }
    else if (0 == ucs::cmp(mnem, "OnGosub") || 0 == ucs::cmp(mnem, "OnGoto")) {
        for (uint16_t i = 0, word = 0; i < arg_len / 2; i++) {
            word = get_uint16(data, size, arg_off + i * 2, little_endian);
            result << ", " << get_id(word, pcode->identifiers, pcode->identifiers_size, pcode->vba_ver, pcode->is64bit) << " ";
        }
    }
    else {
        for (uint16_t i = 0; i < arg_len; i++) {
            result << (i ? " " : "") << stream_fmt_0nX(2) << (uint32_t) * (data + arg_off + i);
        }
    }
    return result.str();
}

std::string disasm_name(const pcode_datas_t* pcode, uint16_t code, uint16_t optype, const char* mnem) {
    if (pcode == nullptr) {
        return "";
    }
    const char* var_types[] = { "", "?", "%'", "&", "!", "#", "@", "?", "$", "?", "?", "?", "?", "?" };
    
    auto identifiers = pcode->identifiers;
    auto identifiers_size = pcode->identifiers_size;
    std::string var_name = get_id(code, identifiers, identifiers_size, pcode->vba_ver, pcode->is64bit);
    
    std::string str_type;
    if (optype < sizeof(var_types) / sizeof(var_types[0])) {
        str_type = var_types[optype];
    }
    else if (optype == 32) {
        var_name = "[" + var_name + "]";
    }

    if (nullptr == mnem) {
    }
    else if (0 == ucs::cmp(mnem, "OnError")) {
        str_type = "";
        if (optype == 1) {
            var_name = "(Resume Next)";
        }
        else if (optype == 2) {
            var_name = "(GoTo 0)";
        }
    }
    else if (0 == ucs::cmp(mnem, "Resume")) {
        str_type = "";
        if (optype == 1) {
            var_name = "(Next)";
        }
        else if (optype != 0) {
            var_name = "";
        }
    }
    return var_name + str_type + " ";
}

std::string disasm_imp(const pcode_datas_t* pcode, uint16_t code, const char* arg, const char* mnem) {
    if (pcode == nullptr || nullptr == arg) {
        return "";
    }

    std::stringstream imp_name;
    if (nullptr == mnem || ucs::cmp(mnem, "Open")) {
        if (0 == ucs::cmp(arg, "imp_") && pcode->object.size >= uint32_t(code + 8)) {
            auto idens = pcode->identifiers;
            auto idens_size = pcode->identifiers_size;
            imp_name << get_name(&pcode->object, code + 6, idens, idens_size
                , pcode->vba_ver, pcode->little_endian, pcode->is64bit);
        }
        else {
            imp_name << std::string(arg) << stream_fmt_0nX(4) << code << " ";
        }
    }
    else {
        const char* access_mode[] = { "Read", "Write", "Read Write" };
        const char* lock_mode[] = { "Read Write", "Write", "Read" };
        auto mode = code & 0x00FF;
        auto access = (code & 0x0F00) >> 8;
        auto lock = (code & 0xF000) >> 12;

        imp_name << "(For ";
        if (mode & 0x01) {
            imp_name << "Input";
        }
        else if (mode & 0x02) {
            imp_name << "Output";
        }
        else if (mode & 0x04) {
            imp_name << "Random";
        }
        else if (mode & 0x08) {
            imp_name << "Append";
        }
        else if (mode == 0x20) {
            imp_name << "Binary";
        }

        if (access && (access <= sizeof(access_mode)/sizeof(access_mode[0]))) {
            imp_name << " Access " << std::string(access_mode[access - 1]);
        }
        if (lock) {
            if (lock & 0x04) {
                imp_name << " Shared";
            }
            else if (lock <= sizeof(lock_mode) / sizeof(lock_mode[0])) {
                imp_name << " Lock " << std::string(lock_mode[lock - 1]);
            }
        } 
        imp_name << ")";
    }
    return imp_name.str();
}

std::string disasm_rec(const pcode_datas_t* pcode, const uint8_t* data, uint32_t size, uint32_t code) {
    if (pcode == nullptr) {
        return "";
    }

    std::string obj_name = get_name(&pcode->indirect, code + 2, pcode->identifiers, pcode->identifiers_size
        , pcode->vba_ver, pcode->little_endian, pcode->is64bit);

    auto option = get_uint16(pcode->indirect.data, pcode->indirect.size, code + 18, pcode->little_endian);
    if ((option & 1) == 0) {
        obj_name = "(Private) " + obj_name;
    }
    return obj_name;
}

std::string disasm_type(const uint8_t* data, uint32_t size, uint32_t code) {
    constexpr const char* dim_types[] = {
        "", "Null", "Integer", "Long", "Single", "Double", "Currency", "Date", "String", "Object", "Error", "Boolean", "Variant", "", "Decimal", "", "", "Byte"
    };
    constexpr auto dim_types_size = sizeof(dim_types) / sizeof(dim_types[0]);

    std::stringstream type_name;
    if (code + 6 < size && (*(data + code + 6) < dim_types_size)) {
        uint8_t type_id = *(data + code + 6);
        type_name << std::string(dim_types[type_id]);
    }
    else {
        type_name << "type_" << stream_fmt_0nX(8) << code;
    }
    return type_name.str();
}

std::string disasm_object(const pcode_datas_t* pcode, uint32_t code) {
    if (pcode == nullptr) {
        return "";
    }

    if (pcode->is64bit) {
        /* TODO - Dim declarations in 64 - bit Office documents */
        return "";
    }

    bool get_ok = true;
    uint32_t type_desc = get_uint32(pcode->indirect.data, pcode->indirect.size, code, pcode->little_endian, &get_ok);
    if (!get_ok) { return ""; }
    uint16_t flags = get_uint16(pcode->indirect.data, pcode->indirect.size, type_desc, pcode->little_endian, &get_ok);
    if (!get_ok) { return ""; }

    if (flags & 0x02) {
        return disasm_type(pcode->indirect.data, pcode->indirect.size, type_desc);
    }
    
    uint16_t word = get_uint16(pcode->indirect.data, pcode->indirect.size, type_desc + 2, pcode->little_endian, &get_ok);
    if (!get_ok || word == 0) { return ""; }

    uint32_t offs = (word >> 2) * 10 + 6;
    auto hl_name = get_uint16(pcode->object.data, pcode->object.size, offs, pcode->little_endian, &get_ok);
    if (!get_ok) { return ""; }

    return get_id(hl_name, pcode->identifiers, pcode->identifiers_size, pcode->vba_ver, pcode->is64bit);
}

std::string disasm_var(const pcode_datas_t* pcode, uint32_t code) {
    if (pcode == nullptr) {
        return "";
    }

    auto ident = pcode->identifiers;
    auto ident_size = pcode->identifiers_size;

    if (code + 1 >= pcode->indirect.size) {
        return "";
    }
    uint8_t flag_a = *(pcode->indirect.data + code);
    uint8_t flag_b = *(pcode->indirect.data + code + 1);
    bool has_as = (flag_a & 0x20) != 0;
    bool has_new = (flag_b & 0x20) != 0;
    std::string var_name = get_name(&pcode->indirect, code + 2, ident, ident_size, pcode->vba_ver, pcode->little_endian, pcode->is64bit);
    if (!has_as && !has_new) {
        return var_name;
    }

    std::string var_type;
    var_type += has_new ? "New" : "";
    var_type += has_new && has_as ? " " : "";
    if (has_as) {
        std::string type_name;
        uint16_t offset = pcode->is64bit ? 16 : 12;
        uint16_t word = get_uint16(pcode->indirect.data, pcode->indirect.size, code + offset + 2, pcode->little_endian);
        if (word == 0xFFFF) {
            if (code + offset < pcode->indirect.size) {
                uint8_t type_id = *(pcode->indirect.data + code + offset);
                type_name = get_typename(type_id);
            }
        }
        else {
            type_name = disasm_object(pcode, code + offset);
        }   
        if (!type_name.empty()) {
            var_type += "As " + type_name;
        }
    }
    if (!var_type.empty()) {
        var_name += " (" + var_type + ")";
    }
    return var_name;
}

std::string disasm_arg(const pcode_table_t* table, const char** identifiers, uint32_t identifiers_cnt
    , uint32_t arg_offset, uint32_t vba_ver, bool little_endian, bool is64bit) {
    uint32_t offs = is64bit ? 4 : 0;
    if (table == nullptr || table->data == nullptr || table->size < arg_offset + offs + 24) {
        return "";
    }
    uint16_t flags = get_uint16(table->data, table->size, arg_offset, little_endian);
    std::string arg_name = get_name(table, arg_offset + 2, identifiers, identifiers_cnt, vba_ver, little_endian, is64bit);
    uint32_t arg_type = get_uint32(table->data, table->size, arg_offset + offs + 12, little_endian);
    uint16_t arg_opts = get_uint16(table->data, table->size, arg_offset + offs + 24, little_endian);
    if (arg_opts & 0x0004) {
        arg_name = "ByVal " + arg_name;
    }
    else if (arg_opts & 0x0002) {
        arg_name = "ByRef " + arg_name;
    }
    else if (arg_opts & 0x0200) {
        arg_name = "Optional " + arg_name;
    }
    
    if (flags & 0x0020) {
        arg_name += " As ";
        if (arg_type & 0xFFFF0000) {
            arg_name += get_typename(arg_type & 0x000000FF);
        }
        else {
            // TODO - Custom type arguments aren't disassembled properly
        }
    }
    else {
        // TODO - ParamArray arguments aren't disassebled properly
    }

    return arg_name;
}

std::string disasm_func(const pcode_datas_t* pcode, uint32_t id_code, uint16_t optype) {
    if (pcode == nullptr || pcode->indirect.size <= id_code) {
        return "";
    }
    
    std::stringstream result;
    result << "(";

    auto ident = pcode->identifiers;
    auto ident_size = pcode->identifiers_size;
    auto flags = get_uint16(pcode->indirect.data, pcode->indirect.size, id_code, pcode->little_endian);
    auto sub_name = get_name(&pcode->indirect, id_code + 2, ident, ident_size, pcode->vba_ver, pcode->little_endian, pcode->is64bit);
    uint32_t offs2 = ((pcode->vba_ver > 5) ? 4 : 0) + (pcode->is64bit ? 16 : 0);
    if (pcode->indirect.size <= id_code + offs2 + 57) {
        return "";
    }
    uint32_t arg_offset = get_uint32(pcode->indirect.data, pcode->indirect.size, id_code + offs2 + 36, pcode->little_endian);
    uint32_t ret_type = get_uint32(pcode->indirect.data, pcode->indirect.size, id_code + offs2 + 40, pcode->little_endian);
    uint32_t dec_offset = get_uint32(pcode->indirect.data, pcode->indirect.size, id_code + offs2 + 44, pcode->little_endian);
    uint8_t options = *(pcode->indirect.data + id_code + offs2 + 54);
    uint8_t new_flags = *(pcode->indirect.data + id_code + offs2 + 57);
    bool has_declare = false;
    // TODO - 'Private' and 'Declare' for 64 - bit Office
    if (pcode->vba_ver > 5) {
        result << ((new_flags & 0x0002) == 0 && !pcode->is64bit ? "Private " : "");
        result << ((new_flags & 0x0004) ? "Friend " : "");
    }
    else {
        result << ((flags & 0x0008) == 0 ? "Private " : "");
    }
    result << ((optype & 0x04) ? "Public " : "");
    result << ((flags & 0x0080) ? "Static " : "");
    if ((options & 0x90) == 0 && (dec_offset != 0xFFFF) && !pcode->is64bit) {
        has_declare = true;
        result << "Declare ";
    }
    result << (pcode->vba_ver > 5 && (new_flags & 0x20) ? "PtrSafe " : "");
    
    if (flags & 0x1000) {
        result << ((optype == 2 || optype == 6) ? "Function " : "Sub ");
    }
    else if (flags & 0x2000) {
        result << "Property Get ";
    }
    else if (flags & 0x4000) {
        result << "Property Let ";
    }
    else if (flags & 0x8000) {
        result << "Property Set ";
    }
    result << sub_name;
    if (has_declare) {
        auto lib_name = get_name(&pcode->declare, dec_offset + 2, ident, ident_size, pcode->vba_ver, pcode->little_endian, pcode->is64bit);
        result << " Lib \"" << lib_name << "\" ";
    }
    std::string arg_list;
    for (uint32_t i = 0; arg_offset && arg_offset != 0xFFFFFFFF && arg_offset + 26 < pcode->indirect.size;) {
        auto arg_name = disasm_arg(&pcode->indirect, ident, ident_size, arg_offset, pcode->vba_ver, pcode->little_endian, pcode->is64bit);
        arg_list += ((i == 0) ? "" : ",") + arg_name;
        arg_offset = get_uint32(pcode->indirect.data, pcode->indirect.size, arg_offset + 20, pcode->little_endian);
    }
    result << "(" << arg_list << ")";

    bool has_as = (flags & 0x0020) != 0;
    if (has_as) {
        result << " As ";
        std::string type_name;
        if ((ret_type & 0xFFFF0000) == 0xFFFF0000) {
            type_name = get_typename(ret_type & 0x000000FF);
        }
        else {
            type_name = get_name(&pcode->indirect, ret_type + 6, ident, ident_size, pcode->vba_ver, pcode->little_endian, pcode->is64bit);
        }
        result << type_name;
    }
    result << ")";
    return result.str();
}

int decode_pcode_line(const pcode_datas_t* pcode, const uint8_t* data, uint32_t size, uint32_t line_offset, uint32_t line_size, std::string& out) {
    if (pcode == nullptr || data == nullptr || line_offset + line_size > size) {
        return BQ::INVALID_ARG;
    }
    if (line_size == 0) {
        out.clear();
        return BQ::OK;
    }
    bool little_endian = pcode->little_endian;

    auto find_opcode = [](uint16_t opcode, const opcode_item_t* opcodes, uint32_t size) -> const opcode_item_t* {
        if (opcode >= size) {
            return nullptr;
        }
        return opcodes + opcode;
    };

    bool get_ok = true;
    std::stringstream result;
    uint16_t opcode = 0, optype = 0;
    for (auto offset = line_offset, end = line_offset + line_size; offset < end; ) {
        opcode = get_uint16(data, size, offset, little_endian, &get_ok);
        if (!get_ok) { break; }
        offset += sizeof(uint16_t);
        optype = (opcode & ~0x03FF) >> 10;
        opcode &= 0x03FF;
        opcode = translate_opcode(opcode, pcode->vba_ver, pcode->is64bit);

        auto instruction = find_opcode(opcode, opcodes, sizeof(opcodes) / sizeof(opcodes[0]));
        if (instruction == nullptr) {
            break;
        }

        result << "\t";
        if (pcode->verbose) {
            result << "0x" << stream_fmt_0nX(4) << opcode;
        }
        result << std::string(instruction->mnem) << " ";

        if (0 == ucs::cmp(instruction->mnem, "Coerce")
            || 0 == ucs::cmp(instruction->mnem, "CoerceVar")
            || 0 == ucs::cmp(instruction->mnem, "DefType")) {
            if (optype < sizeof(pcode_types) / sizeof(pcode_types[0])) {
                result << "(" << std::string(pcode_types[optype]) << ") ";
            }
            else if (optype == 17) {
                result << "(Byte) ";
            }
            else {
                result << "(" << optype << ") ";
            }
        }
        else if (0 == ucs::cmp(instruction->mnem, "Dim")
            || 0 == ucs::cmp(instruction->mnem, "DimImplicit")
            || 0 == ucs::cmp(instruction->mnem, "Type")) {
            std::string dim_type;
            if (optype & 0x04) {
                dim_type += "Global";
            }
            else if (optype & 0x08) {
                dim_type += "Public";
            }
            else if (optype & 0x10) {
                dim_type += "Private";
            }
            else if (optype & 0x20) {
                dim_type += "Static";
            }
            if ((optype & 0x01) && ucs::cmp(instruction->mnem, "Type")) {
                dim_type += " Const";
            }
            if (!dim_type.empty()) {
                result << "(" << dim_type << ") ";
            }
        }
        else if (0 == ucs::cmp(instruction->mnem, "LitVarSpecial")) {
            if (optype < sizeof(pcode_specials) / sizeof(pcode_specials[0])) {
                result << "(" << std::string(pcode_specials[optype]) << ")";
            }
        }
        else if (0 == ucs::cmp(instruction->mnem, "ArgsCall")
            || 0 == ucs::cmp(instruction->mnem, "ArgsMemCall")
            || 0 == ucs::cmp(instruction->mnem, "ArgsMemCallWith")) {
            if (optype < 16) {
                result << "(Call) ";
            }
            else {
                optype -= 16;
            }
        }
        else if (0 == ucs::cmp(instruction->mnem, "Option")) {
            if (optype < sizeof(pcode_options) / sizeof(pcode_options[0])) {
                result << " (" << std::string(pcode_options[optype]) << ")";
            }
        }
        else if (0 == ucs::cmp(instruction->mnem, "Redim") || 0 == ucs::cmp(instruction->mnem, "RedimAs")) {
            if (optype & 16) {
                result << "(Preserve) ";
            }
        }

        for (uint32_t i = 0; i < sizeof(instruction->args) / sizeof(instruction->args[0]); i++) {
            if (instruction->args[i] == 0) { break; }

            auto arg = instruction->args[i];
            if (0 == ucs::cmp(arg, "name")) {
                uint16_t word = get_uint16(data, size, offset, little_endian, &get_ok);
                if (!get_ok) { break; }
                offset += sizeof(uint16_t);
                result << disasm_name(pcode, word, optype, instruction->mnem);
            }
            else if (0 == ucs::cmp(arg, "0x") || 0 == ucs::cmp(arg, "imp_")) {
                uint16_t word = get_uint16(data, size, offset, little_endian, &get_ok);
                if (!get_ok) { break; }
                offset += sizeof(uint16_t);
                result << disasm_imp(pcode, word, arg, instruction->mnem);
            }
            else if (0 == ucs::cmp(arg, "func_")
                || 0 == ucs::cmp(arg, "var_")
                || 0 == ucs::cmp(arg, "rec_")
                || 0 == ucs::cmp(arg, "type_")
                || 0 == ucs::cmp(arg, "context_")) {
                uint32_t dword = get_uint32(data, size, offset, little_endian, &get_ok);
                if (!get_ok) { break; }
                offset += sizeof(uint32_t);

                if (0 == ucs::cmp(arg, "rec_") && pcode->indirect.size >= dword + 20) {
                    result << disasm_rec(pcode, data, size, dword);
                }
                else if (0 == ucs::cmp(arg, "type_") && pcode->indirect.size >= dword + 7) {
                    auto tmp = disasm_type(pcode->indirect.data, pcode->indirect.size, dword);
                    result << "(As " << tmp << ")";
                }
                else if (0 == ucs::cmp(arg, "var_") && pcode->indirect.size >= dword + 16) {
                    if (optype & 0x20) {
                        result << "(WithEvents) ";
                    }
                    result << disasm_var(pcode, dword);
                    if (optype & 0x10) {
                        auto tmp = get_uint16(data, size, offset, little_endian, &get_ok);
                        if (!get_ok) { break; }
                        offset += sizeof(uint16_t);
                        result << " 0x" << stream_fmt_0nX(4) << tmp;
                    }
                }
                else if (0 == ucs::cmp(arg, "func_") && pcode->indirect.size >= dword + 61) {
                    result << disasm_func(pcode, dword, optype);
                }
                else {
                    result << std::string(arg) << stream_fmt_0nX(8) << dword << " ";
                }
                if (pcode->is64bit && 0 == ucs::cmp(arg, "context_")) {
                    uint32_t dword = get_uint32(data, size, offset, little_endian, &get_ok);
                    if (!get_ok) { break; }
                    offset += sizeof(uint32_t);
                    result << stream_fmt_0nX(8) << dword << " ";
                }   
            }
            else {

            }
        }

        if (!get_ok) { break; }

        if (instruction->varg) {
            auto w_len = get_uint16(data, size, offset, little_endian, &get_ok);
            if (!get_ok) { break; }
            offset += sizeof(w_len);
            std::string var_arg = disasm_var_arg(pcode, data, size, offset, w_len, instruction->mnem);
            result << var_arg;
            offset += w_len + ((w_len & 1) ? 1 : 0);
        }
    }

    out = result.str();
    return !get_ok ? BQ::ERR_FORMAT : BQ::OK;
}

int decode_pcode(const uint8_t* data, uint32_t size, string_list_t* identifiers, uint32_t syskind, uint16_t vba_version, uint8_t** out, uint32_t* out_size) {
    if (data == nullptr || size == 0 || out == nullptr || out_size == nullptr) {
        return BQ::INVALID_ARG;
    }

    int nret = BQ::OK;
    bool is_bit64 = syskind == 3;
    bool little_endian = true;
    auto tmp = get_uint16(data, size, 2, true);
    if (tmp > 0xFF) { little_endian = false; }

    pcode_datas_t pcode;
    pcode.magic = 0xCAFE;
    pcode.is64bit = is_bit64;
    pcode.little_endian = little_endian;
    pcode.verbose = false;
    pcode.vba_ver = 3;
    pcode.identifiers = identifiers->items;
    pcode.identifiers_size = identifiers->count;

    // Office 2010 is 0x0097; Office 2013 is 0x00A3;
    // Office 2016 PC 32 - bit is 0x00B2, 64 - bit is 0x00D7, Mac is 0x00D9
    bool get_ok = true;
    uint32_t offset = 0;
    do {
        if (vba_version >= 0x6B) {
            pcode.vba_ver = (vba_version >= 0x97) ? 7 : 6;

            pcode.declare.offset = is_bit64 ? 0x0043 : 0x003F;
            pcode.declare.size = get_uint32(data, size, pcode.declare.offset, little_endian, &get_ok);
            if (!get_ok) { break; }
            pcode.declare.offset += sizeof(uint32_t);

            pcode.indirect.offset = get_uint32(data, size, 0x0011, little_endian, &get_ok);
            if (!get_ok) { break; }
            pcode.indirect.offset += is_bit64 ? 12 : 10;
            pcode.indirect.size = get_uint32(data, size, pcode.indirect.offset, little_endian, &get_ok);
            if (!get_ok) { break; }
            pcode.indirect.offset += sizeof(uint32_t);

            pcode.object.offset = get_uint32(data, size, 0x0005, little_endian, &get_ok) + 0x8A;
            if (!get_ok) { break; }
            pcode.object.size = get_uint32(data, size, pcode.object.offset, little_endian, &get_ok);
            if (!get_ok) { break; }
            pcode.object.offset += sizeof(uint32_t);

            offset = 0x0019;
        }
        else {
            pcode.vba_ver = 5;
            offset = 11;

            pcode.declare.offset = offset;
            pcode.declare.size = get_uint32(data, size, pcode.declare.offset, little_endian, &get_ok);
            if (!get_ok) { break; }
            pcode.declare.offset += sizeof(uint32_t);

            offset = skip_structure<uint32_t>(data, size, offset, little_endian, 1, false);
            if (offset > size) { break; }
            offset += 64;
            offset = skip_structure<uint16_t>(data, size, offset, little_endian, 16, false);
            if (offset > size) { break; }
            offset = skip_structure<uint32_t>(data, size, offset, little_endian, 1, false);
            if (offset > size) { break; }
            offset += 6;
            offset = skip_structure<uint32_t>(data, size, offset, little_endian, 1, false);
            if (offset > size) { break; }

            pcode.indirect.offset = get_uint32(data, size, offset + 8, little_endian, &get_ok) + 10;
            if (!get_ok) { break; }
            pcode.indirect.size = get_uint32(data, size, pcode.indirect.offset, little_endian, &get_ok);
            if (!get_ok) { break; }
            pcode.indirect.offset += sizeof(uint32_t);

            pcode.object.offset = get_uint32(data, size, offset, little_endian, &get_ok) + 0x008A;
            if (!get_ok) { break; }
            pcode.object.size = get_uint32(data, size, pcode.object.offset, little_endian, &get_ok);
            if (!get_ok) { break; }
            pcode.object.offset += sizeof(uint32_t);

            offset += 77;
        }

        if (pcode.object.offset >= size || pcode.declare.offset >= size || pcode.indirect.offset >= size) {
            nret = BQ::ERR_FORMAT;
            break;
        }

        pcode.object.size = std::min(pcode.object.size, size - pcode.object.offset);
        pcode.declare.size = std::min(pcode.declare.size, size - pcode.declare.offset);
        pcode.indirect.size = std::min(pcode.indirect.size, size - pcode.indirect.offset);
        pcode.object.data = data + pcode.object.offset;
        pcode.declare.data = data + pcode.declare.offset;
        pcode.indirect.data = data + pcode.indirect.offset;
    } while (false);

    if (nret != BQ::OK) {
        return nret;
    }

    if (!get_ok || offset > size) {
        return BQ::ERR_FORMAT;
    }

    do {
        offset = get_uint32(data, size, offset, little_endian) + 0x003C;
        auto magic = get_uint16(data, size, offset, little_endian);
        if (magic != pcode.magic) {
            nret = BQ::ERR_MAGIC;
            break;
        }
        offset += sizeof(uint16_t) * 2;
        pcode.vba_line_cnt = get_uint16(data, size, offset, little_endian, &get_ok);
        if (!get_ok) { break; }
        offset += sizeof(uint16_t);
        pcode.vba_line_start = offset + pcode.vba_line_cnt * 12 + 10;
    } while (false);

    if (nret != BQ::OK) {
        return nret;
    }

    if (!get_ok || offset > size) {
        return BQ::ERR_FORMAT;
    }

    std::vector<pcode_line_t> lines;
    for (uint32_t i = 0; i < pcode.vba_line_cnt; i++) {
        offset += 4;
        pcode_line_t line;
        line.bytes = get_uint16(data, size, offset, little_endian, &get_ok);
        if (!get_ok) { break; }
        line.offset = get_uint32(data, size, offset + 4, little_endian, &get_ok);
        if (!get_ok) { break; }
        offset += 8;
        lines.push_back(line);
    }

    if (!get_ok || offset > size) {
        return BQ::ERR_FORMAT;
    }

    std::stringstream decode;
    for (uint32_t i = 0; i < lines.size(); i++) {
        std::string line_out;
        nret = decode_pcode_line(&pcode, data, size, lines[i].offset + pcode.vba_line_start, lines[i].bytes, line_out);
        if (nret != BQ::OK) {
            break;
        }
        decode << line_out << "\n";
    }

    do {
        if (nret != BQ::OK) {
            break;
        }

        auto size = decode.str().size();
        if (size == 0) {
            *out = nullptr;
            *out_size = 0;
            break;
        }

        auto buff = (uint8_t*)malloc(size);
        if (buff == nullptr) {
            nret = BQ::ERR_MALLOC;
            break;
        }
        memset(buff, 0, size);
        memcpy(buff, decode.str().c_str(), size);
        *out = buff;
        *out_size = size;
    } while (false);

    return nret;
}

int parse_performance_cache(const uint8_t* data, uint32_t size, string_list_t* items, bool little_endian) {
    if (data == 0 || size < 0x20) {
        return BQ::INVALID_ARG;
    }
    uint16_t magic = get_uint16(data, size, 0, little_endian);
    if (magic != 0x61CC) {
        return BQ::ERR_FORMAT;
    }
    uint16_t version = get_uint16(data, size, 2, little_endian);
    bool unicode_ref = ((version >= 0x5B) && (version < 0x60 || version > 0x63)) || version == 0x61 || version == 0x4E;
    bool unicode_name = ((version >= 0x59) && (version < 0x60 || version > 0x63)) || version == 0x61 || version == 0x4E;
    bool non_unicode_name = ((version <= 0x59) && (version != 0x4E)) || (version > 0x5F && version < 0x6B);
    uint16_t word = get_uint16(data, size, 5, little_endian);
    little_endian = (word != 0x000E);

    uint32_t offset = 0x1E;
    uint16_t num_refs = get_uint16(data, size, offset, little_endian);
    offset += sizeof(uint16_t) * 2;

    bool get_ok = true;
    for (uint16_t i = 0, ref_len = 0; i < num_refs; i++) {
        ref_len = get_uint16(data, size, offset, little_endian, &get_ok);
        if (!get_ok) { break; }
        offset += sizeof(uint16_t);

        if (ref_len == 0) {
            offset += 6;
        }
        else if ((unicode_ref && (ref_len < 5)) || ((!unicode_ref) && (ref_len < 3))) {
            offset += ref_len;
        }
        else {
            uint32_t c_offset = offset + (unicode_ref ? 4 : 2);
            offset += ref_len;
            if (c_offset < size && (*(data + c_offset) == 'C' || *(data + c_offset) == 'D')) {
                offset = skip_structure<uint16_t>(data, size, offset, little_endian, 1, false);
            }
            if (offset > size) { break; }
        }

        offset += 10;
        word = get_uint16(data, size, offset, little_endian, &get_ok);
        if (!get_ok) { break; }
        offset += sizeof(word);

        if (word) {
            offset = skip_structure<uint16_t>(data, size, offset, little_endian, 1, false);
            if (offset > size) { break; }
            word = get_uint16(data, size, offset, little_endian, &get_ok);
            if (!get_ok) { break; }
            offset += sizeof(word) + (word ? 2 : 0);
            offset += word + 30;
        }
    }

    if (get_ok == false || offset > size) {
        return BQ::ERR_FORMAT;
    }

    uint16_t num_projects = 0;
    do {
        // Number of entries in the class / user forms table
        offset = skip_structure<uint16_t>(data, size, offset, little_endian, 2, false);
        if (offset > size) { break; }
        // Number of compile - time identifier - value pairs
        offset = skip_structure<uint16_t>(data, size, offset, little_endian, 4, false);
        if (offset > size) { break; }
        offset += 2;
        // Typeinfo typeID
        offset = skip_structure<uint16_t>(data, size, offset, little_endian, 1, true);
        if (offset > size) { break; }
        // Project description
        offset = skip_structure<uint16_t>(data, size, offset, little_endian, 1, true);
        if (offset > size) { break; }
        // Project help file name
        offset = skip_structure<uint16_t>(data, size, offset, little_endian, 1, true);
        if (offset > size) { break; }
        offset += 0x64;
        // Skip the module descriptors
        num_projects = get_uint16(data, size, offset, little_endian, &get_ok);
        if (!get_ok) { break; }
        offset += sizeof(num_projects);
    } while (false);

    if (get_ok == false || offset > size) {
        return BQ::ERR_FORMAT;
    }
    
    for (uint16_t i = 0, wlen = 0; i < num_projects; i++) {
        wlen = get_uint16(data, size, offset, little_endian, &get_ok);
        if (!get_ok) { break; }
        // Code module name
        offset += sizeof(uint16_t) + (unicode_name ? wlen : 0);
        if (non_unicode_name && wlen) {
            wlen = get_uint16(data, size, offset, little_endian, &get_ok);
            if (!get_ok) { break; }
            offset += sizeof(uint16_t) + wlen;
        }
        // Stream time
        offset = skip_structure<uint16_t>(data, size, offset, little_endian, 1, false);
        if (offset > size) { break; }
        offset = skip_structure<uint16_t>(data, size, offset, little_endian, 1, true);
        if (offset > size) { break; }
        offset += sizeof(uint16_t);
        if (version > 0x6B) {
            offset = skip_structure<uint16_t>(data, size, offset, little_endian, 1, true);
            if (offset > size) { break; }
        }
        offset = skip_structure<uint16_t>(data, size, offset, little_endian, 1, true);
        if (offset > size) { break; }
        offset += sizeof(uint16_t);
        offset += (version != 0x51) ? 4 : 0;
        offset = skip_structure<uint16_t>(data, size, offset, little_endian, 8, false);
        if (offset > size) { break; }
        offset += 11;
    }
    
    if (get_ok == false || offset > size) {
        return BQ::ERR_FORMAT;
    }

    uint16_t num_ids = 0, num_junk_ids = 0;
    do {
        offset += 6;
        offset = skip_structure<uint32_t>(data, size, offset, little_endian, 1, false);
        if (offset > size) { break; }
        offset += 6;
        auto w0 = get_uint16(data, size, offset, little_endian, &get_ok);
        if (!get_ok) { break; }
        offset += sizeof(w0);
        num_ids = get_uint16(data, size, offset, little_endian, &get_ok);
        if (!get_ok) { break; }
        offset += sizeof(num_ids);
        auto w1 = get_uint16(data, size, offset, little_endian, &get_ok);
        if (!get_ok) { break; }
        offset += sizeof(w1);
        offset += 4;
        num_junk_ids = num_ids + w1 - w0;
        num_ids = w0 - w1;
        // Skip the junk IDs
        for (uint16_t i = 0, word = 0, id_type = 0, id_len = 0; i < num_junk_ids; i++) {
            offset += 4;
            word = get_uint16(data, size, offset, little_endian, &get_ok);
            if (!get_ok) { break; }
            offset += sizeof(word);
            id_type = (word & 0xFF00) >> 8;
            id_len = (word & 0x00FF);
            offset += (id_type > 0x7F ? 6 : 0) + id_len;
        }
    } while (false);

    if (get_ok == false || offset > size) {
        return BQ::ERR_FORMAT;
    }

    // Now offset points to the start of the variable names area
    std::vector<std::string> identifiers;
    for (uint16_t i = 0, word = 0, id_type = 0, id_len = 0; i < num_ids; i++) {
        bool is_kwd = false;
        word = get_uint16(data, size, offset, little_endian, &get_ok);
        if (!get_ok) { break; }
        offset += sizeof(word);
        id_type = (word & 0xFF00) >> 8;
        id_len = (word & 0x00FF);

        if (id_len == 0 && id_type == 0) {
            offset += sizeof(uint16_t);
            word = get_uint16(data, size, offset, little_endian, &get_ok);
            if (!get_ok) { break; }
            offset += sizeof(word);
            id_type = (word & 0xFF00) >> 8;
            id_len = (word & 0x00FF);
            is_kwd = true;
        }
        offset += (id_type & 0x80) ? 6 : 0;
        if (id_len > 0 && offset + id_len < size) {
            std::string tmp = std::string((const char*)(data + offset), id_len);
            identifiers.push_back(tmp);
        }
        offset += id_len + (is_kwd ? 0 : 4);
    }

    if (get_ok == false || offset > size) {
        return BQ::ERR_FORMAT;
    }

    return vector2strlist(identifiers, items);
}