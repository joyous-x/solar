//
// Created by jiao on 2021/10/13.
//
#include <cassert>
#include <map>
#include <algorithm>
#include "ms_other.h"
#include "../3rds/base64.h"
#include "../3rds/tinyxml2_9.0.0/tinyxml2.h"
#include "./helper.h"


const char* tag_bindata = "binData";
const char* tag_pkg_bindata = "pkg:binaryData";


int ms_other::filetype(const uint8_t* data, uint32_t size) {
    if (data == nullptr || 0 == size) {
        return ft_unknown;
    }

    /*
    * TODO:
    * 
    * Check if it is a SLK / SYLK file - https://en.wikipedia.org/wiki/SYmbolic_LinK_(SYLK)
    *   It must start with "ID" in uppercase, no whitespace or newline allowed before by Excel :
    *       if data.startswith(b'ID') : self.open_slk(data)
    */

    tinyxml2::XMLDocument xml_doc;
    int nret = xml_doc.Parse((const char*)data, size);
    if (nret != tinyxml2::XML_SUCCESS) {
        return ft_unknown;
    }

    nret = ft_unknown;
    if (0 != ucs::icmp(xml_doc.RootElement()->Name(), "pkg:package")) {
        return nret;
    }

    auto attr_type = xml_doc.RootElement()->FindAttribute("xmlns:pkg");
    if (attr_type == nullptr || attr_type->Value() == nullptr) {
        return nret;
    }

    do {
        if (0 != ucs::icmp(attr_type->Value(), ms_other::xmlns_wordml)) {
            break;
        }

        for (const tinyxml2::XMLNode* current = xml_doc.FirstChild(); current; current = current->NextSibling()) {
            auto mso_header = current->ToDeclaration();

            if (mso_header == nullptr || nullptr == mso_header->Value()) {
                continue;
            }

            if (strstr(mso_header->Value(), mso_xml_head) && strstr(mso_header->Value(), xml_doc_head)) {
                nret = ft_ms_doc_2003xml;
                break;
            }
        }
    } while (false);

    if (nret != ft_unknown) {
        return nret;
    }

    do {
        if (0 != ucs::icmp(attr_type->Value(), ms_other::xmlns_off2007)) {
            break;
        }

        for (const tinyxml2::XMLNode* current = xml_doc.FirstChild(); current; current = current->NextSibling()) {
            auto mso_header = current->ToDeclaration();

            if (mso_header == nullptr || nullptr == mso_header->Value()) {
                continue;
            }

            if (strstr(mso_header->Value(), mso_xml_head)) {
                if (strstr(mso_header->Value(), xml_ppt_head)) {
                    nret = ft_ms_ppt_2007xml;
                    break;
                }
                else if (strstr(mso_header->Value(), xml_doc_head)) {
                    nret = ft_ms_ppt_2007xml;
                    break;
                }
            }
        }
    } while (false);
    
    if (nret != ft_unknown) {
        return nret;
    }

    return nret;
}

int parse_word2003xml(const ctx_t* ctx, const file_obj_t* xml, ifilehandler* cb_file) {
    if (xml == nullptr || xml->file == nullptr || cb_file == nullptr) {
        return BQ::INVALID_ARG;
    }

    tinyxml2::XMLDocument xml_doc;
    int nret = xml_doc.Parse((const char*)xml->file->mapping(), (uint32_t)xml->file->size());
    if (nret != tinyxml2::XML_SUCCESS) {
        return BQ::ERR_PARSE_XML;
    }

    if (ucs::icmp(xml_doc.RootElement()->Name(), "pkg:package") || nullptr == xml_doc.RootElement()->FindAttribute("xmlns:pkg")) {
        return BQ::ERR_FORMAT;
    }

    nret = BQ::OK;
    for (const tinyxml2::XMLElement* current = xml_doc.RootElement()->FirstChildElement(); current; current = current->NextSiblingElement()) {
        if (0 != ucs::icmp(current->Name(), tag_bindata)) {
            continue;
        }

        // here, we may get mso(ActiveMine) files
        auto attr_name = current->FindAttribute("pkg:name") ? current->FindAttribute("pkg:name")->Value() : "noname.mso.bin";

        size_t raw_size = 0;
        auto raw = base64_decode((const unsigned char*)current->Value(), ucs::len(current->Value()), &raw_size);
        if (raw == nullptr) {
            continue;
        }
        ON_SCOPE_EXIT([&]() { if (raw) free(raw); });

        auto embedded = create_embedded(ctx, xml, (const uint8_t*)raw, raw_size, ft_unknown, attr_name);
        if (embedded) {
            if (0 > cb_file->handle(ctx, (const file_obj_t*)embedded)) {
                nret = BQ::ERR_USER_INTERRUPT;
            }
            delete embedded, embedded = nullptr;
        }
        else {
            nret = BQ::ERR_FILE_CREATOR;
        }

        if (nret != BQ::OK) {
            break;
        }
    }

    return nret; 
}

int parset_flatopc(const ctx_t* ctx, const file_obj_t* xml, ifilehandler* cb_file) {
    if (xml == nullptr || xml->file == nullptr || cb_file == nullptr) {
        return BQ::INVALID_ARG;
    }

    tinyxml2::XMLDocument xml_doc;
    int nret = xml_doc.Parse((const char*)xml->file->mapping(), (uint32_t)xml->file->size());
    if (nret != tinyxml2::XML_SUCCESS) {
        return BQ::ERR_PARSE_XML;
    }

    nret = BQ::OK;
    if (ucs::icmp(xml_doc.RootElement()->Name(), "pkg:package") || nullptr == xml_doc.RootElement()->FindAttribute("xmlns:pkg")) {
        return BQ::ERR_FORMAT;
    }

    for (const tinyxml2::XMLElement* current = xml_doc.RootElement()->FirstChildElement(); current; current = current->NextSiblingElement()) {
        if (0 != ucs::icmp(current->Name(), "pkg:part")) {
            continue;
        }

        auto attr_name = current->FindAttribute("pkg:name") ? current->FindAttribute("pkg:name")->Value() : "noname.mso.bin";
        // here, we may get vbaProject files which has contentType == "application/vnd.ms-office.vbaProject".
        // To simplify the process, we can only process the vbaProject item.
        auto attr_type = current->FindAttribute("pkg:contentType") ? current->FindAttribute("pkg:contentType")->Value() : "unknown";

        for (const tinyxml2::XMLElement* sub_l1 = current->FirstChildElement(); sub_l1; sub_l1 = sub_l1->NextSiblingElement()) {
            if (0 != ucs::icmp(sub_l1->Name(), tag_pkg_bindata)) {
                continue;
            }

            auto data = sub_l1->GetText();
            size_t raw_size = 0;
            auto raw = base64_decode((const unsigned char*)data, ucs::len(data), &raw_size);
            if (raw == nullptr) {
                continue;
            }
            ON_SCOPE_EXIT([&]() { if (raw) free(raw); });

            auto embedded = create_embedded(ctx, xml, (const uint8_t*)raw, raw_size, ft_unknown, attr_name);
            if (embedded) {
                if (0 > cb_file->handle(ctx, (const file_obj_t*)embedded)) {
                    nret = BQ::ERR_USER_INTERRUPT;
                }
                delete embedded, embedded = nullptr;
            }
            else {
                nret = BQ::ERR_FILE_CREATOR;
            }
            
            if (nret != BQ::OK) {
                break;
            }
        }
    }

    return nret;
}

int ms_other::parse_xml(const ctx_t* ctx, const file_obj_t* xml, ifilehandler* cb_file) {
    if (xml == nullptr || xml->file == nullptr || cb_file == nullptr) {
        return BQ::INVALID_ARG;
    }

    auto nret = BQ::OK;
    auto xml_filetype = xml->filetype;
    if (xml_filetype == ft_unknown || xml_filetype >= ft_ignore) {
        xml_filetype = filetype(xml->file->mapping(), (uint32_t)xml->file->size());
    }

    switch (xml_filetype) {
    case ft_ms_doc_2003xml: {
        nret = parse_word2003xml(ctx, xml, cb_file);
        break;
    }
    case ft_ms_doc_2007xml:
    case ft_ms_ppt_2007xml: {
        // Flat OPC : must contain the namespace xmlns_off2007
        nret = parset_flatopc(ctx, xml, cb_file);
        break;
    }
    default:
        nret = BQ::ERR_FORMAT;
    }

    return nret;
}