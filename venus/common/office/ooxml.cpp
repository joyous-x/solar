#include <algorithm>
#include <set>
#include <stack>
#include "ooxml.h"
#include "./offcrypto.h"
#include "./helper.h"
#include "./ole_utils.h"
#include "./ms_vba.h"
#include "./ooxml_data.h"
#include "../file/memfile.h"
#include "../3rds/tinyxml2_9.0.0/tinyxml2.h"

const uint32_t ooxml_expected_xl_index = 3;
const uint32_t ooxml_expected_count = 3;
const char* ooxml_expected_files[] = {
    "[Content_Types].xml",
    "docProps/app.xml",
    /* the following should only one be shown */
    "word/document.xml",
    "xl/workbook.xml",
    "ppt/presentation.xml",
};

/* xl/macrosheets/[*].xml */
const char* ooxml_macrosheet_prefix = "xl/macrosheets/";
const char* ooxml_macrosheet_suffix = ".xml";

/* word/vbaProject.bin, xl/vbaProject.bin, ppt/vbaProject.bin */
const char* ooxml_vba_project = "/vbaProject.bin";

/* word/_rels/settings.xml.rels，xl/_rels/settings.xml.rels，ppt/_rels/settings.xml.rels */
const char* ooxml_template_rels = "/_rels/settings.xml.rels";

/* xl/_rels/workbook.xml.rels */
const char* ooxml_workbook_rels = "xl/_rels/workbook.xml.rels";
const char* ooxml_xl_sharedstr = "xl/sharedStrings.xml";

/*
* [Content_Types].xml
*/
const char* content_type_excel[] = {
    "application/vnd.openxmlformats-officedocument.spreadsheetml.",
    "application/vnd.ms-excel."
};
const char* content_type_doc[] = {
    "application/vnd.openxmlformats-officedocument.wordprocessingml."
};
const char* content_type_ppt[] = {
    "application/vnd.openxmlformats-officedocument.presentationml."
};

auto make_ooxml_did = [](uint32_t subid, uint32_t offset) {
    return ((subid << 16) | offset);
};

int try_derefrence_cell(const cfb_ctx_t* ctx, const ooxml_file_t* ooxml, xl_ecma_datas_t* datas, xl_cell_t* cell);

int parse_str2cell(const char* txt, const cell_ref_style* style, const xl_cellcoor_t* refer, const std::vector<xl_sheet_table_t>* sheet_tables, xl_cellcoor_t* cell) {
    std::string sheet_name;
    int nret = xl_ecma_utils::cell_str2coor(txt, style, refer, &sheet_name, cell);
    if (nret != BQ::OK) {
        return nret;
    }

    if (!sheet_name.empty()) {
        cell->sheet_id = 0;
        if (sheet_name.at(0) == '\'' && sheet_name.at(sheet_name.size() - 1) == '\'') {
            sheet_name = sheet_name.substr(1, sheet_name.size() - 2);
        }
        for (uint32_t i = 0; sheet_tables && i < sheet_tables->size(); i++) {
            if (ucs::icmp(sheet_tables->at(i).name.c_str(), sheet_name.c_str()) == 0) {
                cell->sheet_id = i;
                break;
            }
        }
        if (cell->sheet_id == 0) {
            nret = BQ::ERR_FORMAT;
        }
    } else {
        cell->sheet_id = refer ? refer->sheet_id : 0;
    }

    return nret;
}

int Ooxml::parse_ooxml_catalog(const cfb_ctx_t* ctx, const char** zip_items, uint32_t count, ooxml_file_t* ooxml) {
    if (zip_items == nullptr || count == 0 || nullptr == ooxml) {
        return BQ::INVALID_ARG;
    }

    uint32_t found = 0;
    for (uint32_t i = 0; i < count; i++) {
        for (int j = 0, expected = (sizeof(ooxml_expected_files) / sizeof(ooxml_expected_files[0])); j < expected; j++) {
            if (0 == ucs::icmp(*(zip_items + i), ooxml_expected_files[j])) {
                found |= (1 << j);
                break;
            }
        }
    }

    if (0x1 != (found & 0x1) || 0 == (found >> 2)) {
        return BQ::OK;
    }

    int filetype = ft_unknown;
    if ((found >> 2) & 0x1) {
        filetype = (ft_unknown == filetype ? ft_ms_docx : ft_unknown);
    }
    if ((found >> 3) & 0x1) {
        filetype = (ft_unknown == filetype ? ft_ms_xlsx : ft_unknown);
    }
    if ((found >> 4) & 0x1) {
        filetype = (ft_unknown == filetype ? ft_ms_pptx : ft_unknown);
    }

    if (filetype == ft_unknown) {
        return BQ::OK;
    }
    ooxml->filetype = filetype;

    std::string prefix;
    switch (filetype) {
    case ft_ms_docx:
        prefix = "word";
        break;
    case ft_ms_xlsx:
        prefix = "xl";
        break;
    case ft_ms_pptx:
        prefix = "ppt";
        break;
    default:
        return BQ::OK;
    }

    uint32_t mcrosheet_prefix_len = ucs::len(ooxml_macrosheet_prefix);
    uint32_t mcrosheet_suffix_len = ucs::len(ooxml_macrosheet_suffix);

    for (uint32_t i = 0; i < count; i++) {
        if (0 == ucs::icmp(*(zip_items + i), (prefix + ooxml_vba_project).c_str())) {
            ooxml->vbaproject_id = (i + 1);
            continue;
        }

        if (0 == ucs::icmp(*(zip_items + i), (prefix + ooxml_template_rels).c_str())) {
            ooxml->setting_rels = i + 1;
            continue;
        }

        if (0 == ucs::icmp(*(zip_items + i), ooxml_workbook_rels)) {
            ooxml->workbook_rels = i + 1;
            continue;
        }

        if (0 == ucs::icmp(*(zip_items + i), ooxml_xl_sharedstr)) {
            ooxml->xl_sharedstring = i + 1;
            continue;
        }

        for (; filetype == ft_ms_xlsx; ) {
            if (0 == ucs::icmp(*(zip_items + i), ooxml_expected_files[ooxml_expected_xl_index])) {
                ooxml->xl_workbook = i + 1;
            }

            uint32_t zip_item_len = ucs::len(*(zip_items + i));
            if (mcrosheet_suffix_len >= zip_item_len) {
                break;
            }

            if (0 == ucs::nicmp(*(zip_items + i) + zip_item_len - mcrosheet_suffix_len, ooxml_macrosheet_suffix, mcrosheet_suffix_len)
                && 0 == ucs::nicmp(*(zip_items + i), ooxml_macrosheet_prefix, mcrosheet_prefix_len)) {
                ooxml->macrosheets[ooxml->macrosheet_cnt++] = (i + 1);
            }

            break;
        }
    }

    /* TODO: check [Content_Types].xml */

    return BQ::OK;
}

int parse_rels_settings(const cfb_ctx_t* ctx, uint8_t* data, uint32_t size, std::vector<std::string>& template_path) {
    if (data == nullptr || size == 0) {
        return BQ::INVALID_ARG;
    }

    tinyxml2::XMLDocument xml_doc;
    int nret = xml_doc.Parse((const char*)data, size);
    if (nret != tinyxml2::XML_SUCCESS) {
        return BQ::ERR_PARSE_XML;
    }

    if (0 != ucs::icmp(xml_doc.RootElement()->Name(), "Relationships")) {
        return BQ::ERR_FORMAT;
    }

    template_path.clear();
    for (const tinyxml2::XMLElement* current = xml_doc.RootElement()->FirstChildElement(); current; current = current->NextSiblingElement()) {
        if (0 != ucs::icmp(current->Name(), "Relationship")) {
            continue;
        }

        if (current->FindAttribute("Id") == nullptr || current->FindAttribute("Id")->Value() == nullptr) {
            continue;
        }

        auto attr_targetnode = current->FindAttribute("TargetMode");
        if (attr_targetnode == nullptr || attr_targetnode->Value() == nullptr || 0 != ucs::icmp(attr_targetnode->Value(), "External")) {
            continue;
        }

        auto attr_type = current->FindAttribute("Type");
        if (attr_type == nullptr || attr_type->Value() == nullptr || nullptr == strstr(attr_type->Value(), "/relationships/attachedTemplate")) {
            // http://schemas.openxmlformats.org/officeDocument/2006/relationships/attachedTemplate
            continue;
        }

        auto attr_target = current->FindAttribute("Target");
        if (attr_target == nullptr || attr_target->Value() == nullptr) {
            continue;
        }
        template_path.push_back(attr_target->Value());
    }

    return BQ::OK;
}

int parse_relationships(const tinyxml2::XMLDocument* xml, std::vector<xl_rels_t>& rels) {
    if (xml == nullptr || nullptr == xml->RootElement()) {
        return BQ::INVALID_ARG;
    }

    if (0 != ucs::icmp(xml->RootElement()->Name(), "Relationships")) {
        return BQ::ERR_FORMAT;
    }

    const char* xmlns = xml->RootElement()->Attribute("xmlns");
    if (xmlns == nullptr) {
        return BQ::ERR_FORMAT;
    }

    const tinyxml2::XMLElement* item = xml->RootElement()->FirstChildElement("Relationship");
    for (; item; item = item->NextSiblingElement("Relationship")) {
        auto attr_id = item->FindAttribute("Id");
        auto attr_type = item->FindAttribute("Type");
        auto attr_target = item->FindAttribute("Target");
        if (attr_id == nullptr || attr_target == nullptr) {
            continue;
        }
        xl_rels_t rels_item;
        rels_item.id = attr_id->Value() ? attr_id->Value() : "";
        rels_item.target = attr_target->Value() ? attr_target->Value() : "";
        rels_item.type = (attr_type && attr_type->Value()) ? attr_type->Value() : "";
        rels.push_back(std::move(rels_item));
    }

    return BQ::OK;
}

int parse_sharedstrings(const tinyxml2::XMLDocument* xml, std::vector<xl_sharedstr_t>& shared) {
    if (xml == nullptr || nullptr == xml->RootElement()) {
        return BQ::INVALID_ARG;
    }

    if (0 != ucs::icmp(xml->RootElement()->Name(), "sst")) {
        return BQ::ERR_FORMAT;
    }

    const char* xmlns = xml->RootElement()->Attribute("xmlns");
    if (xmlns == nullptr) {
        return BQ::ERR_FORMAT;
    }

    const tinyxml2::XMLElement* si = xml->RootElement()->FirstChildElement("si");
    for (uint32_t id = 0; si; si = si->NextSiblingElement("si"), id++) {
        xl_sharedstr_t sharedstr;
        sharedstr.id = id;

        for (auto item = si->FirstChildElement(); item; item = item->NextSiblingElement()) {
            auto name = item->Name();
            if (name == nullptr) {
                continue;
            }

            // text (<t>)   
            if (ucs::nicmp(name, "t", 1) == 0) {
                sharedstr.val += item->GetText() ? item->GetText() : "";
                continue;
            }

            // RichTextRun (<r>) & RunProperties (<rPr>, ignore) 
            if (ucs::nicmp(name, "r", 1) == 0 && item->FirstChildElement("t")) {
                auto rt = item->FirstChildElement("t");
                sharedstr.val += rt->GetText() ? rt->GetText() : "";
            }
        }
        
        shared.push_back(std::move(sharedstr));
    }

    return BQ::OK;
}

int parse_workbook(const tinyxml2::XMLDocument* xml, cell_ref_style* ref_mode, std::vector<xl_defined_sheet_t>& sheets, std::vector<xl_defined_name_t>& names) {
    if (xml == nullptr || nullptr == xml->RootElement()) {
        return BQ::INVALID_ARG;
    }

    if (0 != ucs::icmp(xml->RootElement()->Name(), "workbook")) {
        return BQ::ERR_FORMAT;
    }

    const char* xmlns = xml->RootElement()->Attribute("xmlns");
    if (xmlns == nullptr) {
        return BQ::ERR_FORMAT;
    }

    cell_ref_style ref_style = ref_style_a1;
    do {
        auto elem_calc_pr = xml->RootElement()->FirstChildElement("calcPr");
        if (elem_calc_pr) {
            auto attr_ref_mode = elem_calc_pr->FindAttribute("refMode");
            if (attr_ref_mode && 0 == ucs::icmp(attr_ref_mode->Value(), "R1C1")) {
                ref_style = ref_style_r1c1;
            }
        }
        if (ref_mode) *ref_mode = ref_style;
    } while(false);

    auto elem_sheets = xml->RootElement()->FirstChildElement("sheets");
    for (auto item = elem_sheets->FirstChildElement("sheet"); item; item = item->NextSiblingElement()) {
        auto attr_name = item->FindAttribute("name");
        auto attr_sheetId = item->FindAttribute("sheetId");
        auto attr_rid = item->FindAttribute("r:id");
        if (attr_rid == nullptr || attr_name == nullptr) {
            continue;
        }
        xl_defined_sheet_t sheet_item;
        sheet_item.name = attr_name->Value() ? attr_name->Value() : "";
        sheet_item.rid = attr_rid->Value() ? attr_rid->Value() : "";
        sheet_item.id = (attr_sheetId && attr_sheetId->Value()) ? attr_sheetId->Value() : "";
        sheets.push_back(std::move(sheet_item));
    }

    auto elem_names = xml->RootElement()->FirstChildElement("definedNames");
    for (auto item = elem_names->FirstChildElement("definedName"); item; item = item->NextSiblingElement()) {
        auto attr_name = item->FindAttribute("name");
        auto value_txt = item->GetText();
        if (attr_name == nullptr) {
            continue;
        }
        xl_defined_name_t defined_name;
        defined_name.name = attr_name->Value() ? attr_name->Value() : "";
        defined_name.cell_raw = value_txt ? value_txt : "";
        names.push_back(std::move(defined_name));
    }

    return BQ::OK;
}

int parse_sheetdata(const tinyxml2::XMLDocument* xml, xl_sheet_t* sheet) {
    if (xml == nullptr || nullptr == xml->RootElement() || nullptr == sheet) {
        return BQ::INVALID_ARG;
    }

    if (0 != ucs::icmp(xml->RootElement()->Name(), "xm:macrosheet")
        && 0 != ucs::icmp(xml->RootElement()->Name(), "worksheet")) {
        return BQ::NOT_IMPLEMENT;
    }    

    const tinyxml2::XMLElement* sheetdata = xml->RootElement()->FirstChildElement("sheetData");
    if (nullptr == sheetdata) {
        return BQ::OK;
    }

    for (const tinyxml2::XMLElement* row = sheetdata->FirstChildElement("row"); row; row = row->NextSiblingElement("row")) {
        auto attr_r = row->FindAttribute("r");
        if (nullptr == attr_r) {
            continue;
        }

        uint32_t row_id = 0;
        if (0 != attr_r->QueryUnsignedValue(&row_id)) {
            continue;
        }

        for (auto col = row->FirstChildElement("c"); col; col = col->NextSiblingElement("c")) {
            auto attr_col_r = col->FindAttribute("r");
            auto attr_col_t = col->FindAttribute("t");
            if (nullptr == attr_col_r || nullptr == attr_col_r->Value()) {
                continue;
            }
            auto col_sub_f = col->FirstChildElement("f");
            auto col_sub_v = col->FirstChildElement("v");
            if (nullptr == col_sub_f && nullptr == col_sub_v) {
                continue;
            }

            std::string colval = attr_col_r->Value();
            colval.erase(std::remove_if(colval.begin(), colval.end(), (int(*)(int))std::isdigit), colval.end());
            if (colval.size() == 0) {
                continue;
            }
            
            xl_cell_t cell;
            cell.sheet_id = sheet->sheet_id;
            cell.row = row_id;
            cell.col = xl_ecma_utils::col_str2num(colval.c_str());
            cell.c_r = attr_col_r->Value() ? attr_col_r->Value() : "";
            cell.is_func = col_sub_f ? true : false;
            if (col_sub_f) {
                cell.f = col_sub_f->GetText() ? col_sub_f->GetText() : "";
            }
            if (col_sub_v) {
                cell.v = col_sub_v->GetText() ? col_sub_v->GetText() : "";
            }            
            if (attr_col_t && attr_col_t->Value()) {
                memcpy(&cell.t[0], attr_col_t->Value(), std::min<uint32_t>(ucs::len(attr_col_t->Value()), sizeof(cell.t) - 1));
            }

            sheet->cells.push_back(std::move(cell));
        }
    }

    return BQ::OK;
}

int Ooxml::decrypt_from_ole(const cfb_ctx_t* ctx, const olefile_t* ole, const char16_t* passwd, crypto_dataspace_t* crypto, ifile** out) {
    if (ctx == nullptr || ctx->file_creator == nullptr || ole == nullptr || out == nullptr) {
        return BQ::INVALID_ARG;
    }

    crypto_dataspace_t crypto_data;
    if (crypto == nullptr) {
        crypto = &crypto_data;
    }

    int nret = offcrypto::parse_dataspace_encryption(ctx, ole, crypto);
    if (nret != BQ::OK) {
        return nret;
    }

    if (false == crypto->encrypted()) {
        return BQ::ERR_FORMAT;
    }
    
    do {
        uint8_t* buffer = 0;
        uint32_t size = 0;

        ON_SCOPE_EXIT([&] { if (buffer) free(buffer); });
        nret = ole::read_stream(ole, crypto->encrypt_pkg_did, -1, &buffer, &size);
        if (nret != BQ::OK) {
            break;
        }

        auto cur_file = (ctx->file_creator)(ole, ctx->temp_dirpath, "encrypted", 0, 0);
        if (nullptr == cur_file) {
            nret = BQ::ERR_FILE_CREATOR;
            break;
        }
        ON_SCOPE_EXIT([&] { if (cur_file) cur_file->release(); });

        if (passwd == nullptr) {
            nret = offcrypto::dataspace_decrypt_stream(ctx, crypto, excel_passwd_default, buffer, size, cur_file);
            if (nret != BQ::OK) {
                nret = offcrypto::dataspace_decrypt_stream(ctx, crypto, ppt_passwd_default, buffer, size, cur_file);
            }
        }
        else {
            nret = offcrypto::dataspace_decrypt_stream(ctx, crypto, passwd, buffer, size, cur_file);
        }

        if (nret != BQ::OK) {
            break;
        }

        *out = cur_file; 
        cur_file = nullptr;
    } while (false);

    return nret;
}

class ooxml_cb_file_wraper : public ifilehandler {
public:
    ooxml_cb_file_wraper(ifilehandler* cb, uint16_t zip_item_id) : m_cb(cb), m_zip_item_id(zip_item_id) {
    }

    virtual int handle(const ctx_t* ctx, const file_obj_t* fileobj, void* reserved = 0) {
        if (fileobj) {
            ((file_obj_t*)fileobj)->container = fileobj->container ? fileobj->container->container : nullptr;
            ((embedded_object_t*)fileobj)->did = make_ooxml_did(m_zip_item_id, ((embedded_object_t*)fileobj)->did);
        }
        return m_cb->handle(ctx, fileobj, reserved);
    }

protected:
    uint16_t      m_zip_item_id{ 0 };
    ifilehandler* m_cb{nullptr};
};

int parse_xml_by_subid(const ooxml_file_t* ooxml, int32_t subid, tinyxml2::XMLDocument* xml) {
    if (ooxml == nullptr || xml == nullptr) {
        return BQ::INVALID_ARG;
    }

    int nret = BQ::OK;
    do {
        uint8_t* data = nullptr;
        uint32_t size = 0;
        ON_SCOPE_EXIT([&]() { if (data) free(data); });

        nret = ooxml->unzip->get_item(subid, &data, &size);
        if (nret != BQ::OK) {
            nret = BQ::ERR_UNCOMPRESS;
            break;
        }

        xml->Clear();
        nret = xml->Parse((const char*)data, size);
        if (nret != tinyxml2::XML_SUCCESS) {
            nret = BQ::ERR_PARSE_XML;
            break;
        }
    } while (false);

    return nret;
}

int parse_workbook_tables(const ooxml_file_t* ooxml, cell_ref_style* ref_mode, std::vector<xl_sheet_table_t>& sheet_tables, std::vector<xl_defined_name_t>& names, std::vector<xl_sharedstr_t>& sharedstr) {
    if (ooxml == nullptr) {
        return BQ::INVALID_ARG;
    }

    int nret = BQ::OK;
    std::vector<xl_defined_sheet_t> sheets;
    std::vector<xl_rels_t> rels;

    do {
        if (ooxml->xl_sharedstring < 1) {
            break;
        }
        auto subid = ooxml->xl_sharedstring - 1;

        tinyxml2::XMLDocument xml;
        nret = parse_xml_by_subid(ooxml, subid, &xml);
        if (nret != BQ::OK) {
            break;
        }

        nret = parse_sharedstrings(&xml, sharedstr);
        if (nret != BQ::OK) {
            // should not be terminated
        }

        nret = BQ::OK;
    } while (false);
    
    if (nret != BQ::OK) {
        return nret;
    }

    do {
        if (ooxml->xl_workbook < 1) {
            break;
        }
        auto subid = ooxml->xl_workbook - 1;

        tinyxml2::XMLDocument xml;
        nret = parse_xml_by_subid(ooxml, subid, &xml);
        if (nret != BQ::OK) {
            break;
        }

        nret = parse_workbook(&xml, ref_mode, sheets, names);
        if (nret != BQ::OK) {
            break;
        }
    } while (false);

    if (nret != BQ::OK) {
        return nret;
    }

    do {
        if (ooxml->workbook_rels < 1) {
            break;
        }
        auto subid = ooxml->workbook_rels - 1;

        tinyxml2::XMLDocument xml;
        nret = parse_xml_by_subid(ooxml, subid, &xml);
        if (nret != BQ::OK) {
            break;
        }
        
        nret = parse_relationships(&xml, rels);
        if (nret != BQ::OK) {
            break;
        }
    } while (false);

    if (nret != BQ::OK) {
        return nret;
    }

    sheet_tables.clear();
    sheet_tables.push_back(xl_sheet_table_t());

    std::set<uint32_t> used;
    for (uint32_t i = 0; i < sheets.size(); i++) {
        xl_sheet_table_t item;
        item.name = sheets[i].name;
        item.rid = sheets[i].rid;
        for (uint32_t j = 0; j < rels.size(); j++) {
            if (0 == ucs::icmp(item.rid.c_str(), rels[j].id.c_str())) {
                item.type = rels[j].type;
                item.target = rels[j].target;
                used.insert(j);
            }
        }
        sheet_tables.push_back(std::move(item));
    }

    for (uint32_t j = 0; j < rels.size(); j++) {
        if (used.find(j) != used.end()) {
            continue;
        }

        xl_sheet_table_t item;
        item.rid = rels[j].id;
        item.type = rels[j].type;
        item.target = rels[j].target;
        sheet_tables.push_back(std::move(item));
    }

    for (uint32_t i = 0; i < names.size(); i++) {
        parse_str2cell(names[i].cell_raw.c_str(), ref_mode, nullptr, &sheet_tables, &names[i].cell);
    }

    return BQ::OK;
}

int try_find_cell_value(const cfb_ctx_t* ctx, const ooxml_file_t* ooxml, xl_ecma_datas_t* datas, xl_cell_t* cell) {
    if (datas == nullptr || cell == nullptr) {
        return BQ::INVALID_ARG;
    }
    int nret = BQ::OK;

    const xl_sheet_table_t* sheet_table = nullptr;
    if (cell->sheet_id > 0 || (uint32_t)cell->sheet_id < datas->sheet_tables.size()) {
        sheet_table = &datas->sheet_tables[cell->sheet_id];
    }

    xl_sheet_t* target_sheet = nullptr;
    for (uint32_t i = 0; i < datas->macro_sheets.size(); i++) {
        if (datas->macro_sheets[i].sheet_id == cell->sheet_id) {
            target_sheet = &datas->macro_sheets[i];
            break;
        }
    }

    do {
        if (target_sheet != nullptr) {
            break;
        }

        for (uint32_t i = 0; i < datas->norm_sheets.size(); i++) {
            if (datas->norm_sheets[i].sheet_id == cell->sheet_id) {
                target_sheet = &datas->norm_sheets[i];
                break;
            }
        }

        if (target_sheet != nullptr) {
            break;
        }

        if (sheet_table == nullptr) {
            nret = BQ::INVALID_ARG;
            break;
        }        

        uint32_t subid = 0;
        for (int32_t i = 0; i < ooxml->zipitem_cnt(); i++) {
            if (0 == ucs::icmp(ooxml->zipitem_path(i), (std::string("xl/") + sheet_table->target).c_str())) {
                subid = i + 1;
                break;
            }
        }
        if (subid == 0) {
            nret = BQ::ERR;
            break;
        }

        tinyxml2::XMLDocument xml;
        nret = parse_xml_by_subid(ooxml, subid - 1, &xml);
        if (nret != BQ::OK) {
            break;
        }

        xl_sheet_t sheet;
        sheet.sheet_id = cell->sheet_id;
        sheet.sheet_type = sheet_type_normal;
        sheet.ooxml_subid = subid;
        nret = parse_sheetdata(&xml, &sheet);
        if (nret != BQ::OK) {
            break;
        }
        datas->norm_sheets.push_back(std::move(sheet));
        target_sheet = &datas->norm_sheets[datas->norm_sheets.size() - 1];
    } while(false);

    if (nret != BQ::OK) {
        if (ctx && ctx->loger) {
            auto sheet_name = sheet_table ? sheet_table->target.c_str() : "sheet_id=" + std::to_string(cell->sheet_id);
            ctx->loger->debug("[try_find_cell_value] %s R%dC%d can't find the target sheet, error(%08x)\n", sheet_name.c_str(), cell->row, cell->col, nret);
        }
        return nret;
    }

    xl_cell_t* target_cell = nullptr;
    for (uint32_t i = 0; i < target_sheet->cells.size(); i++) {
        auto cur_cell = &target_sheet->cells[i];
        if (cur_cell->col == cell->col && cur_cell->row == cell->row) {
            try_derefrence_cell(ctx, ooxml, datas, cur_cell);
            target_cell = cur_cell;
            break;
        }
    }

    if (target_cell == nullptr) {
        nret = BQ::ERR;
        if (ctx && ctx->loger) {
            auto sheet_name = sheet_table ? sheet_table->target.c_str() : "sheet_id=" + std::to_string(cell->sheet_id);
            ctx->loger->debug("[try_find_cell_value] %s R%dC%d can't find the cell in sheet \n", sheet_name.c_str(), cell->row, cell->col);
        }
    } else {
        cell->v = target_cell->v;
        cell->f = target_cell->f;
        cell->c_r = target_cell->c_r;
        cell->is_func = target_cell->is_func;
        cell->is_deobfuscated = target_cell->is_deobfuscated;
        memcpy(cell->t, target_cell->t, sizeof(target_cell->t));
        nret = BQ::OK;
    }

    return nret;
}

/*
* @brief try to dereference a cell and return the value with the argument cell.
*        and write the cell value to ooxml_datas_t.macro_sheets or ooxml_datas_t.sheets
*/
int try_derefrence_cell(const cfb_ctx_t* ctx, const ooxml_file_t* ooxml, xl_ecma_datas_t* datas, xl_cell_t* cell) {
    if (nullptr == ooxml || datas == nullptr || cell == nullptr) {
        return BQ::INVALID_ARG;
    }

    if (cell->is_deobfuscated) {
        return BQ::OK;
    }
    else if (ucs::icmp(cell->t, "s") == 0) {
        if (!xl_ecma_utils::only_digits(cell->v.c_str())) {
            return BQ::ERR_FORMAT;
        }
        uint32_t shared_index = std::atoi(cell->v.c_str());
        if (shared_index >= datas->sharedstr.size()) {
            return BQ::ERR_FORMAT;
        }
        cell->v = datas->sharedstr[shared_index].val;
        cell->is_deobfuscated = true;
        return BQ::OK;
    }
    else if (!cell->is_func) {
        return BQ::OK;
    }
    else if (cell->is_func && !cell->v.empty() && ucs::icmp(cell->t, "str") == 0) {
        cell->is_deobfuscated = true;
        return BQ::OK;
    }

#if _DEBUG
    auto cell2str = [](const xl_cell_t* cell) {
        std::string cell_info;
        cell_info += std::to_string(cell->sheet_id) + ":R" + std::to_string(cell->row) + "C" + std::to_string(cell->col);
        cell_info += ", is_func: " + cell->is_func ? "true" : "false";
        cell_info += ", deobfuscated: " + cell->is_deobfuscated ? "true" : "false";
        cell_info += ", " + cell->c_r + ", " + cell->f + "\n\t\t value: " + cell->v;
        return cell_info;
    };

    ON_SCOPE_EXIT([&]() {
        std::string cell_info_e = "---> ";
        cell_info_e += cell2str(cell);
        if (ctx && ctx->loger) {
            ctx->loger->debug("[try_derefrence_cell] %s \n", cell_info_e.c_str());
        }
    });
#endif

    std::vector<std::string> splited;
    int nret = xl_ecma_utils::split_cell_formula(cell->f.c_str(), splited);
    if (nret != BQ::OK) {
        cell->is_deobfuscated = true;
        return nret;
    }

    std::vector<xl_cell_t> dereferenced; 
    for (uint32_t i = 0; i < splited.size(); i++) {
        dereferenced.push_back(xl_cell_t());

        // simply, we think the function names and defined names must be start with an alpha or '_' or '\''
        if (splited[i].at(0) != '_' && splited[i].at(0) != '\'' && !isalpha(splited[i].at(0))) {
            continue;
        }

        const xl_defined_name_t* referenced_name = nullptr;
        for (uint32_t d = 0; d < datas->defined_names.size(); d++) {
            if (datas->defined_names[d].name == splited[i]) {
                referenced_name = &datas->defined_names[d];
                break;
            }
        }

        xl_cell_t* referenced = &dereferenced[dereferenced.size() - 1];
        if (referenced_name == nullptr) {
            if (BQ::OK != parse_str2cell(splited[i].c_str(), &datas->ref_mode, cell, &datas->sheet_tables, referenced)) {
                continue;
            }
        } else {
            memcpy(referenced, &referenced_name->cell, sizeof(xl_cellcoor_t));
        }

        if (BQ::OK != try_find_cell_value(ctx, ooxml, datas, referenced)) {
            memset(referenced, 0, sizeof(xl_cellcoor_t));
        }
    }
    cell->is_deobfuscated = true;

    for (uint32_t i = 0; i < splited.size(); i++) {
        if (dereferenced[i].col || dereferenced[i].row) {
            splited[i] = dereferenced[i].v;
        }
    }

    /*
    * parse functions and reconstruct the cell value
    */
    std::string final;
    for (uint32_t i = 0; i < splited.size(); i++) {
        if (splited[i].empty() || splited[i] == "&") continue;

        if (!isalpha(splited[i].at(0)) || i + 1 >= splited.size() || splited[i+1] != "(") {
            final += splited[i];
            continue;
        }

        uint32_t func_start = i, func_end = i;
        for (uint32_t l_part = 0, r_part = 0; func_end < splited.size(); func_end++) {
            if (splited[func_end] == "(") l_part++;
            else if (splited[func_end] == ")") r_part++;
            if (l_part > 0 && l_part == r_part) { break;}
        }

        if (func_end >= splited.size() || func_end == func_start + 2) {
            for (; i <= func_end && i < splited.size(); i++) final += splited[i];
            i--; continue;
        }

        std::string func_rst;
        xl_ecma_utils::deobfuscate_function(splited, func_start, func_end, func_rst);
        final += func_rst;
        i = func_end;
    }
    cell->v = final;

    return BQ::OK;
}

int try_derefrence_sheets(const cfb_ctx_t* ctx, const ooxml_file_t* ooxml, xl_ecma_datas_t* datas) {
    if (ooxml == nullptr || datas == nullptr) {
        return BQ::INVALID_ARG;
    }

    auto& sheets = datas->macro_sheets;
    for (uint32_t i = 0; i < sheets.size(); i++) {
        if (sheets[i].sheet_type != sheet_type_macro) {
            continue;
        }
        auto& cells = sheets[i].cells;
        for (uint32_t j = 0; j < cells.size(); j++) {
            try_derefrence_cell(ctx, ooxml, datas, &cells[j]);
        }
    }

    return BQ::OK;
}

int Ooxml::extract(const cfb_ctx_t* ctx, const ooxml_file_t* ooxml, ifilehandler* cb_file, const char16_t* passwd) {
    if (ctx == nullptr || nullptr == ooxml || nullptr == cb_file || ooxml->file == nullptr
        || ooxml->unzip == nullptr || ooxml->filetype == ft_unknown) {
        return BQ::INVALID_ARG;
    }

    int nret = BQ::OK;
    bool little_endian = Ooxml::is_little_endian;

    /* xx/_rels/settings.xml.rels */
    do {
        if (ooxml->setting_rels < 1 || ooxml->setting_rels > ooxml->zipitem_cnt()) {
            break;
        }

        uint32_t subid = ooxml->setting_rels - 1;
        uint8_t* data = nullptr;
        uint32_t size = 0;
        ON_SCOPE_EXIT([&]() { if (data) free(data); });

        nret = ooxml->unzip->get_item(subid, &data, &size);
        if (nret != BQ::OK) {
            nret = BQ::ERR_UNCOMPRESS;
            return nret;
        }

        std::vector<std::string> templates;
        nret = parse_rels_settings(ctx, data, size, templates);
        if (nret != BQ::OK || templates.size() == 0) {
            break;
        }

        std::string template_path;
        for (auto iter : templates) {
            template_path += iter + "\n";
        }

        std::string name = ooxml->zipitem_path(subid);
        auto embedded = create_embedded(ctx, ooxml, (const uint8_t*)template_path.c_str(), template_path.size(), ft_url, name.c_str());
        if (embedded == nullptr) {
            nret = BQ::ERR_FILE_CREATOR;
            break;
        }
        ON_SCOPE_EXIT([&]() { if (embedded) delete embedded; });
        
        embedded->did = make_ooxml_did(subid+1, 0);
        if (0 > cb_file->handle(ctx, (const file_obj_t*)embedded)) {
            nret = BQ::ERR_USER_INTERRUPT;
            break;
        }
    } while (false);

    if (nret != BQ::OK) {
        return nret;
    }

    /* xx/macrosheets/xxx.xml */
    do {
        if (ooxml->macrosheet_cnt == 0) {
            break;
        }

        xl_ecma_datas_t ooxml_datas;
        nret = parse_workbook_tables(ooxml, &ooxml_datas.ref_mode, ooxml_datas.sheet_tables, ooxml_datas.defined_names, ooxml_datas.sharedstr);
        if (nret != BQ::OK) {
            break;
        }

        auto find_sheet_by_path = [](const std::vector<xl_sheet_table_t>& sheets, const char* path) -> int {
            if (path == nullptr) {
                return -1;
            }
            int nret = -1;
            for (int i = 0; i < (int)sheets.size(); i++) {
                std::string target = std::string("xl/") + sheets[i].target.c_str();
                if (ucs::icmp(target.c_str(), path) == 0) {
                    nret = i;
                    break;
                }
            }
            return nret;
        };

        for (int32_t i = 0, subid = 0; i < ooxml->macrosheet_cnt; i++) {
            if (ooxml->macrosheets[i] < 1 || ooxml->macrosheets[i] > ooxml->zipitem_cnt()) {
                continue;
            }

            subid = ooxml->macrosheets[i] - 1;
            auto sheed_id = find_sheet_by_path(ooxml_datas.sheet_tables, ooxml->zipitem_path(subid));
            if (sheed_id < 0) {
                continue;
            }

            tinyxml2::XMLDocument xml;
            nret = parse_xml_by_subid(ooxml, subid, &xml);
            if (nret != BQ::OK) {
                break;
            }

            xl_sheet_t sheet;
            sheet.sheet_id = sheed_id;
            sheet.sheet_type = sheet_type_macro;
            sheet.ooxml_subid = subid;
            nret = parse_sheetdata(&xml, &sheet);
            if (nret != BQ::OK) {
                break;
            }

            ooxml_datas.macro_sheets.push_back(std::move(sheet));
        }

        if (ctx && ctx->xlm_deobfuscation) {
            try_derefrence_sheets(ctx, ooxml, &ooxml_datas);
        }

        for (uint32_t i = 0; i < ooxml_datas.macro_sheets.size(); i++) {
            if (ooxml_datas.macro_sheets[i].sheet_type != sheet_type_macro) {
                continue;
            }
            const xl_sheet_table_t* sheet_table = nullptr;
            if (ooxml_datas.macro_sheets[i].sheet_id > 0 || ooxml_datas.macro_sheets[i].sheet_id < ooxml_datas.sheet_tables.size()) {
                sheet_table = &ooxml_datas.sheet_tables[ooxml_datas.macro_sheets[i].sheet_id];
            }
            auto subid = ooxml_datas.macro_sheets[i].ooxml_subid;
            auto& cells = ooxml_datas.macro_sheets[i].cells;

            auto embedded = create_embedded(ctx, ooxml, 0, 0, ft_xl4, ooxml->zipitem_path(subid));
            if (embedded == nullptr) {
                nret = BQ::ERR_FILE_CREATOR;
                break;
            }
            ON_SCOPE_EXIT([&]() { if (embedded) delete embedded; });

            std::sort(cells.begin(), cells.end(), [](const xl_cell_t& a, const xl_cell_t& b) {
                if (a.col == b.col) { return a.row < b.row; }
                return a.col < b.col;
            });

            auto macrosheet = embedded->obj.file;
            if (ctx && ctx->write_subfile_header && ctx->header_vba_xlm) {
                std::string header = ctx->header_vba_xlm;
                header += "XLM\n";
                macrosheet->write(0, (const uint8_t*)header.c_str(), header.size());
            }
            for (uint32_t i = 0; i < cells.size(); i++) {
                if (i == 0 || cells[i].col != cells[i - 1].col) {
                    std::string col_header = ctx && ctx->header_xlm_col ? ctx->header_xlm_col : "\n";
                    if (!col_header.empty()) {
                        col_header += sheet_table ? sheet_table->name : "";
                        col_header += ".C" + std::to_string(cells[i].col);
                        col_header += "\t" + (sheet_table ? sheet_table->rid : "");
                        col_header += "\t" + (sheet_table ? sheet_table->target : "");
                    }
                    col_header += "\n";
                    macrosheet->write(-1, (const uint8_t*)col_header.c_str(), col_header.size());
                }
                if (cells[i].is_deobfuscated) {
                    macrosheet->write(-1, (const uint8_t*)cells[i].v.c_str(), cells[i].v.size());
                }
                else if (cells[i].is_func) {
                    macrosheet->write(-1, (const uint8_t*)cells[i].f.c_str(), cells[i].f.size());
                }
                else {
                    macrosheet->write(-1, (const uint8_t*)cells[i].v.c_str(), cells[i].v.size());
                }
                macrosheet->write(-1, (const uint8_t*)"\n", 1);
            }

            embedded->did = make_ooxml_did(subid + 1, 0);
            if (0 > cb_file->handle(ctx, (const file_obj_t*)embedded)) {
                nret = BQ::ERR_USER_INTERRUPT;
                break;
            }
        }
    } while (false);

    if (nret != BQ::OK) {
        return nret;
    }

    /* vbaProject.bin */
    do {
        if (ooxml->vbaproject_id < 1 || ooxml->vbaproject_id > ooxml->zipitem_cnt()) {
            break;
        }

        if (ooxml->vbaproject_id >= 0xFFFF) {
            nret = BQ::ERR_OOXML_VBAPROJECT_ID;
            break;
        }

        uint32_t subid = ooxml->vbaproject_id - 1;
        uint8_t* data = nullptr;
        uint32_t size = 0;
        ON_SCOPE_EXIT([&]() { if (data) free(data); });

        nret = ooxml->unzip->get_item(subid, &data, &size);
        if (nret != BQ::OK) {
            nret = BQ::ERR_UNCOMPRESS;
            break;
        }

        std::string alias = std::string(ooxml->file->alias() ? ooxml->file->alias() : "");
        if (!alias.empty()) { alias += "__"; }
        alias += ooxml->zipitem_path(subid);
#if use_embedded
        auto embedded = create_embedded(ctx, ooxml, data, size, ft_ole, alias.c_str());
        if (embedded == nullptr) {
            nret = BQ::ERR_FILE_CREATOR;
            break;
        }
        auto project_bin = embedded->obj.file;
#else 
        auto project_bin = new MemFile(nullptr, alias.c_str());
        if (0 != project_bin->write(0, data, size)) {
            nret = BQ::ERR_FILE_CREATOR;
            delete project_bin, project_bin = nullptr;
            break;
        }
#endif
        olefile_t vbaproject;
        nret = ole::parse(ctx, project_bin, &vbaproject);
        if (nret != BQ::OK) {
            break;
        }
        vbaproject.container = ooxml;

        ooxml_cb_file_wraper cb_file_wraper(cb_file, subid + 1);
        nret = msvba().extract_vba_project(ctx, &vbaproject, &cb_file_wraper);
    } while (false);

    if (nret != BQ::OK) {
        return nret;
    }

    return nret;
}
