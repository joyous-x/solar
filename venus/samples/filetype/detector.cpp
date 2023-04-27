#pragma once
#include <map>
#include <string>
#include "detector.h"
#include "./npe.h"
#include "../common/file/filetype.h"
#include "../common/office/ole.h"
#include "../common/utils/utils.h"

#define NPE_FILE_SIZE_MAX               0x200000    // 2 MB  
#define NPE_FILE_SIZE_MAX_OFFICE        0xA00000    // 10 MB  
#define NPE_FILE_SIZE_GETTYPE           (2 * 1024 - 1) 

const std::map<uint32_t, const char*> g_filetype2str = {
    { ft_unknown, "unknown" },
    { ft_url, "url" },
    { ft_ace, "ace" },
    { ft_arc, "arc" },
    { ft_ape, "ape" },
    { ft_apm_wmf, "apm_wmf" },
    { ft_arj, "arj" },
    { ft_asf, "asf" },
    { ft_acad, "autocad" },
    { ft_bmp, "bmp" },
    { ft_bzip2, "bzip2" },
    { ft_borland_lib, "borland_lib" },
    { ft_chm, "chm" },
    { ft_ctm, "ctm" },
    { ft_cpio, "cpio" },
    { ft_cur, "cur" },
    { ft_emf, "emf" },
    { ft_email, "email" },
    { ft_gidhlp, "gidhlp" },
    { ft_gif, "gif" },
    { ft_gzip, "gzip" },
    { ft_hta, "hta" },
    { ft_html, "html" },
    { ft_ico, "ico" },
    { ft_ini, "ini" },
    { ft_java_cls, "java_cls" },
    { ft_jpg, "jpg" },
    { ft_lzh, "lzh" },
    { ft_ms_lnk, "lnk" },
    { ft_mdb, "mdb" },
    { ft_mid, "mid" },
    { ft_mp3, "mp3" },
    { ft_ms_dds, "ms_dds" },
    { ft_ms_lib, "ms_lib" },
    { ft_morphine, "morphine" },
    { ft_coff, "ms_coff" },
    { ft_ms_lrh, "ms_lrh" },
    { ft_nsis, "nsis" },
    { ft_oe_dbx, "outlook_express_dbx" },
    { ft_pch, "pch" },
    { ft_pdb_vcx, "pdb_vcx" },
    { ft_pdf, "pdf" },
    { ft_pnf, "pnf" },
    { ft_png, "png" },
    { ft_ms_pif, "pif" },
    { ft_pst, "pst" },
    { ft_rar, "rar" },
    { ft_ms_reg4, "reg4" },
    { ft_ms_reg5, "reg5" },
    { ft_riff, "riff" },
    { ft_rm, "rm" },
    { ft_rpm, "rpm" },
    { ft_swf, "swf" },
    { ft_sbr, "sbr" },
    { ft_sis, "sis" },
    { ft_tar, "tar" },
    { ft_tiff, "tiff" },
    { ft_text, "txt" },
    { ft_uue, "uue" },
    { ft_ms_wma, "wma" },
    { ft_wmf, "wmf" },
    { ft_xml, "xml" },
    { ft_zoo, "zoo" },
    { ft_elf, "elf" },
    { ft_elf32, "elf32" },
    { ft_elf64, "elf64" },
    { ft_elf_x86, "elf_x86" },
    { ft_elf_x64, "elf_x64" },
    { ft_elf_ia64, "elf_ia64" },
    { ft_elf_mips, "elf_mips" },
    { ft_elf_mips64, "elf_mips64" },
    { ft_elf_arm, "elf_arm" },
    { ft_elf_arm64, "elf_arm64" },
    { ft_pe, "pe" },
    { ft_pe_x86, "pe_x86" },
    { ft_pe_x64, "pe_x64" },
    { ft_pe_ia64, "pe_ia64" },
    { ft_pe_arm, "pe_arm" },
    { ft_pe_arm64, "pe_arm64" },
    { ft_os2_ne, "os2_ne" },
    { ft_os2_le, "os2_le" },
    { ft_zip, "zip" },
    { ft_jar, "jar" },
    { ft_apk, "apk" },
    { ft_7z, "7z" },
    { ft_cab, "cab" },
    { ft_rtf, "rtf" },
    { ft_ms_activemine, "ms_activemine" },
    { ft_ms_doc_2003xml, "ms_doc_2003xml" },
    { ft_ms_doc_2007xml, "ms_doc_2007xml" },
    { ft_ms_ppt_2007xml, "ms_ppt_2007xml" },
    { ft_hwp5, "hwp5" },
    { ft_ole, "ole" },
    { ft_ms_doc, "ms_doc" },
    { ft_ms_xls, "ms_xls" },
    { ft_ms_ppt, "ms_ppt" },
    { ft_msi, "ms_installer" },
    { ft_ooxml_encrypted, "ooxml_encrypted" },
    { ft_ooxml, "ooxml" },
    { ft_ms_docx, "ms_docx" },
    { ft_ms_docm, "ms_docm" },
    { ft_ms_dotx, "ms_dotx" },
    { ft_ms_dotm, "ms_dotm" },
    { ft_ms_xlsx, "ms_xlsx" },
    { ft_ms_xlsm, "ms_xlsm" },
    { ft_ms_xltx, "ms_xltx" },
    { ft_ms_xltm, "ms_xltm" },
    { ft_ms_xlsb, "ms_xlsb" },
    { ft_ms_pptx, "ms_pptx" },
    { ft_ms_pptm, "ms_pptm" },
    { ft_ms_potx, "ms_potx" },
    { ft_ms_potm, "ms_potm" },
    { ft_vba, "vba" },
    { ft_xl4, "xl4" },
    { ft_xl4_macros, "xl4_macros" },
    { ft_xl4_drawingroup, "xl4_drawingroup" },
    { ft_xl4_drawing, "xl4_drawing" },
    { ft_macho, "mach-o" },
    { ft_macho_32, "mach-o_32" },
    { ft_macho_64, "mach-o_64" },
    { ft_macho_x86, "mach-o_x86" },
    { ft_macho_x64, "mach-o_x64" },
    { ft_macho_arm, "mach-o_arm" },
    { ft_macho_arm64, "mach-o_arm64" },
    { ft_macho_fat32, "mach-o_fat32"},
    { ft_macho_fat64, "mach-o_fat64"}
};

/*
* @return if detection is ok, return the detected filetype; otherwise, return ftype_raw
*/
int in_depth_ole(ifile* file, int ftype_raw) {
    olefile_t olefile;
    if (0 == ole::parse(nullptr, file, &olefile)) {
        auto new_ftype = ole::filetype(nullptr, &olefile);
        if (new_ftype != ft_unknown) {
            ftype_raw = new_ftype;
        }
    }
    return ftype_raw;
}

int in_depth_npe(ifile* file, const wchar_t* pszRealName, int ftype_raw) {
    if (nullptr == file) {
        return ftype_raw;
    }

    int ftype = ft_unknown;
    char* buffer = nullptr;
    do {
        uint32_t read_bytes = 0;
        uint32_t buffer_bytes = file->size() > NPE_FILE_SIZE_GETTYPE ? NPE_FILE_SIZE_GETTYPE : file->size();

        buffer = new char[buffer_bytes + 1];
        memset(buffer, 0, buffer_bytes + 1);

        if (0 != file->read(0, (uint8_t*)buffer, buffer_bytes, &read_bytes)) {
            break;
        }

        ftype = (DWORD)NpeUtil::AnalyseBuf(buffer, read_bytes, pszRealName, false);
        if (0 == ftype) {
            ftype = (DWORD)NpeUtil::AnalyseBufEx(buffer, read_bytes, pszRealName, false);
        }
    } while (false);

    if (buffer != nullptr) {
        delete[]buffer;
        buffer = nullptr;
    }

    // ��������жϲ��������ٸ����ļ���׺���ж�
    if (ftype == ft_unknown) {
        ftype = NpeUtil::AnalyseFile(pszRealName);
    }

    return ftype == ft_unknown ? ftype_raw : ftype;
}

int filetype::detect_mem(const uint8_t* buffer, uint32_t size) {
    return common_ftype::detect_mem(buffer, size);
}

int filetype::detect(ifile* reader, zip_creator_t z_reader, void* user_data) {
    if (reader == nullptr) {
        return ft_unknown;
    }

    auto ftype = common_ftype::detect(reader);
    if (ftype == ft_ole) {
        ftype = in_depth_ole(reader, ftype);
    }

    if (ftype == ft_unknown) {
        std::wstring filename = XsUtils::utf2uni(reader->alias());
        ftype = in_depth_npe(reader, filename.c_str(), ftype);
    }

    return ftype;
}

const char* filetype::ftype2str(uint32_t ftype) {
    auto iter = g_filetype2str.find(ftype);
    if (iter == g_filetype2str.end()) {
        iter = g_filetype2str.find(ft_unknown);
    }
    return iter->second;
}

int filetype::is_archive_bind(ifile* reader) {
    // TODO: from archa
    return -1;
}

int filetype::is_archive_swc(ifile* reader) {
    // TODO: from archa
    return -1;
}