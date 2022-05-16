#pragma once

/*
* filetype_t = 0x76543210
* 
* |  0   1  |  2   3  |   4   |  5  6  7  |
* |---------|---------|-------|-----------|
* |  value  | subtype |  flag |    type   |
* 
* type    : 支持位与(&)操作
* flag    : 支持位与(&)操作，目前用作判断是否压缩包，(filetype & 0x000F0000) == ft_archive
* subtype : 子类型，递增，判定方式如，(filetype & 0x0000FF00) == ft_pe
* value   : 在单个 subtype 范围内，不可重复
* 
* NOTE:
*   1. subtype + value 联合即可唯一确定一个类型
*/

typedef enum __em_filetype {
    ft_unknown = 0x00000000,
    /// <summary>
    /// type : 0xFFF00000
    /// </summary>
    ft_none = ft_unknown,
    ft_windows = 0x0010 << 16,
    ft_linux = 0x0020 << 16,
    ft_mac = 0x0040 << 16,
    ft_android = 0x0080 << 16,
    ft_office = 0x0100 << 16,
    ft_media = 0x0200 << 16,
    ft_java = 0x0400 << 16,
    ft_script = 0x0800 << 16,
    ft_ios = 0x1000 << 16,
    /// <summary>
    /// flags : 0x000F0000
    /// </summary>
    ft_archive = 0x0001 << 16,

    /// <summary>
    /// subtype : 0x0000FF00
    /// </summary>
    ft_pe = 0x0100,
    ft_dos = 0x0200,
    ft_link = 0x0300,
    ft_coff = 0x0400,
    ft_elf = 0x0500,
    ft_macho = 0x0600,
    ft_ole = 0x0700,
    ft_ooxml = 0x0800,
    ft_document = 0x0900,
    ft_picture = 0x0A00,
    ft_sound = 0x0B00,
    ft_video = 0x0C00,
    ft_flash = 0x0D00,
    ft_data = 0x0E00,
    ft_text = 0x0F00,
    ft_arch_base = ft_archive + 0x8000,

    /// pe
    ft_pe_x86 = ft_windows + ft_pe + 0x01,
    ft_pe_x64,
    ft_pe_ia64,
    ft_pe_arm,
    ft_pe_arm64,
    ft_os2_ne = ft_windows + ft_pe + 0x0A,
    ft_os2_le = ft_windows + ft_pe + 0x0B,
    ft_os2_lx = ft_windows + ft_pe + 0x0C,
    /// dos
    ft_ms_dos = ft_windows + ft_dos + 0x01,
    /// lnk
    ft_ms_lnk = ft_windows + ft_link + 0x01,
    ft_ms_pif,
    /// elf
    ft_elf32 = ft_linux + ft_elf + 0x01,
    ft_elf64,
    ft_elf_x86,
    ft_elf_x64,
    ft_elf_ia64,
    ft_elf_mips,
    ft_elf_mips64,
    ft_elf_arm,
    ft_elf_arm64,
    /// mach-o
    ft_macho_32 = ft_mac + ft_macho + 0x01,
    ft_macho_64,
    ft_macho_x86,
    ft_macho_x64,
    ft_macho_arm,
    ft_macho_arm64,
    ft_macho_fat32,
    ft_macho_fat64,
    /// ole 
    ft_msi = ft_windows + ft_ole + 0x01,
    ft_hwp5 = ft_office + ft_ole + 0x02,
    ft_ms_doc,
    ft_ms_xls,
    ft_ms_ppt,
    ft_ms_activemine,
    ft_ooxml_encrypted,
    /// ooxml 
    ft_ms_docx = ft_office + ft_ooxml + 0x01,
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
    ft_rtf = ft_office + ft_document + 0x01,
    ft_pdf,
    ft_wps,
    /// archive
    ft_cab = ft_arch_base + 0x0200,
    ft_gzip = ft_arch_base + 0x0300,
    ft_arj = ft_arch_base + 0x0400,
    ft_zip = ft_arch_base + 0x0500,
    ft_apk = ft_android + ft_zip + 0x04,
    ft_jar = ft_zip + 0x05,
    ft_rar = ft_arch_base + 0x0600,
    ft_rar5 = ft_arch_base + 0x0601,
    ft_7z = ft_arch_base + 0x0700,
    ft_ace = ft_arch_base + 0x0800,
    ft_sis = ft_arch_base + 0x0900,
    ft_bzip2 = ft_arch_base + 0x0A00,
    ft_chm = ft_arch_base + 0x0B00,
    ft_cpio = ft_arch_base + 0x0C00,
    ft_lzh = ft_arch_base + 0x0D00,
    ft_nsis = ft_windows + ft_arch_base + 0x0E00,
    ft_tar = ft_arch_base + 0x0F00,
    // ? = ft_archive + 0x1000,
    ft_iso = ft_arch_base + 0x1100,
    ft_inno = ft_arch_base + 0x1200,
    ft_mscompress = ft_arch_base + 0x1300,
    ft_autoit = ft_windows + ft_arch_base + 0x1400,
    ft_oe_dbx = ft_arch_base + 0x1500,
    ft_compound = ft_arch_base + 0x1600,
    ft_z = ft_arch_base + 0x1700,
    ft_wim = ft_arch_base + 0x1800,
    ft_xar = ft_arch_base + 0x1900,
    ft_udf = ft_arch_base + 0x1A00,
    ft_uue = ft_arch_base + 0x1B00,
    ft_zoo = ft_arch_base + 0x1C00,
    ft_hfs = ft_arch_base + 0x1D00,
    /// archive : linux
    ft_rpm = ft_linux + ft_arch_base + 0x4100,
    ft_deb = ft_linux + ft_arch_base + 0x4200,
    /// media : picture 
    ft_ms_dds = ft_media + ft_picture + 0x01,
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
    ft_ape = ft_media + ft_sound + 0x01,
    ft_aiff,
    ft_flac,
    ft_mid,
    ft_mp3,
    ft_ms_wma,
    /// media : video 
    ft_asf = ft_media + ft_video + 0x01,
    ft_flv,
    ft_rm,
    ft_mkv,
    ft_mpeg,
    ft_ms_wmv,
    ft_ms_asf,
    /// media : flash 
    ft_swf = ft_media + ft_flash + 0x01,
    /// data : other
    ft_java_cls = ft_java + ft_data + 0x01,
    ft_ms_lib = ft_none + ft_data + 0x11,
    ft_borland_lib,
    ft_mdb,
    ft_pdb_vcx,
    ft_pnf,
    ft_pst,
    ft_pcap,
    ft_sbr,
    /// text : script
    ft_html = ft_script + ft_text + 0x01,
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
    ft_ms_doc_2003xml = ft_office + ft_text + 0x51,
    ft_ms_doc_2007xml,
    ft_ms_ppt_2007xml,
    /// text : other
    ft_ms_hxq = ft_none + ft_text + 0x61,
    ft_ini,
    ft_pch,
    ft_xml,

    /// <summary>
    /// other : other
    /// </summary>
    ft_url = ft_none + ft_none + 0x01,
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
    ft_xl4 = ft_none + ft_none + 0x80,
    ft_xl4_macros,
    ft_xl4_drawingroup,
    ft_xl4_drawing,

    ft_ignore = 0x7FFFFFFF,                /* greater than ft_ignore, should be ignored. */
} filetype_t;
