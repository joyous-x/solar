// filetyper.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "gtest/gtest.h"
#include "../filetype/detector.h"
#include "./helper.h"
#include "./simple_helper.h"


#define dir_sample "../sample/"


izip* ziper_creator(ifile* file, bool write_mode, void* user_data) {
    auto ziper = (iziper*)user_data;
    if (ziper && 0 == ziper->parse(file)) {
        ziper->addref();
        return ziper;
    }
    return nullptr;
}

int get_ftype(ifile* file) {
    if (file == nullptr) {
        return BQ::INVALID_ARG;
    }

    ZipHelper* ziper = new ZipHelper();
    ON_SCOPE_EXIT([&] { if (ziper) ziper->release(); });
    return filetype::detect(file, ziper_creator, ziper);
}

int get_ftype(const char* filepath, iziper* ziper = nullptr) {
    ifile* file = openfile(filepath, "rb");
    if (file == nullptr) {
        return ft_unknown;
    }
    ON_SCOPE_EXIT([&]() { if (file) file->release(); });

    return get_ftype(file);
}


#define MY_SMAPLE_T(t) const char* sample_##t = "sample."#t;

#define MY_TEST_T(t) TEST(filetype, ft_##t) { \
    std::string filepath = std::string(dir_sample) + sample_##t; \
    ZipHelper ziper; \
    ASSERT_EQ(get_ftype(filepath.c_str(), &ziper), ft_##t); \
}

#define MY_SMAPLE_TN(t, n) const char* sample_##t##n = "sample."#n"."#t;

#define MY_TEST_TN(t, n) TEST(filetype, ft_##t##n) { \
    std::string filepath = std::string(dir_sample) + sample_##t##n; \
    ZipHelper ziper; \
    ASSERT_EQ(get_ftype(filepath.c_str(), &ziper), ft_##t); \
}

#define MY_CHECKER(t) MY_SMAPLE_T(t); MY_TEST_T(t);
#define MY_CHECKER_EX(t, n) MY_SMAPLE_TN(t, n); MY_TEST_TN(t, n);

MY_CHECKER_EX(apk, a)
MY_CHECKER_EX(cab, a)
MY_CHECKER_EX(cab, b)
MY_CHECKER(ace)
MY_CHECKER(arj)
MY_CHECKER(autoit) // ft_autoit ---> ft_pe_x86
MY_CHECKER(bzip2)
MY_CHECKER(cpio)
MY_CHECKER(chm)
MY_CHECKER(deb)
MY_CHECKER(elf_x64)
MY_CHECKER(oe_dbx)
MY_CHECKER(dmg)
MY_CHECKER(gzip)
MY_CHECKER(gif)
MY_CHECKER(hfs) // ft_hfs 无样本
MY_CHECKER(html)
MY_CHECKER(iso)
MY_CHECKER(inno) // ft_inno ---> ft_pe_x86
MY_CHECKER(jar)
MY_CHECKER(java_cls)
MY_CHECKER(lzh)
MY_CHECKER(ms_lib)
MY_CHECKER(ms_lnk)
MY_CHECKER(macho_arm64)
MY_CHECKER(macho_x64)
MY_CHECKER(macho_fat32)
MY_CHECKER(msi)
MY_CHECKER(ms_docx)
MY_CHECKER(ms_pptx)
MY_CHECKER(ms_xlsx)
MY_CHECKER(ms_doc)
MY_CHECKER(ms_ppt)
MY_CHECKER(ms_xls)
MY_CHECKER(mscompress)
MY_CHECKER(nsis) // ft_nsis ---> ft_pe_x86
MY_CHECKER(pe_x86)
MY_CHECKER(pe_x64)
MY_CHECKER(png)
MY_CHECKER(pdf)
MY_CHECKER(pdb_vcx)
MY_CHECKER(rpm)
MY_CHECKER_EX(rar, 15) // rar v4
MY_CHECKER_EX(rar5, 50) // rar v5
MY_CHECKER(udf)
MY_CHECKER(swc)
MY_CHECKER(tar)
MY_CHECKER(xar)
MY_CHECKER(xml)
MY_CHECKER(wim)
MY_CHECKER(z)
MY_CHECKER(ttf)
MY_CHECKER(ini)

MY_CHECKER(swf)
MY_CHECKER(ogg)

TEST(filetype_text, text_encoding) {
    std::string filepath = std::string(dir_sample) + "sample.text";

    ifile* file = openfile(filepath.c_str(), "rb");
    ON_SCOPE_EXIT([&]() { if (file) file->release(); });
    ASSERT_TRUE(file != nullptr);

    uint8_t* buffer = nullptr;
    uint32_t buffer_size = 2 * 1024 * 1024;
    ON_SCOPE_EXIT([&]() { if (buffer) { free(buffer); buffer = nullptr; } });

    uint32_t size = (uint32_t)file->size();
    const uint8_t* data = file->mapping();
    if (data == nullptr) {
        buffer_size = std::min<uint32_t>(buffer_size, size);
        buffer = (uint8_t*)malloc(buffer_size);
        ASSERT_TRUE(buffer != nullptr);

        memset(buffer, 0, buffer_size);
        auto nRet = file->read(0, buffer, buffer_size, &buffer_size);
        ASSERT_TRUE(nRet >= 0);

        data = buffer;
        size = buffer_size;
    }

    extern int detect_text_type(const uint8_t * data, uint32_t size);
    detect_text_type(data, size);
}

TEST(check_val_unique, check_em_filetype) {
    uint32_t fts[] = {
        /// <summary>
        /// type : 0xFFF00000
        /// </summary>
        ft_other,
        ft_windows,
        ft_linux,
        ft_mac,
        ft_android,
        ft_office,
        ft_media,
        ft_java,
        ft_script,
        ft_ios,

        /// <summary>
        /// subtype flags : 0x000F0000
        /// </summary>
        ft_archive,

        /// <summary>
        /// subtype : 0x0000FF00
        /// </summary>
        ft_pe,
        ft_dos,
        ft_link,
        ft_coff,
        ft_elf,
        ft_macho,
        ft_ole,
        ft_ooxml,
        ft_document,
        ft_picture,
        ft_sound,
        ft_video,
        ft_flash,
        ft_data,
        ft_text,

        /// pe
        ft_pe_x86,
        ft_pe_x64,
        ft_pe_ia64,
        ft_pe_arm,
        ft_pe_arm64,
        ft_os2_ne,
        ft_os2_le,
        ft_os2_lx,
        /// dos
        ft_ms_dos,
        /// lnk
        ft_ms_lnk,
        ft_ms_pif,
        /// elf
        ft_elf32,
        ft_elf64,
        ft_elf_x86,
        ft_elf_x64,
        ft_elf_ia64,
        ft_elf_mips,
        ft_elf_mips64,
        ft_elf_arm,
        ft_elf_arm64,
        /// mach-o
        ft_macho_32,
        ft_macho_64,
        ft_macho_x86,
        ft_macho_x64,
        ft_macho_arm,
        ft_macho_arm64,
        ft_macho_fat32,
        ft_macho_fat64,
        /// ole 
        ft_msi,
        ft_hwp5,
        ft_ms_doc,
        ft_ms_xls,
        ft_ms_ppt,
        ft_ms_activemine,
        ft_ooxml_encrypted,
        /// ooxml 
        ft_ms_docx,
        ft_ms_docm,
        ft_ms_dotx,
        ft_ms_dotm,
        ft_ms_xlsx,
        ft_ms_xlsm,
        ft_ms_xltx,
        ft_ms_xltm,
        ft_ms_xlsb,
        ft_ms_pptx,
        ft_ms_pptm,
        ft_ms_potx,
        ft_ms_potm,
        /// document 
        ft_rtf,
        ft_pdf,
        ft_wps,
        /// archive
        ft_cab,
        ft_gzip,
        ft_arj,
        ft_zip,
        ft_apk,
        ft_jar,
        ft_rar,
        ft_rar5,
        ft_7z,
        ft_ace,
        ft_sis,
        ft_bzip2,
        ft_chm,
        ft_cpio,
        ft_lzh,
        ft_nsis,
        ft_tar,
        // ?,
        ft_iso,
        ft_inno,
        ft_mscompress,
        ft_autoit,
        ft_oe_dbx,
        ft_compound,
        ft_z,
        ft_wim,
        ft_xar,
        ft_udf,
        ft_uue,
        ft_zoo,
        ft_hfs,
        /// archive : linux
        ft_rpm,
        ft_deb,
        /// media : picture 
        ft_ms_dds,
        ft_apm_wmf,
        ft_acad,
        ft_bmp,
        ft_cur,
        ft_emf,
        ft_gif,
        ft_ico,
        ft_jpg,
        ft_png,
        ft_riff,
        ft_tiff,
        ft_wmf,
        /// media : sound 
        ft_ape,
        ft_aiff,
        ft_flac,
        ft_mid,
        ft_mp3,
        ft_ms_wma,
        /// media : video 
        ft_asf,
        ft_flv,
        ft_rm,
        ft_mkv,
        ft_mpeg,
        ft_ms_wmv,
        ft_ms_asf,
        /// media : flash 
        ft_swf,
        /// data : other
        ft_java_cls,
        ft_ms_lib,
        ft_borland_lib,
        ft_mdb,
        ft_pdb_vcx,
        ft_pnf,
        ft_pst,
        ft_pcap,
        ft_sbr,
        /// text : script
        ft_html,
        ft_htm,
        ft_hta,
        ft_js,
        ft_vbs,
        ft_wsf,
        ft_bat,
        ft_powershell,
        ft_php,
        ft_vba,
        ft_vba_pcode,
        /// text : office
        ft_ms_doc_2003xml,
        ft_ms_doc_2007xml,
        ft_ms_ppt_2007xml,
        /// text : other
        ft_ms_hxq,
        ft_ini,
        ft_pch,
        ft_xml,

        /// <summary>
        /// other : other
        /// </summary>
        ft_url,
        ft_arc,
        ft_ctm,
        ft_dmg,
        ft_email,
        ft_gidhlp,
        ft_morphine,
        ft_ms_reg4,
        ft_ms_reg5,
        ft_ms_hiv,
        ft_ms_lrh,
        ft_ms_urlcache,
        ft_pcx,
        ft_swc,
        ft_sqlite,
        ft_ttc,
        ft_ttf,
        ft_ogg,
        /// vba & xl4
        ft_xl4,
        ft_xl4_macros,
        ft_xl4_drawingroup,
        ft_xl4_drawing,
    };

    std::set<uint32_t> setVals;
    for (auto i : fts) setVals.insert(i);
    ASSERT_EQ(setVals.size(), sizeof(fts) / sizeof(fts[0]));
}