//
// Created by jiao on 2021/10/13.
//

#ifndef COMMON_TEST_MS_OFFXML_H
#define COMMON_TEST_MS_OFFXML_H
#include "cfb_base.h"

//////////////////////////////////////////////////////////////////////////
//
// reference:
//      https://docs.microsoft.com/en-us/archive/msdn-magazine/2003/november/the-xml-files-xml-in-microsoft-office-word-2003
//
//////////////////////////////////////////////////////////////////////////

class ms_other {
public:
    /**
     * @brief check xml filetype
     *
     * @param data                  
     * @param size
     * @return int    return filetype detected
     */
    int filetype(const uint8_t* data, uint32_t size);

    /**
     * @brief extract embedded objects from mso xml files
     *
     * @param ctx                   optional, the workflow context
     * @param xml                  
     * @param cb_file
     * @return int
     */
    int parse_xml(const ctx_t* ctx, const file_obj_t* xml, ifilehandler* cb_file);

protected:
    const char* mso_xml_head = "mso-application";
    const char* xmlns_wordml = "http://schemas.microsoft.com/office/word/2003/wordml";
    const char* xmlns_off2007 = "http://schemas.microsoft.com/office/2006/xmlPackage";

    /*
    * Word 2003 XML: 
    *   1. XML 文件，暂不处理
    *   2. MS WORD XML 文档 : word2003/2007+
    * 
    * MS WORD XML:
    *   a. the tag <w:binData w:name="editdata.mso"> contains the VBA macro code.
    *   b. doc/docx : <?mso-application progid="Word.Document"?>
    */
    const char* xml_doc_head = "progid=\"Word.Document\"";
    /*
    * PowerPoint 2007+ XML: 
    *   1. MS PowerPoint XML 演示文稿
    * 
    * ppt/pptx : <?mso-application progid="PowerPoint.Show"?>
    */
    const char* xml_ppt_head = "progid=\"PowerPoint.Show\"";
    /*
    * Excel XML: 
    *   1. XML 文件，暂不处理
    * 
    * xls/xlsx : <?mso-application progid="Excel.Sheet"?>
    */
};

#endif //COMMON_TEST_MS_OFFXML_H
