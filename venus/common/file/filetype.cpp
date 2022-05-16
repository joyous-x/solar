#include <assert.h>
#include <stdint.h>
#include "filetype.h"
#include "../utils/scope_guard.h"
#include "../utils/utils.h"
#include "../3rds/crc16.h"
#include "./struct_nt/ntimage.h"
#include "./struct_ace/all.h"
#include "./struct_elf/elf.h"
#include "./struct_ole.h"
#include "./struct_mach/macho.h"
#include "./struct_mach/fat.h"
#include "./struct_java_cls/java_cls.h"
#include "./zip_parser_simple.h"
#include "./chartype.h"

const uint8_t MAGIC_MS_CAB[] = { 0x4D, 0x53, 0x43, 0x46 }; /* MSCF, winrar can open it */
const uint8_t MAGIC_WIM[] = { 0x4D, 0x53, 0x57, 0x49, 0x4D, 0x00, 0x00, 0x00 }; /* "MSWIM\0\0\0", wim */
/* "!<arch>\x0Adebian-binary   " */
const uint8_t MAGIC_DEB[] = {
    0x21, 0x3C, 0x61, 0x72, 0x63, 0x68, 0x3E, 0x0A, 0x64, 0x65, 0x62, 0x69, 0x61, 0x6E, 0x2D, 0x62, 0x69, 0x6E, 0x61, 0x72, 0x79, 0x20, 0x20, 0x20
};
/*
* MAGIC_MS_DDS : "DDS \x7C\0\0\0", Windows Imaging Component (WIC)
*   https://docs.microsoft.com/en-us/windows/win32/wic/dds-format-overview
*/
const uint8_t MAGIC_MS_DDS[] = { 0x44, 0x44, 0x53, 0x20, 0x7C, 0, 0, 0 };
const uint8_t MAGIC_MSLIB[] = { 0x21, 0x3C, 0x61, 0x72, 0x63, 0x68, 0x3E, 0x0A, 0x2F }; /* !<arch>x0A/ */
/*
* MSURLCACHE : "Client UrlCache [MMF Ver 5.2\0]"
*   http://www.milincorporated.com/a3_index.dat.html
*/ 
const uint8_t MAGIC_MSURLCACHE[] = { 0x43, 0x6C, 0x69, 0x65, 0x6E, 0x74, 0x20, 0x55, 0x72, 0x6C, 0x43, 0x61, 0x63, 0x68, 0x65, 0x20 };
const uint8_t MAGIC_COFF_OBJ[] = { 0x4C, 0x01 };
const uint8_t MAGIC_RTF[] = { 0x7B, 0x5C, 0x72, 0x74, }; /* rtf */
const uint8_t MAGIC_MSO_ACTIVEMINE[] = "ActiveMime";
const uint8_t MAGIC_7ZIP[] = { 0x37, 0x7A, 0xBC, 0xAF, 0x27, 0x1C };
const uint8_t MAGIC_APE[] = { 0x4D, 0x41, 0x43 };
const uint8_t MAGIC_APM_WMF[] = { 0xD7, 0xCD, 0xC6, 0x9A }; /* apm/wmf */
const uint8_t MAGIC_ARJ[] = { 0x60, 0xEA };
const uint8_t MAGIC_ASF[] = { 0x39, 0x36, 0x38, 0x46, 0x30, 0x34, 0x45, 0x39 };
const uint8_t MAGIC_AutoCAD_A[] = { 0x41, 0x43, 0x31, 0x30, 0x31, 0x35, 0, 0, 0, 0, 0, 0xD };
const uint8_t MAGIC_AutoCAD_B[] = { 0x41, 0x43, 0x31, 0x30, 0x31, 0x38, 0, 0, 0, 0, 0, 0x4C };
const uint8_t MAGIC_AutoIt[] = { 0xA3, 0x48, 0x4B, 0xBE, 0x98, 0x6C, 0x4A, 0xA9, 0x99, 0x4C, 0x53, 0x0A, 0x86, 0xD6, 0x48, 0x7D };
const uint8_t MAGIC_BorlandLIB[] = { 0xF0, 0x0D, 0x00, 0x00 };
const uint8_t MAGIC_CTM[] = { 0x4D, 0x4C, 0x43, 0x54 }; // MLCT
const uint8_t MAGIC_ELF[] = { 0x7F, 0x45, 0x4C, 0x46 };
// https://www.gidhome.com/whats-gid/
const uint8_t MAGIC_GIDHLP[] = { 0x3F, 0x5F, 0x03, 0x00 };
const uint8_t MAGIC_GIF_A[] = { 0x47, 0x49, 0x46, 0x38, 0x37, 0x61 };
const uint8_t MAGIC_GIF_B[] = { 0x47, 0x49, 0x46, 0x38, 0x39, 0x61 };
// "ITOLITLS", HXQ is a file extension commonly associated with Microsoft Help Merged Query Index Format files.
const uint8_t MAGIC_MS_HXQ[] = { 0x49, 0x54, 0x4F, 0x4C, 0x49, 0x54, 0x4C, 0x53 };
const uint8_t MAGIC_HTA[] = { 0x4D, 0x53, 0x46, 0x54 }; // MSFT
// "REGEDIT4\x0D\x0A"
const uint8_t MAGIC_REG4_B[] = { 0x52, 0x45, 0x47, 0x45, 0x44, 0x49, 0x54, 0x34 };
// "Windows Registry Editor Version 5.00\x0D\x0A"
const uint8_t MAGIC_REG5[] = { 0x57, 0x69, 0x6E, 0x64, 0x6F, 0x77, 0x73, 0x20, 0x52, 0x65, 0x67, 0x69, 0x35, 0x2E, 0x30, 0x30 };
/*
* "regf", hive, Windows registry file format specification
*   https://github.com/msuhanov/regf/blob/master/Windows%20registry%20file%20format%20specification.md
* MSNTUSER_DAT
*/
const uint8_t MAGIC_MS_HIVE[] = { 0x72, 0x65, 0x67, 0x66 };
const uint8_t MAGIC_JAVA_CLS[] = { 0xCA, 0xFE, 0xBA, 0xBE };
const uint8_t MAGIC_LNK[] = { 0x4C, 0, 0, 0, 0x01, 0x14, 0x02, 0, 0, 0, 0, 0, 0xC0, 0, 0, 0, 0, 0, 0, 0x46 };
/*
* MSAccessFile
*   "Standard Jet DB\0\0/1\0\0\0": mdb
*   "Standard ACE DB\0\2\0\0\0": accdb
*/
const uint8_t MAGIC_MDB[] = { 0x53, 0x74, 0x61, 0x6E, 0x64, 0x61, 0x72, 0x64, 0x20, 0x4A, 0x65, 0x74, 0x20, 0x44, 0x42, 0 };
const uint8_t MAGIC_MID[] = { 0x4D, 0x54, 0x68, 0x64, 0x00, 0x00, 0x00, 0x06 }; // "MThd\0\0\0\6", MIDI
const uint8_t MAGIC_NSIS[] = { 0xEF, 0xBE, 0xAD, 0xDE, 0x4E, 0x75, 0x6C, 0x6C, 0x73, 0x6F, 0x66, 0x74, 0x49, 0x6E, 0x73, 0x74 };
const uint8_t MAGIC_PCH[] = { 0x56, 0x43, 0x50, 0x43, 0x48 }; // "VCPCH0", pch
const uint8_t MAGIC_PNF[] = { 0x01, 0x01, 0x02, 0x00 };
// "\x89PNG\x0D\x0A\x1A\x0A\0\0\0\x0DIHDR"
const uint8_t MAGIC_PNG[] = { 0x89, 0x50, 0x4E, 0x47, 0xD, 0xA, 0x1A, 0xA, 0, 0, 0, 0xD, 0x49, 0x48, 0x44, 0x52 };
const uint8_t MAGIC_PST[] = { 0x21, 0x42, 0x44, 0x4E };
const uint8_t MAGIC_REG4_A[] = { 0xFF, 0xFE, 0x57, 0, 0x69, 0, 0x6E, 0 };
const uint8_t MAGIC_RIFF[] = { 0x52, 0x49, 0x46, 0x46 };
// https://wiki.multimedia.cx/index.php?title=RealMedia
const uint8_t MAGIC_RM_A[] = { 0x2E, 0x52, 0x4D, 0x46 };
const uint8_t MAGIC_RM_B[] = { 0x66, 0x6D, 0x72, 0x2E };
const uint8_t MAGIC_RPM[] = { 0xED, 0xAB, 0xEE, 0xDB };
// "SQLite format 3\0", https://www.sqlite.org/fileformat.html
const uint8_t MAGIC_SQLITE3[] = { 0x53, 0x51, 0x4C, 0x69, 0x74, 0x65, 0x20, 0x66, 0x6F, 0x72, 0x6D, 0x61, 0x74, 0x20, 0x33, 0x00 };
// sbr : 0x00 0x02/0x03 0x00 0x07, 
const uint8_t MAGIC_SBR_A[] = { 0x0, 0x2, 0x0, 0x7 };
const uint8_t MAGIC_SBR_B[] = { 0x0, 0x3, 0x0, 0x7 };
const uint8_t MAGIC_SIS9[] = { 0x7A, 0x1A, 0x20, 0x10 };
/*
* there are two cases:
*   SIS A : offset: 4, { 0x12, 0x3A, 0x00, 0x10, 0x19, 0x04, 0x00, 0x10 }
*   SIS B : offset: 4, { 0x6D, 0x00, 0x00, 0x10, 0x19, 0x04, 0x00, 0x10 }
* so, simply, start from offset 6
*/
const uint8_t MAGIC_SIS[] = { 0x00, 0x10, 0x19, 0x04, 0x00, 0x10 };
const uint8_t MAGIC_TIFF_LE[] = { 0x49, 0x49, 0x2A, 0 };
const uint8_t MAGIC_TIFF_BE_A[] = { 0x4D, 0x4D, 0, 0x2A };
const uint8_t MAGIC_TIFF_BE_B[] = { 0x4D, 0x4D, 0, 0x2B };
const uint8_t MAGIC_WMA[] = { 0x30, 0x26, 0xB2, 0x75, 0x8E, 0x66, 0xCF, 0x11 };
const uint8_t MAGIC_ZOO[] = { 0xDC, 0xA7, 0xC4, 0xFD };
const uint8_t MAGIC_XAR[] = { 0x78, 0x61, 0x72, 0x21, 0x00, 0x1c }; // "xar!", 0x001c = sizeof(XARHeader), xar
const uint8_t MAGIC_Z[] = { 0x1F, 0x9D };
const uint8_t MAGIC_MSC_A[] = { 0x53, 0x5A, 0x44, 0x44, 0x88, 0xF0, 0x27, 0x33 }; // SZDD\x88\xF0'3"
const uint8_t MAGIC_MSC_B[] = { 0x4B, 0x57, 0x41, 0x4A, 0x88, 0xF0, 0x27, 0xD1 }; // "KWAJ\x88\xF0'\xD1"
const uint8_t MAGIC_CompoundUUID[] = { 0xD0, 0xCF, 0x11, 0xE0, 0xA1, 0xB1, 0x1A, 0xE1 }; // Compound UUID
// ?
const uint8_t MAGIC_BPI[] = { 0xF0, 0x0D, 0x00, 0x00 };
// ?
const uint8_t MAGIC_DCX[] = { 0xB1, 0x68, 0xDE, 0x3A };
// ?
const uint8_t MAGIC_NOD[] = { 0x90, 0x0D, 0x03, 0x00, 0x02, 0x11, 0xC8, 0xFC, 0xA0, 0x02, 0x06, 0x7B, 0x03, 0xC9, 0x01 };
// .wab文件主要与微软的Outlook Express有关, 用作通讯簿文件, 包含联系信息，如姓名，号码和电子邮件地址。
const uint8_t MAGIC_WAB[] = { 0x9C, 0xCB, 0xCB, 0x8D, 0x13, 0x75, 0xD2, 0x11, 0x91, 0x58, 0x00, 0xC0, 0x4F, 0x79, 0x56, 0xA4 };


typedef struct __st__ftype_magic {
    uint32_t       filetype{ 0 };
    uint8_t        type{ 0 }; /* 0, direct memory comparison of magicbytes; 1, magicbytes used as content-based content; */
    uint32_t       offset{ 0 };
    const uint8_t* magic{ 0 };
    uint32_t       size{ 0 };
} ftype_magic_t;

typedef int (*func_check_ftype)(const uint8_t*, uint32_t, ifile* file);

typedef struct __st_ftype_checker {
    uint8_t          chan;
    ftype_magic_t    magic;
    func_check_ftype checker{ nullptr };
} ftype_checker_t;

bool is_print(uint8_t c) {
    return c < 0x80 && isprint(c);
}

bool is_space(uint8_t c) {
    return c <= 0x80 && isspace(c);
}

uint32_t uint32_littlendian(const uint8_t* pbyBuf) {
    return (pbyBuf[1] << 8) + *pbyBuf + (((pbyBuf[3] << 8) + pbyBuf[2]) << 16);
}

uint16_t uint16_littlendian(const uint8_t* pbyBuf) {
    return *pbyBuf + (pbyBuf[1] << 8);
}

int is_pe(const uint8_t* data, uint32_t size, ifile* file) {
    if (data == nullptr || size < sizeof(IMAGE_DOS_HEADER)) {
        return ft_unknown;
    }

    const uint8_t DOS_MAGIC[] = { 0x4d, 0x5a }; // MZ
    const uint8_t NT_MAGIC[] = { 0x50, 0x45, 0x00, 0x00 };  // PE00
    const uint8_t OS2_NE_MAGIC[] = { 0x4E, 0x45 }; // NE
    const uint8_t OS2_LE_MAGIC[] = { 0x4C, 0x45 }; // LE
    const uint8_t OS2_LX_MAGIC[] = { 0x4C, 0x58 }; // LX

    if (*data != DOS_MAGIC[0] || *(data + 1) != DOS_MAGIC[1]) {
        return ft_unknown;
    }
    auto dos_header = (const IMAGE_DOS_HEADER*)data;
    auto e_lfanew = XsUtils::byteorder2host<uint32_t>(dos_header->e_lfanew, true);
    if (e_lfanew % 4) {
        return ft_unknown;
    }

    uint32_t avaiable_size_max = 0;
    uint32_t e_lfanew_size_min = 6;
    uint8_t e_lfanew_bytes[0x20] = { 0 };

    if (e_lfanew + e_lfanew_size_min <= size) {
        memcpy(e_lfanew_bytes, data + e_lfanew, e_lfanew_size_min);
        avaiable_size_max = size;
    }
    else if (file && e_lfanew + e_lfanew_size_min <= file->size()) {
        if (0 == file->read(e_lfanew, e_lfanew_bytes, e_lfanew_size_min, nullptr)) {
            avaiable_size_max = file->size();
        }
    }

    if (avaiable_size_max == 0) {
        return ft_unknown;
    }

    auto rst = ft_unknown;
    if (e_lfanew_bytes[0] == NT_MAGIC[0] && e_lfanew_bytes[1] == NT_MAGIC[1]) {
        if (e_lfanew + sizeof(IMAGE_NT_HEADERS32) > avaiable_size_max) {
            rst = ft_unknown;
        }
        auto machine = XsUtils::byteorder2host<uint16_t>(*(const uint16_t*)&e_lfanew_bytes[4], true);
        switch (machine) {
        case 0x014c: {
            rst = ft_pe_x86; break;
        }
        case 0x0200: {
            rst = ft_pe_ia64; break;
        }
        case 0x8664: {
            rst = ft_pe_x64; break;
        }
        case 0x01c0: {
            rst = ft_pe_arm; break;
        }
        case 0xAA64: {
            rst = ft_pe_arm64; break;
        }
        default: {
            rst = ft_pe; break;
        }
        }
    }
    else if (e_lfanew_bytes[0] == OS2_NE_MAGIC[0] && e_lfanew_bytes[1] == OS2_NE_MAGIC[1]) {
        if (e_lfanew + sizeof(IMAGE_OS2_HEADER) > avaiable_size_max) {
            rst = ft_unknown;
        }
        else {
            rst = ft_os2_ne;
        }
    }
    else if (e_lfanew_bytes[0] == OS2_LE_MAGIC[0] && e_lfanew_bytes[1] == OS2_LE_MAGIC[1]) {
        if (e_lfanew + sizeof(IMAGE_OS2_HEADER) > avaiable_size_max) {
            rst = ft_unknown;
        }
        else {
            rst = ft_os2_le;
        }
    }
    else if (e_lfanew_bytes[0] == OS2_LX_MAGIC[0] && e_lfanew_bytes[1] == OS2_LX_MAGIC[1]) {
        if (e_lfanew + sizeof(IMAGE_OS2_HEADER) > avaiable_size_max) {
            rst = ft_unknown;
        }
        else {
            rst = ft_os2_lx;
        }
    }

    return rst;
}

int is_elf(const uint8_t* data, uint32_t size, ifile* file = nullptr) {
    if (data == nullptr || size < sizeof(Elf32_Ehdr)) {
        return ft_unknown;
    }

    const Elf32_Ehdr* header = reinterpret_cast<const Elf32_Ehdr*>(data);
    if (header->e_ident[0] != 0x7f || memcmp(header->e_ident + 1, "ELF", 3) != 0) {
        return ft_unknown;
    }
    const Elf64_Ehdr* header64 = reinterpret_cast<const Elf64_Ehdr*>(data);

    auto rst = ft_unknown;
    bool is_little_endian = false, is_b64 = false;

    if (XsUtils::byteorder2host<uint16_t>(header->e_phentsize, true) == sizeof(Elf32_Phdr)) {
        is_little_endian = true, is_b64 = false;
    }
    else if (XsUtils::byteorder2host<uint16_t>(header->e_phentsize, false) == sizeof(Elf32_Phdr)) {
        is_little_endian = false, is_b64 = false;
    }
    else if (size < sizeof(Elf64_Ehdr)) {
        is_little_endian = false, is_b64 = false;
        return ft_unknown;
    }
    else if (XsUtils::byteorder2host<uint16_t>(header64->e_phentsize, true) == sizeof(Elf64_Phdr)) {
        is_little_endian = true, is_b64 = true;
    }
    else if (XsUtils::byteorder2host<uint16_t>(header64->e_phentsize, false) == sizeof(Elf64_Phdr)) {
        is_little_endian = false, is_b64 = true;
    }
    else {
        return ft_unknown;
    }

    uint64_t entry_point = 0;
    uint16_t header_size = 0, program_size = 0;
    if (!is_b64) {
        entry_point = XsUtils::byteorder2host<uint32_t>(header->e_entry, is_little_endian);
        header_size = XsUtils::byteorder2host<uint16_t>(header->e_ehsize, is_little_endian);
    }
    else {
        entry_point = XsUtils::byteorder2host<uint64_t>(header64->e_entry, is_little_endian);
        header_size = XsUtils::byteorder2host<uint16_t>(header64->e_ehsize, is_little_endian);
    }

    if ((is_b64 && header_size != sizeof(Elf64_Ehdr)) || (!is_b64 && header_size != sizeof(Elf32_Ehdr))) {
        return ft_unknown;
    }

    auto machine = XsUtils::byteorder2host<uint16_t>(header->e_machine, is_little_endian);
    switch (machine) {
    case EM_386: {
        rst = ft_elf_x86; break;
    }
    case EM_IA_64: {
        rst = ft_elf_ia64; break;
    }
    case EM_ARM: {
        rst = ft_elf_arm; break;
    }
    case EM_AARCH64: {
        rst = ft_elf_arm64; break;
    }
    case EM_X86_64: {
        rst = ft_elf_x64; break;
    }
    case EM_MIPS:
    case EM_MIPS_RS4_BE: {
        rst = is_b64 ? ft_elf_mips64 : ft_elf_mips; break;
    }
    default: {
        if (machine > EM_NONE && machine <= EM_XTENSA) {
            rst = rst = is_b64 ? ft_elf64 : ft_elf32;
        }
        else {
            rst = ft_unknown;
        }
    }
    }

    return rst;
}

int is_msi(const uint8_t* data, uint32_t size, ifile* file) {
    if (data == nullptr || size < sizeof(ole2_header_t)) {
        return ft_unknown;
    }

    const uint8_t magic_a[] = { 0xD0, 0xCF, 0x11, 0xE0, 0xA1, 0xB1, 0x1A, 0xE1 };
    const uint8_t magic_b[] = { 0x3E, 0x00, 0x03, 0x00, 0xFE, 0xFF, 0x09, 0x00 };
    if (memcmp(data, magic_a, sizeof(magic_a)) || memcmp(data + 24, magic_b, sizeof(magic_b))) {
        return ft_unknown;
    }

    uint32_t sector_size = (uint32_t)1 << ((const ole2_header_t*)data)->sector_shift;
    uint32_t root_entry_sid = ((const ole2_header_t*)data)->directory_entry_sid;
    uint32_t ole_size_min = sector_size * (root_entry_sid + 1) + sizeof(ole2_directory_entry_t);

    ole2_directory_entry_t temp_entry;
    const ole2_directory_entry_t* root_entry = nullptr;
    if (ole_size_min < size) {
        root_entry = (const ole2_directory_entry_t*)(data + sector_size * (root_entry_sid + 1));
    }
    else if (file && ole_size_min < file->size()) {
        auto offset = sector_size * (root_entry_sid + 1);
        if (0 == file->read(offset, (uint8_t*)&temp_entry, sizeof(temp_entry), nullptr)) {
            root_entry = &temp_entry;
        }
    }

    if (root_entry == nullptr || root_entry->obj_type != ole_dir_entry_root_storage) {
        return ft_unknown;
    }

    /*
    * CLSID_MsiTransform = { 0xC1082, 0x0, 0x0, {0xC0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x46} };
    * CLSID_MsiPackage = { 0xC1084, 0x0, 0x0, {0xC0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x46} };
    * CLSID_MsiPatch = { 0xC1086, 0x0, 0x0, {0xC0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x46} };
    * CLSID_MsiInstaller = { 0xC1090, 0x0, 0x0, {0xC0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x46} };
    */
    const uint8_t clsid[] = { 0x90, 0x10, 0x0C, 0, 0, 0, 0, 0, 0xC0, 0, 0, 0, 0, 0, 0, 0x46 };
    if (memcmp(&root_entry->clsid[1], &clsid[1], sizeof(clsid) - 1) == 0
        && root_entry->clsid[0] <= 0x90
        && root_entry->clsid[0] >= 0x82) {
        return ft_msi;
    }

    return ft_unknown;
}

int is_ole(const uint8_t* data, uint32_t size, ifile* file) {
    const uint8_t MAGIC_OLE[] = { 0xD0, 0xCF, 0x11, 0xE0, 0xA1, 0xB1, 0x1A, 0xE1 };
    if (data == nullptr || size < sizeof(MAGIC_OLE) || memcmp(data, MAGIC_OLE, sizeof(MAGIC_OLE))) {
        return ft_unknown;
    }
    auto emFileType = is_msi(data, size, file);
    return emFileType != ft_unknown ? emFileType : ft_ole;
}

int is_ace(const uint8_t* data, uint32_t size, ifile* file = nullptr) {
    if (data == nullptr || size < 14) {
        return ft_unknown;
    }

    uint16_t h_crc = XsUtils::byteorder2host<uint16_t>(*(const uint16_t*)(data), true);
    uint16_t h_size = XsUtils::byteorder2host<uint16_t>(*(const uint16_t*)(data + 2), true);
    uint8_t  h_type = *(data + 4);
    uint16_t h_flags = XsUtils::byteorder2host<uint16_t>(*(const uint16_t*)(data + 5), true);
    /*
    * reference : BASE_ARCBLK_AdjustEndiannessOfHeaderRest
    *   h_type 决定当前结构的具体类型，如：BASE_ACESTRUC_BLOCK_MAIN、BASE_ACESTRUC_BLOCK_FILE32...
    */
    if (!(h_type == BASE_ACESTRUC_BLOCK_MAIN
        || h_type == BASE_ACESTRUC_BLOCK_FILE32
        || h_type == BASE_ACESTRUC_BLOCK_RECOVERY32
        || h_type == BASE_ACESTRUC_BLOCK_FILE
        || h_type == BASE_ACESTRUC_BLOCK_RECOVERY
        || h_type == BASE_ACESTRUC_BLOCK_RECOVERY2)) {
        return ft_unknown;
    }

    if (ucs::nicmp((const char*)(data + 7), BASE_ACESTRUC_ACESIGN, 7)) {
        return ft_unknown;
    }

    uint16_t h_crc_val = 0;
    if (size >= (uint32_t)h_size + 4) {
        h_crc_val = crc16(BASE_CRC_MASK, data + 4, h_size);
    }
    else {
        /* can't check crc，we think it may be an ace file. */
        return ft_ace;
    }

    return (h_crc_val == h_crc) ? ft_ace : ft_unknown;
}

int is_arc(const uint8_t* data, uint32_t size, ifile* file = nullptr) {
    if (data == nullptr || size < 15 || *data != 0x1A || *(data + 1) == 0 || *(data + 1) > 9) {
        return ft_unknown;
    }
    int rst = ft_arc;
    for (auto cur = data; cur < data + 13; cur++) {
        uint8_t v = *(cur + 2);
        if (v == 0) break;
        if (0 == is_print(v)) rst = ft_unknown;
    }
    return rst;
}

int is_bmp(const uint8_t* data, uint32_t size, ifile* file = nullptr) {
    if (data == nullptr || size < 32) {
        return ft_unknown;
    }

    if (*data == 0x42 && *(data + 1) == 0x4D) {
        auto f = uint32_littlendian(data + 28);
        auto filesize = uint16_littlendian(data + 2);
        if (f == 1 || f == 4 || f == 8 || f == 24 || filesize == size) {
            return ft_bmp;
        }
    }
    return ft_unknown;
}

int is_dmg(const uint8_t* data, uint32_t size, ifile* file = nullptr) {
    const uint32_t KARC_DMG_HEADERSIZE = 0x01E0;
    const uint32_t KARC_DMG_XML_MAX = (((uint32_t)1 << 31) - (1 << 14));
    const uint8_t byXMLSign[] = { 0x3C, 0x3F, 0x78, 0x6D, 0x6C, 0x20 };  // "<?xml version="

    int64_t filesize = file ? file->size() : size;
    if (filesize < KARC_DMG_HEADERSIZE) {
        return ft_unknown;
    }

    uint8_t buffer[KARC_DMG_HEADERSIZE] = { 0 };
    int64_t nHeadPos = filesize - KARC_DMG_HEADERSIZE;
    if (data && size >= nHeadPos + KARC_DMG_HEADERSIZE) {
        memcpy(buffer, data + nHeadPos, KARC_DMG_HEADERSIZE);
    }
    else if (file) {
        uint32_t readed = 0;
        if (0 != file->read(nHeadPos, buffer, KARC_DMG_HEADERSIZE, &readed) || readed != KARC_DMG_HEADERSIZE) {
            return ft_unknown;
        }
    }
    else {
        return ft_unknown;
    }

    if (memcmp(&buffer[0x00], &buffer[0xB8], 8) != 0) {
        return ft_unknown;
    }

    int64_t nAddr = XsUtils::byteorder2host<int64_t>(*(int64_t*)buffer, false);
    int64_t nSize = XsUtils::byteorder2host<int64_t>(*(int64_t*)(buffer + 0xC0), false);
    if (!((nSize > 0) && (nSize < KARC_DMG_XML_MAX) && (nAddr > 0) && (nAddr + nSize < nHeadPos))) {
        return ft_unknown;
    }

    memset(buffer, 0, sizeof(buffer));

    if (data && size >= nAddr + sizeof(byXMLSign)) {
        memcpy(buffer, data + nAddr, sizeof(byXMLSign));
    }
    else if (file) {
        uint32_t readed = 0;
        if (0 != file->read(nAddr, buffer, sizeof(byXMLSign), &readed) || readed != sizeof(byXMLSign)) {
            return ft_unknown;
        }
    }
    else {
        return ft_unknown;
    }

    if (memcmp(buffer, byXMLSign, sizeof(byXMLSign)) == 0) {
        return ft_dmg;
    }
    return ft_unknown;
}

int is_bzip2(const uint8_t* data, uint32_t size, ifile* file = nullptr) {
    const uint8_t MAGIC_BZIP2_P1[] = { 0x42u, 0x5Au };
    const uint8_t MAGIC_BZIP2_P2[] = { 0x31u, 0x41, 0x59, 0x26, 0x53, 0x59 };

    bool is = (size > 8 && 0 == memcmp(data, MAGIC_BZIP2_P1, sizeof(MAGIC_BZIP2_P1)) && 0 == memcmp(data + 4, MAGIC_BZIP2_P2, sizeof(MAGIC_BZIP2_P2)));
    if (is && ft_dmg == is_dmg(data, size, file)) { // DMG可能是用BZip2压缩的
        return ft_dmg;
    }
    return is ? ft_bzip2 : ft_unknown;
}

int is_chm(const uint8_t* data, uint32_t size, ifile* file = nullptr) {
    // "ITSF\x03\x00\x00\x00"
    const uint8_t CHM_V3[] = { 0x49, 0x54, 0x53, 0x46, 0x03, 0x00, 0x00, 0x00 };
    // {7C01FD10-7BAA-11D0-9E0C-00A0-C922-E6EC}, {7C01FD11-7BAA-11D0-9E0C-00A0-C922-E6EC}
    const uint8_t CHM_UUID[] = {
        0x10, 0xFD, 0x01, 0x7C, 0xAA, 0x7B, 0xD0, 0x11, 0x9E, 0x0C, 0x00, 0xA0, 0xC9, 0x22, 0xE6, 0xEC,
        0x11, 0xFD, 0x01, 0x7C, 0xAA, 0x7B, 0xD0, 0x11, 0x9E, 0x0C, 0x00, 0xA0, 0xC9, 0x22, 0xE6, 0xEC
    };
    bool is = (size >= 0x60 && 0 == memcmp(data, CHM_V3, sizeof(CHM_V3)) && 0 == memcmp(data + 0x18, CHM_UUID, sizeof(CHM_UUID)));
    return is ? ft_chm : ft_unknown;
}

int is_dbx(const uint8_t* data, uint32_t size, ifile* file = nullptr) {
    /*
    * OUTLOOK_EXPRESS_DBX
    *
    * CLSID_SyncOpDatabase : 0xCF, 0xAD, 0x12, 0xFE, {26FE9D30-1A8F-11d2-AABF-006097D474C4}
    * CLSID_xx_Database    : 0xCF, 0xAD, 0x12, 0xFE, {6F74FD??-E366-11D1-9A4E-00C04FA309D4}
    *   MessageDatabase : 0xCF, 0xAD, 0x12, 0xFE, {6F74FDC5-E366-11D1-9A4E-00C04FA309D4}
    *   FolderDatabase : 0xCF, 0xAD, 0x12, 0xFE, {6F74FDC6-E366-11D1-9A4E-00C04FA309D4}
    *   Pop3uidl.dbx : 0xCF, 0xAD, 0x12, 0xFE, {6F74FDC7-E366-11D1-9A4E-00C04FA309D4}
    */
    const uint8_t magic_dbx_p0[] = { 0xCF, 0xAD, 0x12, 0xFE };
    const uint8_t magic_dbx_p4[] = { 0xC5, 0xFD, 0x74, 0x6F, 0x66, 0xE3, 0xD1, 0x11, 0x9A, 0x4E, 0x00, 0xC0, 0x4F, 0xA3, 0x09, 0xD4 };
    const uint8_t magic_dbx_op[] = { 0x30, 0x9D, 0xFE, 0x26, 0x8F, 0x1A, 0xD2, 0x11, 0xAA, 0xBF, 0x00, 0x60, 0x97, 0xD4, 0x74, 0xC4 };
    if (nullptr == data || size < sizeof(magic_dbx_p0) + sizeof(magic_dbx_p4)) {
        return ft_unknown;
    }

    if (0 != memcmp(data, magic_dbx_p0, sizeof(magic_dbx_p0))) {
        return ft_unknown;
    }

    if (0 == memcmp(data + 4, magic_dbx_op, sizeof(magic_dbx_op))) {
        return ft_oe_dbx;
    }
    else if ((data[4] == 0xC5 || data[4] == 0xC6 || data[4] == 0xC7)
        && 0 == memcmp(data + 5, &magic_dbx_p4[1], sizeof(magic_dbx_p4) - 1)) {
        return ft_oe_dbx;
    }
    return ft_unknown;
}

int is_cpio(const uint8_t* data, uint32_t size, ifile* file = nullptr) {
    if (data && size >= 2) {
        // 0x71 0xC7, CPIO Old Binary, Big-Endian
        // 0xC7 0x71, CPIO Old Binary, Little-Endian
        if ((data[0] == 0x71 && data[1] == 0xC7) || (data[0] == 0xC7 && data[1] == 0x71)) {
            return ft_cpio;
        }
    }

    const uint8_t sign_cpio[] = { 0x30, 0x37, 0x30, 0x37, 0x30 };  // "07070x", x = 1, 2, 7
    const uint32_t sign_size = sizeof(sign_cpio);

    if (data && size >= (sign_size + 1) && 0 == memcmp(data, sign_cpio, sign_size)) {
        unsigned char c = data[sign_size];
        if (c == '1' || c == '2' || c == '7') {
            // 070701, New ASCII
            // 070702, New CRC
            // 070707, Portable ASCII 
            return ft_cpio;
        }
    }

    return ft_unknown;
}

int is_ico(const uint8_t* data, uint32_t size, ifile* file = nullptr) {
    if (data == nullptr || size < 10) {
        return ft_unknown;
    }

    bool b6true = false, b7true = false;
    if (uint32_littlendian(data) == 0x00010000 && uint16_littlendian(data + 4) && 0 == *(data + 9)) {
        uint8_t b6 = *(data + 6);
        uint8_t b7 = *(data + 7);
        b6true = (b6 == 16 || b6 == 32 || b6 == 48 || b6 == 64);
        b7true = (b7 == 16 || b7 == 32 || b7 == 48 || b7 == 64);
    }
    return (b6true && b7true) ? ft_ico : ft_unknown;
}

int is_cur(const uint8_t* data, uint32_t size, ifile* file = nullptr) {
    if (data == nullptr || size < 10) {
        return ft_unknown;
    }

    bool b6true = false, b7true = false;
    if (uint32_littlendian(data) == 0x00020000 && uint16_littlendian(data + 4) && 0 == *(data + 9)) {
        uint8_t b6 = *(data + 6);
        uint8_t b7 = *(data + 7);
        b6true = (b6 == 16 || b6 == 32 || b6 == 48 || b6 == 64);
        b7true = (b7 == 16 || b7 == 32 || b7 == 48 || b7 == 64);
    }
    return (b6true && b7true) ? ft_cur : ft_unknown;
}

int is_emf(const uint8_t* data, uint32_t size, ifile* file = nullptr) {
    const uint8_t MAGIC_P1[] = { 0x01, 0x00, 0x00, 0x00 };
    const uint8_t MAGIC_P2[] = { 0x20, 0x45, 0x4D, 0x46 };
    bool is = (size > 44 && 0 == memcmp(data, MAGIC_P1, sizeof(MAGIC_P1)) && 0 == memcmp(data + 40, MAGIC_P2, sizeof(MAGIC_P2)));
    return is ? ft_chm : ft_unknown;
}

const uint8_t* skip_chars(const uint8_t* data, uint32_t size, uint32_t char_flag) {
    if (data == nullptr || size == 0) {
        return nullptr;
    }
    for (int32_t offset = 0; offset < size; offset++, data++) {
        if (!(g_CharTypeTable[*data] & char_flag)) break;
    }
    return data;
};

int is_email(const uint8_t* data, uint32_t size, ifile* file = nullptr) {
    if (data == nullptr || size < 0x20) {
        return ft_unknown;
    }

    auto data_raw = data;
    data = skip_chars(data, size, _IS_SPACE_EOL_FLAG);
    if (data == nullptr || data + 2 >= data_raw + size) {
        return ft_unknown;
    }

    // 2. 快速分流
    uint16_t word = XsUtils::byteorder2host<uint16_t>(*(uint16_t*)data, false);
    switch (word | 0x2020)
    {
    case 0x7265: { // re: Received, Return-Path
        if (0 == ucs::nicmp((const char*)data, "RETURN-PATH", 11)) {
            data += 11;
        }
        else if (0 == ucs::nicmp((const char*)data, "RECEIVED", 8)) {
            data += 8;
        }
        else {
            goto Exit0;
        }
        break;
    }
    case 0x6D69: {  // mi: MIME-Version
        if (0 == ucs::nicmp((const char*)data, "MIME-VERSION", 12)) {
            data += 12;
        }
        else {
            goto Exit0;
        }
        break;
    }
    case 0x6D65: {
        if (0 == ucs::nicmp((const char*)data, "MESSAGE-ID", 10)) {
            data += 10;
        }
        else {
            goto Exit0;
        }
        break;
    }
    case 0x7375: {
        if (0 == ucs::nicmp((const char*)data, "SUBJECT", 7)) {
            data += 7;
        }
        else {
            goto Exit0;
        }
        break;
    }
    case 0x636F: {
        if (0 == ucs::nicmp((const char*)data, "Content-Type", 12)) {
            data += 12;
        }
        else {
            goto Exit0;
        }
        break;
    }
    case 0x6672: {
        if (0 == ucs::nicmp((const char*)data, "From", 4)) {
            data += 4;
        }
        else {
            goto Exit0;
        }
        break;
    }
    case 0x6461: {
        if (0 == ucs::nicmp((const char*)data, "Date", 4)) {
            data += 4;
        }
        else {
            goto Exit0;
        }
        break;
    }
    case 0x5448: {
        if (0 == ucs::nicmp((const char*)data, "THREAD-", 7)) {
            data += 7;
        }
        else {
            goto Exit0;
        }
        break;
    }
    case 0x746F: { // to: To
        data += 2;
        break;
    }
    case 0x782D: { // x-: X-Priority, X-MSMail-Priority, X-MimeOLE, etc
        data = skip_chars(data + 2, data + 2 - data_raw, _IS_EMAIL_TOKEN_FLAG);
        break;
    }
    default:
        goto Exit0;
    }

    if (data == nullptr || data >= data_raw + size) {
        goto Exit0;
    }

    data = skip_chars(data, data - data_raw, _IS_EMAIL_TOKEN_FLAG);
    if (data == nullptr || data >= data_raw + size) {
        goto Exit0;
    }

    return (*data == ':') ? ft_email : ft_unknown;
Exit0:
    return ft_unknown;
}

int is_flac(const uint8_t* data, uint32_t size, ifile* file = nullptr) {
    if (data == nullptr || size < 8) {
        return ft_unknown;
    }

    /* flac, http://flac.sourceforge.net/format.html */

    const uint8_t magic_flac_p0[] = { 0x66, 0x4C, 0x61, 0x43 }; // "fLaC", flac
    const uint8_t magic_flac_p4[] = { 0x00, 0x00, 0x00, 0x22 }; // Last = 0, BLOCK_TYPE = STREAMINFO, Length = sizeof(METADATA_BLOCK_STREAMINFO)
    if (0 == memcmp(data, magic_flac_p0, sizeof(magic_flac_p0))
        && 0 == memcmp(data + 4, magic_flac_p4, sizeof(magic_flac_p4))) {
        return ft_flac;
    }
    return ft_unknown;
}

int is_gzip(const uint8_t* data, uint32_t size, ifile* file = nullptr) {
    /* magic: 0x1F 0x8B 0x08 */
    bool is = (size >= 10 && *(data + 1) == 0x8B && *(data + 2) == 0x8 && *(data + 9) <= 0x10);
    return is ? ft_gzip : ft_unknown;
}

#if defined(_WIN64) || defined(__LP64__)   // 64位平台
#define ARC_HFS_INTSIZE  uint64_t
#else   // 32位平台
#define ARC_HFS_INTSIZE  uint32_t
#endif

int is_hfs(const uint8_t* data, uint32_t size, ifile* file = nullptr) {
    ARC_HFS_INTSIZE* pInt = (ARC_HFS_INTSIZE*)data;

    // 496 = sizeof(VolHeader)
    if (size < 1024 + 496) {
        return ft_unknown;
    }

    // 前1024个字节为0, 用KARC_HFS_INTSIZE(本机字长)可加快速度
    for (int i = 0; i < 1024 / sizeof(ARC_HFS_INTSIZE); i++) {
        if (!(pInt[i] == 0)) {
            return ft_unknown;
        }
    }

    // HFS+: H+\00\04, HFSX: HX\00\05
    uint32_t uMagic = XsUtils::byteorder2host<uint32_t>(*(uint32_t*)(data + 1024), false);
    if ((uMagic == 0x482B0004) || (uMagic == 0x48580005)) {
        return ft_hfs;
    }
    return ft_unknown;
}

#undef ARC_HFS_INTSIZE

int pare_xxml_brackets(const uint8_t* data, uint32_t size, std::string& tag, uint32_t& pos, uint32_t& len, bool& closure) {
    if (data == nullptr || size == 0) {
        return -1;
    }

    auto spare_find = [](const uint8_t* data, uint32_t size, const char* m) {
        if (data == nullptr || size == 0 || m == nullptr) {
            return false;
        }
        uint32_t pos = 0, mpos = 0, mlen = ucs::len(m);
        for (; pos < size && is_space(*(data + pos)); ++pos);
        for (; mpos < mlen && pos < size; ++pos) {
            if (*(m + mpos) == *(data + pos)) {
                mpos++;
                continue;
            }
            else if (is_space(*(data + pos))) {
                continue;
            }
            else {
                break;
            }
        }
        return (mpos == mlen);
    };

    auto is_tagname_end = [](uint8_t c) {
        return (is_space(c) || c == '>');
    };

    const char* nextag = nullptr;
    std::string curtag = "";
    uint32_t curpos = 0, curlen = 0;

    if (!tag.empty() && tag[0] == '<') tag = tag.substr(1);

    if (0 == ucs::nicmp((const char*)data, "<script", 6)) {
        curpos = 0;
    }

    for (uint32_t i = 0, start = 0, length = 0; i < size; ++i) {
        if (*(data + i) != '<') continue;
        for (i += 1; i < size && is_space(*(data + i)); ++i);
        if (i >= size) break;

        closure = false;

        if (tag.size() > 1) {
            if (spare_find(data + i, size - i, tag.c_str())) {
                curtag = tag;
                nextag = ">";
                curpos = i;
            }
            else {
                nextag = nullptr;
                continue;
            }
        }
        else if (*(data + i) == '!') {
            curpos = i;
            if (spare_find(data + i, size - i, "!--")) {
                curtag = "!--";
                nextag = "-->";
                closure = true;
            }
            else if (spare_find(data + i, size - i, "![CDATA[")) {
                curtag = "![CDATA[";
                nextag = "]]>";
                closure = true;
            }
            else if (spare_find(data + i, size - i, "!DOCTYPE")) {
                curtag = "!DOCTYPE";
                nextag = ">";
                closure = true;
            }
            else {
                curtag += *(data + i);
                for (; i < size && is_space(*(data + i)); i += 1);
                for (; i < size && !is_tagname_end(*(data + i)); i += 1) curtag += *(data + i);
                nextag = ">";
            }
        }
        else if (*(data + i) == '?') {
            curpos = i;
            curtag += *(data + i);
            for (; i < size && is_space(*(data + i)); i += 1);
            for (; i < size && !is_tagname_end(*(data + i)); i += 1) curtag += *(data + i);
            nextag = ">";
        }
        else {
            curpos = i;
            for (; i < size && !is_tagname_end(*(data + i)); i += 1) curtag += *(data + i);
            nextag = ">";
        }

        if (nextag == nullptr) {
            break;
        }

        uint32_t tag_len = strlen(nextag);
        for (; i + tag_len <= size && ucs::nicmp(nextag, (const char*)data + i, tag_len); ++i);
        if (i + tag_len <= size) {
            curlen = i + tag_len - curpos;
            nextag = 0;
        }

        for (auto r = curlen + curpos - 1; !closure && curlen && r < size && r > curpos; --r) {
            if (is_space(*(data + r))) { continue; }
            else if (*(data + r) == '/') { closure = true; }
            break;
        }

        break;
    }

    pos = curpos;
    len = curlen;
    tag = curtag;
    return 0;
}

int is_xml(const uint8_t* data, uint32_t size, ifile* file = nullptr) {
    if (data == nullptr || size < 4) {
        return ft_unknown;
    }

    uint32_t pos = 0;
    for (; pos < size && !is_print(*(data + pos)); ++pos);
    if (pos >= size || *(data + pos) != '<') {
        return ft_unknown;
    }
    for (pos += 1; pos < size && is_space(*(data + pos)); ++pos);
    if (!((pos + 4 < size) && 0 == ucs::nicmp("?xml", (const char*)data + pos, 4))) {
        return ft_unknown;
    }

    return ft_xml;
}

int is_html(const uint8_t* data, uint32_t size, ifile* file = nullptr) {
    if (data == nullptr || size < 4) {
        return ft_unknown;
    }

    /*
    * <?xml version="1.0" encoding="utf-8" ?>
    *       encoding 为可选，xml 的第一行一定为此
    * <!DOCTYPE note SYSTEM "book.dtd">
    *       文档定义类型（DTD：Document Type Definition），DTD是可选的
    * <![CDATA[...]]>
    *       CDATA 指的是不应由 XML 解析器进行解析的文本数据。
    *       CDATA 内容部分不能包含字符串 "]]>"。也不允许嵌套的 CDATA 部分。标记 CDATA 结尾的 "]]>" 不能包含空格或折行。
    *
    * <!DOCTYPE html>
    *       文档定义类型（DTD：Document Type Definition），DTD是可选的
    * <html>
    *       html 的开始，可以附加其它属性
    * <script type="text/javascript"> ... </script>
    *       JavaScript 脚本部分，可以操作 html 内容(所以其中可能有 html 元素)
    *
    * <tag /> 可以表示一个空元素
    *
    * <!-- ... --> 注释
    */

    uint32_t pos = 0;
    for (; pos < size && !is_print(*(data + pos)); ++pos);
    if (pos >= size || *(data + pos) != '<') {
        return ft_unknown;
    }
    for (pos += 1; pos < size && is_space(*(data + pos)); ++pos);
    if (!((pos + 4 < size) && 0 == ucs::nicmp("html", (const char*)data + pos, 4))
        && !((pos + 8 < size) && 0 == ucs::nicmp("!DOCTYPE", (const char*)data + pos, 8))) {
        return ft_unknown;
    }

    struct xxml_part {
        std::string tag;
        uint32_t pos;
        uint32_t len;
        bool     closure;
    };
    std::vector<xxml_part> parts;

    const char* nextag = nullptr;
    for (uint32_t i = 0, start = 0, length = 0; i < size; ) {
        xxml_part item;
        pare_xxml_brackets(data + i, size - i, item.tag, item.pos, item.len, item.closure);
        if (0 == item.len) {
            break;
        }
        item.pos += i;
        i = item.pos + item.len;

        if (item.closure) {
            parts.push_back(item);
            continue;
        }

        if (0 != ucs::icmp(item.tag.c_str(), "Script")) {
            parts.push_back(item);
            continue;
        }

        xxml_part pair;
        pair.tag = "/" + item.tag;
        pare_xxml_brackets(data + i, size - i, pair.tag, pair.pos, pair.len, pair.closure);
        if (0 == item.len) {
            break;
        }
        assert(item.closure == false);
        i += pair.pos + pair.len;

        item.len = i - item.pos;
        item.closure = true;
        parts.push_back(item);
    }

    auto cover_size = 0;
    for (auto& iter : parts) cover_size += iter.len;

    auto total_size = 0;
    for (uint32_t i = 0, last = 0, cur = 0; i < parts.size(); i++) {
        if (parts[i].closure) {
            total_size += parts[i].len;
            continue;
        }
        for (uint32_t j = parts.size() - 1; j > i; j--) {
            if (parts[i].tag == parts[j].tag.substr(1)) {
                total_size += parts[j].len + parts[j].pos - parts[i].pos;
                i = j;
                break;
            }
        }
    }
    bool is = (100 * total_size / size > 70) && (cover_size * 100 / size > 40);
    return is ? ft_html : ft_unknown;
}

int is_ini(const uint8_t* data, uint32_t size, ifile* file = nullptr) {
    if (data == nullptr) {
        return ft_unknown;
    }

    uint32_t pos = 0;
    for (;; pos++) {
        for (; pos < size && is_space(*(data + pos)); ++pos) {}

        if (pos >= size || (*(data + pos) != '[' && *(data + pos) != ';' && *(data + pos) != '#')) {
            pos = size;
            break;
        }

        if (*(data + pos) == ';' || *(data + pos) == '#') {
            for (; pos < size && *(data + pos) != 0xA; ++pos) {}
            continue;
        }

        break;
    }

    for (; pos < size && *(data + pos) != ']' && *(data + pos) != 0xA; ++pos) {}

    if (pos >= size || *(data + pos) == 0xA) {
        return ft_unknown;
    }
    pos++;

    int rst = ft_unknown;
    for (uint8_t v = 0; pos < size; ++pos) {
        v = *(data + pos);
        if (v == '\0' || v == ' ') continue;
        if (v == '#' || v == ';' || v == 0x0A || v == 0x0D) {
            rst = ft_ini;
        }
        break;
    }

    return rst;
}

int CheckUDFAVDP(const uint8_t* data, uint32_t nSize) {
    if (nSize < 0x10) {
        return ft_unknown;
    }

    // Tag Identifier 0x0002: Anchor Volume Descriptor Pointer (ECMA 167r3 3/7.2.1)
    if (0x0002 != XsUtils::byteorder2host<uint16_t>(*(uint16_t*)data, true)) {
        return ft_unknown;
    }

    // Calc Checksum, skip tagChecksum & reserved
    uint8_t nSum = 0;
    for (int i = 0; i < 4; i++) nSum += data[i];
    for (int i = 6; i < 16; i++) nSum += data[i];
    if (!((nSum == data[4]) && (data[5] == 0))) {
        return ft_unknown;
    }

    return ft_udf;
}

int is_udf(const uint8_t* data, uint32_t size, ifile* file = nullptr) {
    int64_t filesize = file ? file->size() : size;
    /*  according to spec, anchor is in either:
     *     block 256
     *     lastblock-256
     *     lastblock
     *  however, if the disc isn't closed, it could be 512 */
    const int32_t nAnchorOff[] = { 2048, 256 };  // 7Zip only support 1<<11 & 1<<8
    const int32_t nBufSize = 0x10;

    int ftype = ft_unknown;
    for (int i = 0; i < 2; i++) {
        if (filesize < nAnchorOff[i]) {
            continue;
        }
        int64_t nOff = filesize - nAnchorOff[i];
        uint8_t Buf[nBufSize] = { 0 };

        if (data && size >= nOff + nBufSize) {
            memcpy(Buf, data + nOff, nBufSize);
        }
        else if (file) {
            uint32_t readed = 0;
            if (0 != file->read(nOff, Buf, nBufSize, &readed) || readed != nBufSize) {
                break;
            }
        }
        else {
            break;
        }

        if (ft_udf != CheckUDFAVDP(Buf, 0x10)) {
            continue;
        }

        ftype = ft_udf;
        break;
    }

    return ftype;
}

int is_iso(const uint8_t* data, uint32_t size, ifile* file = nullptr) {
    const uint8_t byISOSign[] = { 0x43, 0x44, 0x30, 0x30, 0x31 };  // "CD001"
    const uint8_t byNSR2Sign[] = { 0x4E, 0x53, 0x52, 0x30, 0x32 };  // "NSR02", UDF
    const uint8_t byNSR3Sign[] = { 0x4E, 0x53, 0x52, 0x30, 0x33 };  // "NSR03", UDF
    const uint32_t sign_offset = 0x8000;
    const uint32_t buffer_size = 0x08;
    uint8_t buffer[buffer_size] = { 0 };

    if (size >= sign_offset + buffer_size) {
        memcpy(buffer, data + sign_offset, buffer_size);
    }
    else if (file != nullptr) {
        uint32_t readed = 0;
        if (0 != file->read(sign_offset, buffer, buffer_size, &readed) || readed != buffer_size) {
            return ft_unknown;
        }
    }

    if (!(memcmp(&buffer[1], byISOSign, sizeof(byISOSign)) == 0 ||
        memcmp(&buffer[1], byNSR2Sign, sizeof(byNSR2Sign)) == 0 ||
        memcmp(&buffer[1], byNSR3Sign, sizeof(byNSR3Sign)) == 0)) {
        return ft_unknown;
    }

    auto ftype_new = is_udf(data, size, file);
    return (ftype_new == ft_unknown) ? ft_iso : ftype_new;
}

int is_inno(const uint8_t* data, uint32_t size, ifile* file = nullptr) {
#if 0
    // enumKFT_Inno, 本文件只是纯压缩数据, 算法及文件起始位置不明, 无法解压
    // 0x69 0x64 0x73 0x6B 0x61 0x33 0x32 0x1A, "idska32\x1A", Inno Disk Slice ID
#endif
    // "rDlPtS\CD\E6\D7\7B\0B\2A", Inno 5.1.5 ~ 5.4.0
    const uint8_t Inno51xxSign[] = { 0x72, 0x44, 0x6C, 0x50, 0x74, 0x53, 0xCD, 0xE6, 0xD7, 0x7B, 0x0B, 0x2A };

    if (size >= sizeof(Inno51xxSign) && 0 == memcmp(data, Inno51xxSign, sizeof(Inno51xxSign))) {
        // check 5.1.5 and newer first
        return ft_inno;
    }
    else if (size > 12) {
        auto a = XsUtils::byteorder2host<uint32_t>(*(uint32_t*)(data + 2), false);
        auto b = XsUtils::byteorder2host<uint32_t>(*(uint32_t*)(data + 8), false);
        auto c = XsUtils::byteorder2host<uint16_t>(*(uint16_t*)(data + 6), false);

        if (!(a == 0x6C507453 && b == 0x87655678)) {
            return ft_unknown;
        }

        // check 5.1.2 and older
        switch (c)
        {
        case 0x3032:  // "rDlPtS02\x87\x65\x56\x78", 2.0.8-3.0.8
        case 0x3034:  // "rDlPtS04\x87\x65\x56\x78", 4.0.0-4.0.2
        case 0x3035:  // "rDlPtS05\x87\x65\x56\x78", 4.0.3-4.0.9
        case 0x3036:  // "rDlPtS06\x87\x65\x56\x78", 4.0.10-4.1.5
        case 0x3037:  // "rDlPtS07\x87\x65\x56\x78", 4.1.6-5.1.2
            return ft_inno;
        default:
            break;
        }
    }

    return ft_unknown;
}

int is_jpg(const uint8_t* data, uint32_t size, ifile* file = nullptr) {
    if (data == nullptr || size < 11) {
        return ft_unknown;
    }

    const uint8_t magic_b[] = { 0x4A, 0x46, 0x49, 0x46 };
    const uint8_t magic_a[] = { 0xFF, 0xD8, 0xFF };
    bool is = (*(data + 10) && 0 == memcmp(data, magic_a, sizeof(magic_a)) && 0 == memcmp(data + 6, magic_b, sizeof(magic_b)));
    return is ? ft_jpg : ft_unknown;
}

int is_lzh(const uint8_t* data, uint32_t size, ifile* file = nullptr) {
    // "-lh?-", lzh
    bool is = (data && size > 6 && *(data + 2) == 45 && *(data + 3) == 108 && *(data + 6) == 45);
    return is ? ft_lzh : ft_unknown;
}

int is_mp3(const uint8_t* data, uint32_t size, ifile* file = nullptr) {
    if (data == nullptr || size < 3) {
        return ft_unknown;
    }
    if (*(data) == 0xFF && (*(data + 1) == 0xFB || *(data + 1) == 0xFA)) {
        return ft_mp3;
    }
    if (*(data) == 0x49 && *(data + 1) == 0x44 && *(data + 2) == 0x33) {
        return ft_mp3;
    }
    return ft_unknown;
}

int is_ms_advanced_stream(const uint8_t* data, uint32_t size, ifile* file = nullptr) {
    /*
    * Microsoft Advanced Streaming Format
    *   reference : https://wiki.multimedia.cx/index.php/Microsoft_Advanced_Streaming_Format
    *   MIME Types:
    *       asf: video/x-ms-asf or application/vnd.ms-asf
    *       wmv: video/x-ms-wmv
    *       wma: video/x-ms-wma
    */

    // ASF_Header_Object, {75B22630-668E-11CF-A6D9-00AA0062CE6C}
    const uint8_t asf_uuid[] = {
        0x30, 0x26, 0xB2, 0x75, 0x8E, 0x66, 0xCF, 0x11, 0xA6, 0xD9, 0x00, 0xAA, 0x00, 0x62, 0xCE, 0x6C
    };

    if (data && size >= sizeof(asf_uuid) && 0 == memcmp(data, asf_uuid, sizeof(asf_uuid))) {
        return ft_ms_asf;
    }
    return ft_unknown;
}

// https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html
// Multibyte data items are always stored in big-endian order, where the high bytes come first.
int is_java_cls(const uint8_t* data, uint32_t size, ifile* file = nullptr) {
    // The current offset set as the position after the magic, minor & major version, constant pool count
    uint32_t current_offset = sizeof(MAGIC_JAVA_CLS) + 3 * sizeof(uint16_t);
    if (current_offset >= size) {
        return ft_unknown;
    }

    if (memcmp(data, MAGIC_JAVA_CLS, sizeof(MAGIC_JAVA_CLS)) != 0) {
        return ft_unknown;
    }

    bool is_little_endian = false;
    uint16_t constant_pool_count = XsUtils::byteorder2host<uint16_t>(*(uint16_t*)(data + 8), is_little_endian);
    if (constant_pool_count <= 0) {
        return ft_unknown;
    }

    auto rst = ft_java_cls;
    // https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html
    // The constant_pool table is indexed from 1 to constant_pool_count-1.
    bool should_break_loop = false;
    for (int i = 1; i < constant_pool_count - 1; i++) {
        if (current_offset >= size) {
            rst = ft_unknown;
            break;
        }

        uint8_t constant_tag = XsUtils::byteorder2host<uint8_t>(*(uint8_t*)(data + current_offset), is_little_endian);
        switch (constant_tag)
        {
        case CONSTANT_UTF8_TAG: {
            auto constant_utf8 = (const CONSTANT_UTF8*)(data + current_offset);
            if (current_offset + sizeof(uint8_t) + sizeof(uint16_t) >= size) {
                rst = ft_unknown;
                should_break_loop = true;
                break;
            }
            uint16_t content_length = XsUtils::byteorder2host<uint16_t>(constant_utf8->length, is_little_endian);
            current_offset += sizeof(uint8_t) + sizeof(uint16_t) + content_length;
            break;
        }
        case CONSTANT_INTEGER_TAG: {
            current_offset += sizeof(CONSTANT_INTEGER);
            break;
        }
        case CONSTANT_FLOAT_TAG: {
            current_offset += sizeof(CONSTANT_FLOAT);
            break;
        }
        case CONSTANT_LONG_TAG: {
            current_offset += sizeof(CONSTANT_LONG);
            break;
        }
        case CONSTANT_DOUBLE_TAG: {
            current_offset += sizeof(CONSTANT_DOUBLE);
            break;
        }
        case CONSTANT_CLASS_TAG: {
            current_offset += sizeof(CONSTANT_CLASS);
            break;
        }
        case CONSTANT_STRING_TAG: {
            current_offset += sizeof(CONSTANT_STRING);
            break;
        }
        case CONSTANT_FIELD_REF_TAG: {
            current_offset += sizeof(CONSTANT_FIELD_REF);
            break;
        }
        case CONSTANT_METHOD_REF_TAG: {
            current_offset += sizeof(CONSTANT_METHOD_REF);
            break;
        }
        case CONSTANT_INTERFACE_METHOD_REF_TAG: {
            current_offset += sizeof(CONSTANT_INTERFACE_METHOD_REF);
            break;
        }
        case CONSTANT_NAME_AND_TYPE_TAG: {
            current_offset += sizeof(CONSTANT_NAME_AND_TYPE);
            break;
        }
        case CONSTANT_METHOD_HANDLE_TAG: {
            current_offset += sizeof(CONSTANT_METHOD_HANDLE);
            break;
        }
        case CONSTANT_METHOD_TYPE_TAG: {
            current_offset += sizeof(CONSTANT_METHOD_TYPE);
            break;
        }
        case CONSTANT_INVOKE_DYNAMIC_TAG: {
            current_offset += sizeof(CONSTANT_INVOKE_DYNAMIC);
            break;
        }
        default: {
            rst = ft_unknown;
            should_break_loop = true;
            break;
        }
        }

        if (should_break_loop) {
            break;
        }
    }

    if (rst == ft_unknown) {
        return ft_unknown;
    }

    // check again, should at least have access_flags, this_class in the following content
    if (current_offset >= size) {
        rst = ft_unknown;
    }

    return rst;
}

int is_macho(const uint8_t* data, uint32_t size, ifile* file = nullptr) {
    auto rst = ft_unknown;
    if (data == nullptr || size < sizeof(MACH_HEADER)) {
        return rst;
    }

    bool is_little_endian = false, is_binary64 = false;
    uint32_t h_magic = XsUtils::byteorder2host<uint32_t>(*(const uint32_t*)(data), true);
    switch (h_magic) {
    case MH_MAGIC:
        is_little_endian = true, is_binary64 = false;
        break;
    case MH_MAGIC_64:
        is_little_endian = true, is_binary64 = true;
        break;
    case MH_CIGAM:
        is_little_endian = false, is_binary64 = false;
        break;
    case MH_CIGAM_64:
        is_little_endian = false, is_binary64 = true;
        break;
    default:
        return rst;
    }

    // 32-bit non-fat mach-o
    if (!is_binary64) {
        auto mach_header = (const MACH_HEADER*)data;
        uint32_t command_count = XsUtils::byteorder2host<uint32_t>(mach_header->ncmds, is_little_endian);
        uint32_t command_size = XsUtils::byteorder2host<uint32_t>(mach_header->sizeofcmds, is_little_endian);
        if (command_count <= 0 || command_size <= 0) {
            rst = ft_unknown;
            return rst;
        }

        auto machine = XsUtils::byteorder2host<int32_t>(mach_header->cputype, is_little_endian);
        switch (machine) {
        case CPU_TYPE_X86:
            rst = ft_macho_x86;
            break;
        case CPU_TYPE_ARM:
            rst = ft_macho_arm;
            break;
        default:
            rst = ft_macho_32;
            break;
        }
    }
    else {
        // 64-fit non-fat mach-o
        auto mach_header = (const MACH_HEADER_64*)data;
        uint32_t command_count = XsUtils::byteorder2host<uint32_t>(mach_header->ncmds, is_little_endian);
        uint32_t command_size = XsUtils::byteorder2host<uint32_t>(mach_header->sizeofcmds, is_little_endian);
        if (command_count <= 0 || command_size <= 0) {
            rst = ft_unknown;
            return rst;
        }

        auto machine = XsUtils::byteorder2host<int32_t>(mach_header->cputype, is_little_endian);
        switch (machine) {
        case CPU_TYPE_ARM64:
        case CPU_TYPE_ARM64_32:
            rst = ft_macho_arm64;
            break;
        case CPU_TYPE_X86_64:
            rst = ft_macho_x64;
            break;
        default:
            rst = ft_macho_64;
        }
    }

    return rst;
}

int is_macho_fat(const uint8_t* data, uint32_t size, ifile* file = nullptr) {
    auto rst = ft_unknown;
    if (data == nullptr || size < sizeof(MACH_FAT_HEADER)) {
        return rst;
    }

    bool is_little_endian = false, is_binary64 = false;
    uint32_t h_magic = XsUtils::byteorder2host<uint32_t>(*(const uint32_t*)(data), true);
    switch (h_magic) {
    case FAT_MAGIC:
        is_little_endian = true, is_binary64 = false;
        break;
    case FAT_MAGIC_64:
        is_little_endian = true, is_binary64 = true;
        break;
    case FAT_CIGAM:
        is_little_endian = false, is_binary64 = false;
        break;
    case FAT_CIGAM_64:
        is_little_endian = false, is_binary64 = true;
        break;
    default:
        return rst;
    }


    // 32-bit fat mach-o
    if (!is_binary64) {
        rst = ft_macho_fat32;
        auto fat_header = (const MACH_FAT_HEADER*)data;
        uint32_t arch_count = XsUtils::byteorder2host<uint32_t>(fat_header->nfat_arch, is_little_endian);

        // 校验 header 长度
        if (size < (sizeof(MACH_FAT_HEADER) + arch_count * sizeof(MACH_FAT_ARCH))) {
            rst = ft_unknown;
            return rst;
        }

        // 校验当前文件大小是否大于 fat 结构中最后包含的 arch 的 offset + size
        auto last_arch = (const MACH_FAT_ARCH*)(data + sizeof(MACH_FAT_HEADER) + (arch_count - 1) * sizeof(MACH_FAT_ARCH));
        uint64_t offset = XsUtils::byteorder2host<uint32_t>(last_arch->offset, is_little_endian);
        uint64_t arch_size = XsUtils::byteorder2host<uint32_t>(last_arch->size, is_little_endian);
        if (size < offset + arch_size) {
            rst = ft_unknown;
            return rst;
        }
    }
    else {
        // 64-bit fat mach-o
        rst = ft_macho_fat64;
        auto fat_header = (const MACH_FAT_HEADER*)data;
        uint32_t arch_count = XsUtils::byteorder2host<uint32_t>(fat_header->nfat_arch, is_little_endian);
        if (size < (sizeof(MACH_FAT_HEADER) + arch_count * sizeof(MACH_FAT_ARCH_64))) {
            rst = ft_unknown;
            return rst;
        }

        auto last_arch = (const MACH_FAT_ARCH_64*)(data + sizeof(MACH_FAT_HEADER) + (arch_count - 1) * sizeof(MACH_FAT_ARCH_64));
        uint64_t offset = XsUtils::byteorder2host<uint64_t>(last_arch->offset, is_little_endian);
        uint64_t arch_size = XsUtils::byteorder2host<uint64_t>(last_arch->size, is_little_endian);
        if (size < offset + arch_size) {
            rst = ft_unknown;
            return rst;
        }
    }

    return rst;
}

int is_vcx_pdb(const uint8_t* data, uint32_t size, ifile* file) {
    if (data == nullptr || size < 0x30) {
        return ft_unknown;
    }

#if 0
    const uint8_t byPDBSign1[] =  // "Microsoft C/C++ MSF 7.00\x0D\x0A\x1ADS\0\0"
    {
        0x4D, 0x69, 0x63, 0x72, 0x6F, 0x73, 0x6F, 0x66, 0x74, 0x20, 0x43, 0x2F, 0x43, 0x2B, 0x2B, 0x20,
        0x4D, 0x53, 0x46, 0x20, 0x37, 0x2E, 0x30, 0x30, 0x0D, 0x0A, 0x1A, 0x44, 0x53, 0x00
    };
    const uint8_t byPDBSign2[] =  // "Microsoft C/C++ program database 2.00\x0D\x0A\x1AJG\0\0"
    {
        0x4D, 0x69, 0x63, 0x72, 0x6F, 0x73, 0x6F, 0x66, 0x74, 0x20, 0x43, 0x2F, 0x43, 0x2B, 0x2B, 0x20,
        0x70, 0x72, 0x6F, 0x67, 0x72, 0x61, 0x6D, 0x20, 0x64, 0x61, 0x74, 0x61, 0x62, 0x61, 0x73, 0x65,
        0x20, 0x32, 0x2E, 0x30, 0x30, 0x0D, 0x0A, 0x1A, 0x4A, 0x47, 0x00, 0x00
    };
    const uint8_t byILKSign[] =  // "Microsoft Linker Database\x0A\x0A"
    {
        0x4D, 0x69, 0x63, 0x72, 0x6F, 0x73, 0x6F, 0x66, 0x74, 0x20, 0x4C, 0x69, 0x6E, 0x6B, 0x65, 0x72,
        0x20, 0x44, 0x61, 0x74, 0x61, 0x62, 0x61, 0x73, 0x65, 0x0A, 0x0A
    };
#endif

    uint32_t pos = 0, ver_pos_max = 0x20;
    for (; pos < size && pos < ver_pos_max && *(data + pos) != 0x1A; ++pos);
    if (pos >= ver_pos_max || ucs::nicmp((const char*)data, "Microsoft C/C++", 15)) {
        return ft_unknown;
    }
    /* signature : DS */
    if ((*(data + pos + 1) != 0x44) || (*(data + pos + 2) != 0x53) || (*(data + pos + 5) != 0x0)) {
        return ft_unknown;
    }
    pos += 6;

    if (pos + sizeof(uint32_t) * 3 >= size) {
        return ft_unknown;
    }
    uint32_t page_size = XsUtils::byteorder2host(*(const uint32_t*)(data + pos), true);
    uint32_t page_cnts = XsUtils::byteorder2host(*(const uint32_t*)(data + pos + 8), true);
    if (page_size != 0x0400 && page_size != 0x0800 && page_size != 0x1000) {
        return ft_unknown;
    }
    if (file) {
        auto filesize = file->size();
        if (filesize >= page_size * (page_cnts + 1) || filesize <= page_size * (page_cnts - 1)) {
            return ft_unknown;
        }
    }
    return ft_pdb_vcx;
}

int is_pdf(const uint8_t* data, uint32_t size, ifile* file = nullptr) {
    int rst = ft_unknown;
    for (uint32_t pos = 0, end = std::min<uint32_t>(size, 1024); data && pos < end; pos++) {
        if (memcmp(data + pos, "%PDF", 4) != 0) {
            continue;
        }
        if (pos == 0 || (*(data + pos - 1) == 10 || *(data + pos - 1) == 13)) {
            rst = ft_pdf;
            break;
        }
    }
    return rst;
}

int is_pcx(const uint8_t* data, uint32_t size, ifile* file = nullptr) {
    if (data == nullptr || size < 3) {
        return ft_unknown;
    }

    if (data[0] == 0x0A && data[1] <= 0x05 && (data[2] == 0x0 || data[2] == 0x1)) {
        return ft_pcx;
    }
    return ft_unknown;
}

int is_pif(const uint8_t* data, uint32_t size, ifile* file = nullptr) {
    // offset 0x171: "MICROSOFT PIFEX\0"
    const uint8_t MAGIC_PIF[] = { 0x4D, 0x49u, 0x43u, 0x52u, 0x4Fu, 0x53u, 0x4Fu, 0x46u, 0x54u, 0x20u, 0x50u, 0x49u, 0x46u, 0x45u, 0x58u };

    uint32_t offset = 0x171;
    if (size >= offset + sizeof(MAGIC_PIF) && 0 == memcmp(data + offset, MAGIC_PIF, sizeof(MAGIC_PIF))) {
        return ft_ms_pif;
    }
    return ft_unknown;
}

int is_ms_lrh(const uint8_t* data, uint32_t size, ifile* file = nullptr) {
    // TODO:
    return ft_unknown;
}

int is_morphine(const uint8_t* data, uint32_t size, ifile* file = nullptr) {
    // TODO:
    return ft_unknown;
}

int is_rar(const uint8_t* data, uint32_t size, ifile* file = nullptr) {
    if (size < 16) {
        return ft_unknown;
    }

    enum RARFORMAT { RARFMT_NONE, RARFMT14, RARFMT15, RARFMT50, RARFMT_FUTURE };
    const uint32_t MAXSFXSIZE = 0x200 * 0x400; /* should be 0x200000, here we give it 512k */
    const uint32_t SIZEOF_MARKHEAD3 = 7;
    const uint32_t SIZEOF_MARKHEAD5 = 8;

    auto rar_signature = [](const uint8_t* head, uint32_t size) {
        const uint8_t MAGIC_RAR_14[] = { 0x52, 0x45, 0x7E, 0x5E };
        const uint8_t MAGIC_RAR_B[] = { 0x52, 0x61, 0x72, 0x21, 0x1A, 0x07 };
        const uint8_t MAGIC_RAR_UNSURE_FROM_COMODO[] = { 0x52, 0x61, 0x70, 0x21 };

        RARFORMAT rar_fmt = RARFMT_NONE;
        do {
            if (head[0] != 0x52) {
                break;
            }
            if (memcmp(head, MAGIC_RAR_14, sizeof(MAGIC_RAR_14)) == 0) {
                rar_fmt = RARFMT14;
            }
            else if (memcmp(head, MAGIC_RAR_B, sizeof(MAGIC_RAR_B)) == 0) {
                if (*(head + 6) == 0) {
                    rar_fmt = RARFMT15;
                }
                else if (*(head + 6) == 1) {
                    rar_fmt = RARFMT50;
                }
                else if (*(head + 6) > 1 && *(head + 6) < 5) {
                    rar_fmt = RARFMT_FUTURE;
                }
            }
            else if (memcmp(head, MAGIC_RAR_UNSURE_FROM_COMODO, sizeof(MAGIC_RAR_UNSURE_FROM_COMODO)) == 0) {
                rar_fmt = RARFMT_FUTURE;
                assert(rar_fmt == RARFMT_FUTURE);
            }
        } while (false);

        return rar_fmt;
    };

    RARFORMAT rar_fmt = RARFMT_NONE;
    uint32_t pos = 0, header_size = 0;

    uint32_t max_search_size = std::min(MAXSFXSIZE, size);
    for (uint32_t i = 0; i < max_search_size - 16; i++) {
        if (*(data + i) != 0x52 || (rar_fmt = rar_signature(data + i, max_search_size - i)) == RARFMT_NONE) {
            continue;
        }
        if (rar_fmt == RARFMT14 && i > 0 && i < 28 && max_search_size > 31) {
            auto p = data + 28 - i;
            if (p[0] != 0x52 || p[1] != 0x53 || p[2] != 0x46 || p[3] != 0x58) {
                continue;
            }
        }
        pos = i;
        break;
    }

    int ftype = ft_unknown;
    if (rar_fmt == RARFMT_FUTURE || rar_fmt == RARFMT_NONE) {
    }
    else if (rar_fmt == RARFMT50) {
        if (*(data + pos + SIZEOF_MARKHEAD3) == 0) {
            ftype = ft_rar5;
            header_size = SIZEOF_MARKHEAD5;
        }
    }
    else {
        ftype = ft_rar;
        header_size = SIZEOF_MARKHEAD3;
    }

    return ftype;
}

int is_swc_swf(const uint8_t* data, uint32_t size, ifile* file = nullptr) {
    if (data == nullptr || size < 4) {
        return ft_unknown;
    }

    /*
    * 可能的 swc 文档：
    *   包含Adobe Flex应用程序使用的一组组件和其他资产； 包括一个 SWF 文件和catalog.xml文件
    *   ，其中列出了包的内容以及各个组件； 由Animate创作工具生成，并使用PKZIP存档格式压缩。
    */

    // "FWS", swf, Version 1.0 ---- Version 32.0
    const uint8_t magic_swf[] = { 0x46, 0x57, 0x53 };
    // "CWS", swc, Version 1.0 ---- Version 32.0
    const uint8_t magic_swc[] = { 0x43, 0x57, 0x53 };

    if (0 == memcmp(data, magic_swf, sizeof(magic_swf)) && (data[3] >= 0x01) && (data[3] <= 0x20)) {
        return ft_swf;
    }
    if (0 == memcmp(data, magic_swc, sizeof(magic_swc)) && (data[3] >= 0x01) && (data[3] <= 0x20)) {
        return ft_swc;
    }
    return ft_unknown;
}

int is_tar(const uint8_t* data, uint32_t size, ifile* file = nullptr) {
    const uint8_t byTarSign1[] = { 0x75, 0x73, 0x74, 0x61, 0x72 };  // "ustar"
    const uint8_t byTarSign2[] = { 0x47, 0x4E, 0x55, 0x74, 0x61, 0x72 };  // "GNUtar"

    if (data == nullptr || size < 512) {
        return ft_unknown;
    }

    // 1. 检查特征码
    if ((memcmp(&data[0x101], byTarSign1, sizeof(byTarSign1)) == 0) ||  // 0x101: char magic[8];
        (memcmp(&data[0x101], byTarSign2, sizeof(byTarSign2)) == 0)) {
        return ft_tar;
    }

    // 无特征码, 尝试比较校验和
    uint32_t nChkSum = 0;
    uint32_t nChkSum1 = 0;

    // 2. 取包头里的CheckSum(以八进制ASCII码方式表示)
    for (int i = 0x94; i < 0x9C; i++) {  // 0x094: char chksum[8]
        if (data[i] == 0) break;
        if ((data[i] < '0') || (data[i] > '7')) continue;  // 无效数据, 是否应该直接结束?
        nChkSum = (nChkSum << 3) + (data[i] - '0');
    }

    // 3. 计算校验和
    // 计算方法为除去checksum字段其他所有的512-8共504个字节的ascii码相加的值再加上256(checksum当作八个空格，即8*0x20）
    for (int i = 0; i < 512; i++) nChkSum1 += data[i];
    // 扣去CheckSum位置的当前值
    for (int i = 0x94; i < 0x9C; i++) nChkSum1 -= data[i];
    // 加上CheckSum的缺省值(八个空格)
    nChkSum1 += (0x20 * 8);

    return nChkSum1 == nChkSum ? ft_tar : ft_unknown;
}

int is_ttc(const uint8_t* data, uint32_t size, ifile* file = nullptr) {
    /* TTC（TrueType Font Collection）字体是TrueType字体集成文件 */
    const uint8_t magic_ttc[] = { 0x74, 0x74, 0x63, 0x66 }; // "ttcf", ttc
    if (data == nullptr || size < 8 || memcmp(data, magic_ttc, sizeof(magic_ttc))) {
        return ft_unknown;
    }
    // TTC version
    if (data[4] == data[6] == data[7] == 0x00 && (data[5] == 0x01 || data[5] == 0x02)) {
        return ft_ttc;
    }
    return ft_unknown;
}

int is_ttf(const uint8_t* data, uint32_t size, ifile* file = nullptr) {
    if (data == nullptr || size < 16) {
        return ft_unknown;
    }

    /* otf, ttf : http://www.microsoft.com/typography/otspec/otff.htm */
    const uint8_t magic_ttf[] = { 0x00, 0x01, 0x00, 0x00 };
    const uint8_t magic_otto[] = { 0x4F, 0x54, 0x54, 0x4F };
    if (memcmp(data, magic_ttf, sizeof(magic_ttf)) && memcmp(data, magic_otto, sizeof(magic_otto))) {
        return ft_unknown;
    }

    uint32_t uMagic = XsUtils::byteorder2host<uint32_t>(*(const uint32_t*)(data + 12), false);
    if ((uMagic == 0x44534947) || (uMagic == 0x4C545348) || (uMagic == 0x4F532F32) || (uMagic == 0x47535542) || (uMagic == 0x42415345))
    {  // "DSIG", "LTSH", "OS/2", "GSUB", "BASE"
        return ft_ttf;
    }

    uint16_t wNumTables = XsUtils::byteorder2host<uint16_t>(*(const uint16_t*)(data + 4), false);
    uint16_t wSearchRange = XsUtils::byteorder2host<uint16_t>(*(const uint16_t*)(data + 6), false);
    uint16_t wRangeShift = XsUtils::byteorder2host<uint16_t>(*(const uint16_t*)(data + 10), false);
    if (wNumTables && wSearchRange && ((wNumTables * 0x10 - wSearchRange) == wRangeShift)) {
        return ft_ttf;
    }
    return ft_unknown;
}

int is_uue(const uint8_t* data, uint32_t size, ifile* file = nullptr) {
    if (data == nullptr || size < 4) {
        return ft_unknown;
    }

    uint32_t pos = 0;
    for (; pos < size && is_space(*(data + pos)); ++pos) {}

    const uint8_t magic[] = { 0x62u, 0x65u, 0x67u, 0x69u, 0x6E, 0x20, 0x36 };
    bool is = (pos + sizeof(magic) <= size && 0 == memcmp(data + pos, magic, sizeof(magic)));
    return is ? ft_uue : ft_unknown;
}

int is_wmf(const uint8_t* data, uint32_t size, ifile* file = nullptr) {
    if (data == nullptr || size < 10) {
        return ft_unknown;
    }
    const uint8_t magic[] = { 0x01, 0x00, 0x09, 0x00, 0x00, 0x03 };
    if (0 == memcmp(data, magic, sizeof(magic))) {
        assert(size == uint32_littlendian(data + sizeof(magic)));
        return ft_uue;
    }
    return ft_unknown;
}

int do_detect_zip(ifile* file) {
    if (file == nullptr) {
        return ft_unknown;
    }

    std::vector<std::string> zip_items;
#if true
    if (0 > parse_zip_items(file, zip_items)) {
        return ft_unknown;
    }
#else
    for (int32_t i = 0; i < ziper->item_cnt(); i++) {
        auto path = ziper->item_utf8path(i);
        zip_items.push_back(path ? path : "");
    }
#endif

    int filetype = ft_unknown;
    do {
        const uint32_t ooxml_expected_xl_index = 3;
        const char* ooxml_expected_files[] = {
            "[Content_Types].xml",
            "docProps/app.xml",
            /* the following should only one be shown */
            "word/document.xml",
            "xl/workbook.xml",
            "ppt/presentation.xml",
        };

        uint32_t found = 0;
        for (uint32_t i = 0; i < zip_items.size(); i++) {
            for (int j = 0, expected = (sizeof(ooxml_expected_files) / sizeof(ooxml_expected_files[0])); j < expected; j++) {
                if (0 == ucs::icmp(zip_items[i].c_str(), ooxml_expected_files[j])) {
                    found |= (1 << j);
                    break;
                }
            }
        }

        if (0x3 != (found & 0x3) || 0 == (found >> 2)) {
            break;
        }

        if ((found >> 2) & 0x1) {
            filetype = (ft_unknown == filetype ? ft_ms_docx : ft_unknown);
        }
        if ((found >> 3) & 0x1) {
            filetype = (ft_unknown == filetype ? ft_ms_xlsx : ft_unknown);
        }
        if ((found >> 4) & 0x1) {
            filetype = (ft_unknown == filetype ? ft_ms_pptx : ft_unknown);
        }

        /* TODO: check xml */

        break;
    } while (false);

    if (filetype != ft_unknown) {
        return filetype;
    }

    do {
        const uint32_t expected_found = 0x7;
        const char* expected_files[] = {
            "AndroidManifest.xml",
            "META-INF/MANIFEST.MF",
            "classes.dex",
        };

        uint32_t found = 0;
        for (uint32_t i = 0; i < zip_items.size(); i++) {
            for (int j = 0, expected = (sizeof(expected_files) / sizeof(expected_files[0])); j < expected; j++) {
                if (0 == ucs::icmp(zip_items[i].c_str(), expected_files[j])) {
                    found |= (1 << j);
                    break;
                }
            }
        }

        filetype = (expected_found == found) ? ft_apk : ft_unknown;

        /* TODO: check xml */

        break;
    } while (false);

    if (filetype != ft_unknown) {
        return filetype;
    }

    do {
        const char* expected_files[] = {
            "META-INF/MANIFEST.MF"
        };
        const char* class_suffix = ".class";

        uint32_t found = 0, class_cnt = 0, total_cnt = 0;
        for (uint32_t i = 0; i < zip_items.size(); i++) {
            if (found == 0 && 0 == ucs::icmp(zip_items[i].c_str(), expected_files[0])) {
                found = 1;
                continue;
            }
            auto p = strrchr(zip_items[i].c_str(), '.');
            if (p) {
                if (0 == ucs::cmp(p, class_suffix)) {
                    class_cnt++;
                }
                total_cnt++;
            }
        }

        filetype = (found && total_cnt && (class_cnt * 2 > total_cnt)) ? ft_jar : ft_unknown;
        break;
    } while (false);

    if (filetype != ft_unknown) {
        return filetype;
    }

    return filetype;
}

int is_zip(const uint8_t* data, uint32_t size, ifile* file = nullptr) {
    if (data == nullptr || size < 4) {
        return ft_unknown;
    }

    int rst = ft_unknown;
    const uint8_t magic_a[] = { 0x50, 0x4B, 3, 4 };
    const uint8_t magic_b[] = { 0x50, 0x4B, 5, 6 };
    constexpr uint32_t magic_size = sizeof(magic_a);

    for (uint32_t pos = 0; pos + magic_size < size; pos += magic_size) {
        if (0 == memcmp(data + pos, magic_a, magic_size)
            || 0 == memcmp(data + pos, magic_b, magic_size)) {
            rst = ft_zip;
            break;
        }
        if (0 == memcmp(data + pos, magic_a, 2)) {
            continue;
        }
        break;
    }

    if (rst == ft_zip) {
        auto new_rst = do_detect_zip(file);
        if (new_rst != ft_unknown) {
            rst = new_rst;
        }
    }

    return rst;
}

int is_ogg(const uint8_t* data, uint32_t size, ifile* file = nullptr) {
    /* ogg : https://xiph.org/ogg/ */
    uint8_t magic[] = { 0x4F, 0x67, 0x67, 0x53 }; // "OggS"
    if (nullptr == data || size < 27 || 0 != memcmp(magic, data, sizeof(magic))) {
        return ft_unknown;
    }

    // 试图找下一个Ogg Page
    uint32_t nTableOffset = 27 + data[26];  // data[26] = PageCount
    if (size < nTableOffset) {
        return ft_unknown;
    }

    uint32_t nTableSize = 0;
    for (uint32_t i = 27; i < nTableOffset; i++) nTableSize += data[i];
    if (size < nTableOffset + nTableSize + 4) {   // 下一个Ogg Page标志有效
        return ft_unknown;
    }

    if (0 != memcmp(magic, data + nTableOffset + nTableSize, sizeof(magic))) {
        return ft_unknown;
    }

    return ft_ogg;
}

int is_mkv(const uint8_t* data, uint32_t size, ifile* file = nullptr) {
    if (data == nullptr || size < 0x10) {
        return ft_unknown;
    }

    /* mkv : http://www.matroska.org/technical/specs/index.html */
    uint8_t magic_p0[] = { 0x1A, 0x45, 0xDF, 0xA3 }; // offset: 0
    uint8_t magic_p8[] = { 0x6D, 0x61, 0x74, 0x72, 0x6F, 0x73, 0x6B, 0x61 }; // offset: 8, "matroska"

    if (0 == memcmp(data, magic_p0, sizeof(magic_p0))
        && 0 == memcmp(data + 8, magic_p8, sizeof(magic_p8))) {
        return ft_mkv;
    }

    return ft_unknown;
}

int is_flv(const uint8_t* data, uint32_t size, ifile* file = nullptr) {
    if (data == nullptr || size < 9) {
        return ft_unknown;
    }

    // The FLV video format was developed by Adobe Systems for its Flash Player.
    const uint8_t magic_flv_p0[] = { 0x46, 0x4C, 0x56 }; // "FLV"
    const uint8_t magic_flv_p5[] = { 0x00, 0x00, 0x00, 0x09 };

    if (0 == memcmp(data, magic_flv_p0, sizeof(magic_flv_p0)
        && (data[4] & 0xFA) == 0)
        && 0 == memcmp(data + 5, magic_flv_p5, sizeof(magic_flv_p5))) {
        return ft_flv;
    }
    return ft_unknown;
}

int is_wps(const uint8_t* data, uint32_t size, ifile* file = nullptr) {
    if (data == nullptr || size < 6) {
        return ft_unknown;
    }

    const uint8_t magic_wps[] = { 0x57, 0x50, 0x53 }; // "WPSx"
    if (0 != memcmp(data + 2, magic_wps, sizeof(magic_wps))) {
        return ft_unknown;
    }

    if (((data[0] >= 0x81 && data[0] <= 0x84) || data[0] == 0x90)
        && (data[1] == 0xFE || data[1] == 0xFF)) {
        // wps
    }
    return ft_unknown;
}

int is_aiff(const uint8_t* data, uint32_t size, ifile* file = nullptr) {
    // TODO: 
    // AIF、AIFF 以及 AIFC，AIFF 音频交换文件格式（Audio Interchange File Format）
    return ft_unknown;
}

int is_mpeg(const uint8_t* data, uint32_t size, ifile* file = nullptr) {
    // TODO:
    /* mpeg : http://www.mpucoder.com/DVD/packhdr.html */
    return ft_unknown;
}

const ftype_checker_t ftype_checker[] = {
    ftype_checker_t{0x4d, ftype_magic_t{ ft_pe, 0, 0, 0, 0 }, is_pe},
    ftype_checker_t{0x7f, ftype_magic_t{ ft_elf, 0, 0, 0, 0 }, is_elf},
    ftype_checker_t{0xD0, ftype_magic_t{ ft_ole, 0, 0, 0, 0 }, is_ole}, /* ft_ole, ft_msi */
    ftype_checker_t{0x37, ftype_magic_t{ ft_7z, 0, 0, MAGIC_7ZIP, sizeof(MAGIC_7ZIP) }, nullptr},
    ftype_checker_t{0x01, ftype_magic_t{ ft_pnf, 0, 0, MAGIC_PNF, sizeof(MAGIC_PNF) }, nullptr},
    ftype_checker_t{0x1F, ftype_magic_t{ ft_z, 0, 0, MAGIC_Z, sizeof(MAGIC_Z) }, nullptr},
    ftype_checker_t{0x21, ftype_magic_t{ ft_deb, 0, 0, MAGIC_DEB, sizeof(MAGIC_DEB) }, nullptr},
    ftype_checker_t{0x21, ftype_magic_t{ ft_ms_lib, 0, 0, MAGIC_MSLIB, sizeof(MAGIC_MSLIB) }, nullptr},
    ftype_checker_t{0x21, ftype_magic_t{ ft_pst, 0, 0, MAGIC_PST, sizeof(MAGIC_PST) }, nullptr},
    ftype_checker_t{0x2E, ftype_magic_t{ ft_rm, 0, 0, MAGIC_RM_A, sizeof(MAGIC_RM_A) }, nullptr},
    ftype_checker_t{0x66, ftype_magic_t{ ft_rm, 0, 0, MAGIC_RM_B, sizeof(MAGIC_RM_B) }, nullptr},
    ftype_checker_t{0x39, ftype_magic_t{ ft_asf, 0, 0, MAGIC_ASF, sizeof(MAGIC_ASF) }, nullptr},
    ftype_checker_t{0x3F, ftype_magic_t{ ft_gidhlp, 0, 0, MAGIC_GIDHLP, sizeof(MAGIC_GIDHLP) }, nullptr},
    ftype_checker_t{0x41, ftype_magic_t{ ft_acad, 0, 0, MAGIC_AutoCAD_A, sizeof(MAGIC_AutoCAD_A) }, nullptr},
    ftype_checker_t{0x41, ftype_magic_t{ ft_acad, 0, 0, MAGIC_AutoCAD_B, sizeof(MAGIC_AutoCAD_B) }, nullptr},
    ftype_checker_t{0x43, ftype_magic_t{ ft_ms_urlcache, 0, 0, MAGIC_MSURLCACHE, sizeof(MAGIC_MSURLCACHE) }, nullptr},
    ftype_checker_t{0x44, ftype_magic_t{ ft_ms_dds, 0, 0, MAGIC_MS_DDS, sizeof(MAGIC_MS_DDS) }, nullptr},
    ftype_checker_t{0x47, ftype_magic_t{ ft_gif, 0, 0, MAGIC_GIF_A, sizeof(MAGIC_GIF_A) }, nullptr},
    ftype_checker_t{0x47, ftype_magic_t{ ft_gif, 0, 0, MAGIC_GIF_B, sizeof(MAGIC_GIF_B) }, nullptr},
    ftype_checker_t{0x49, ftype_magic_t{ ft_ms_hxq, 0, 0, MAGIC_MS_HXQ, sizeof(MAGIC_MS_HXQ) }, nullptr},
    ftype_checker_t{0x49, ftype_magic_t{ ft_tiff, 0, 0, MAGIC_TIFF_LE, sizeof(MAGIC_TIFF_LE) }, nullptr},
    ftype_checker_t{0x4D, ftype_magic_t{ ft_tiff, 0, 0, MAGIC_TIFF_BE_A, sizeof(MAGIC_TIFF_BE_A) }, nullptr},
    ftype_checker_t{0x4D, ftype_magic_t{ ft_tiff, 0, 0, MAGIC_TIFF_BE_B, sizeof(MAGIC_TIFF_BE_B) }, nullptr},
    ftype_checker_t{0x4C, ftype_magic_t{ ft_ms_lnk, 0, 0, MAGIC_LNK, sizeof(MAGIC_LNK) }, nullptr},
    ftype_checker_t{0x4C, ftype_magic_t{ ft_coff, 0, 0, MAGIC_COFF_OBJ, sizeof(MAGIC_COFF_OBJ) }, nullptr},
    ftype_checker_t{0x4D, ftype_magic_t{ ft_cab, 0, 0, MAGIC_MS_CAB, sizeof(MAGIC_MS_CAB) }, nullptr},
    ftype_checker_t{0x4D, ftype_magic_t{ ft_ape, 0, 0, MAGIC_APE, sizeof(MAGIC_APE) }, nullptr},
    ftype_checker_t{0x4D, ftype_magic_t{ ft_ctm, 0, 0, MAGIC_CTM, sizeof(MAGIC_CTM) }, nullptr},
    ftype_checker_t{0x4D, ftype_magic_t{ ft_hta, 0, 0, MAGIC_HTA, sizeof(MAGIC_HTA) }, nullptr},
    ftype_checker_t{0x4D, ftype_magic_t{ ft_mid, 0, 0, MAGIC_MID, sizeof(MAGIC_MID) }, nullptr},
    ftype_checker_t{0x4D, ftype_magic_t{ ft_wim, 0, 0, MAGIC_WIM, sizeof(MAGIC_WIM) }, nullptr},
    ftype_checker_t{0x4B, ftype_magic_t{ ft_mscompress, 0, 0, MAGIC_MSC_B, sizeof(MAGIC_MSC_B) }, nullptr},
    ftype_checker_t{0x53, ftype_magic_t{ ft_mscompress, 0, 0, MAGIC_MSC_A, sizeof(MAGIC_MSC_A) }, nullptr},
    ftype_checker_t{0x53, ftype_magic_t{ ft_sqlite, 0, 0, MAGIC_SQLITE3, sizeof(MAGIC_SQLITE3) }, nullptr},
    ftype_checker_t{0x52, ftype_magic_t{ ft_riff, 0, 0, MAGIC_RIFF, sizeof(MAGIC_RIFF) }, nullptr},
    ftype_checker_t{0x52, ftype_magic_t{ ft_ms_reg4, 0, 0, MAGIC_REG4_B, sizeof(MAGIC_REG4_B) }, nullptr},
    ftype_checker_t{0xFF, ftype_magic_t{ ft_ms_reg4, 0, 0, MAGIC_REG4_A, sizeof(MAGIC_REG4_A) }, nullptr},
    ftype_checker_t{0x57, ftype_magic_t{ ft_ms_reg5, 0, 0, MAGIC_REG5, sizeof(MAGIC_REG5) }, nullptr},
    ftype_checker_t{0x56, ftype_magic_t{ ft_pch, 0, 0, MAGIC_PCH, sizeof(MAGIC_PCH) }, nullptr},
    ftype_checker_t{0x60, ftype_magic_t{ ft_arj, 0, 0, MAGIC_ARJ, sizeof(MAGIC_ARJ) }, nullptr},
    ftype_checker_t{0x72, ftype_magic_t{ ft_ms_hiv, 0, 0, MAGIC_MS_HIVE, sizeof(MAGIC_MS_HIVE) }, nullptr},
    ftype_checker_t{0x78, ftype_magic_t{ ft_xar, 0, 0, MAGIC_XAR, sizeof(MAGIC_XAR) }, nullptr},
    ftype_checker_t{0x7A, ftype_magic_t{ ft_sis, 0, 0, MAGIC_SIS9, sizeof(MAGIC_SIS9) }, nullptr},
    ftype_checker_t{0x7B, ftype_magic_t{ ft_rtf, 0, 0, MAGIC_RTF, sizeof(MAGIC_RTF) }, nullptr},
    ftype_checker_t{0x89, ftype_magic_t{ ft_png, 0, 0, MAGIC_PNG, sizeof(MAGIC_PNG) }, nullptr},
    ftype_checker_t{0xD0, ftype_magic_t{ ft_compound, 0, 0, MAGIC_CompoundUUID, sizeof(MAGIC_CompoundUUID) }, nullptr},
    ftype_checker_t{0xD7, ftype_magic_t{ ft_apm_wmf, 0, 0, MAGIC_APM_WMF, sizeof(MAGIC_APM_WMF) }, nullptr},
    ftype_checker_t{0xED, ftype_magic_t{ ft_rpm, 0, 0, MAGIC_RPM, sizeof(MAGIC_RPM) }, nullptr},
    ftype_checker_t{0xF0, ftype_magic_t{ ft_borland_lib, 0, 0, MAGIC_BorlandLIB, sizeof(MAGIC_BorlandLIB) }, nullptr},
    ftype_checker_t{0xFF, ftype_magic_t{ ft_jpg, 0, 0, 0, 0 }, is_jpg},
    ftype_checker_t{0xCF, ftype_magic_t{ ft_oe_dbx, 0, 0, 0, 0 }, is_dbx},
    ftype_checker_t{0xCA, ftype_magic_t{ ft_java_cls, 0, 0, 0, 0 }, is_java_cls},
    ftype_checker_t{0x74, ftype_magic_t{ ft_ttc, 0, 0, 0, 0 }, is_ttc},
    ftype_checker_t{0x66, ftype_magic_t{ ft_flac, 0, 0, 0, 0 }, is_flac},
    ftype_checker_t{0x4F, ftype_magic_t{ ft_ogg, 0, 0, 0, 0 }, is_ogg},
    ftype_checker_t{0x49, ftype_magic_t{ ft_chm, 0, 0, 0, 0 }, is_chm},
    ftype_checker_t{0x46, ftype_magic_t{ ft_flv, 0, 0, 0, 0 }, is_flv},
    ftype_checker_t{0x46, ftype_magic_t{ ft_swf, 0, 0, 0, 0 }, is_swc_swf},
    ftype_checker_t{0x43, ftype_magic_t{ ft_swc, 0, 0, 0, 0 }, is_swc_swf},
    ftype_checker_t{0x42, ftype_magic_t{ ft_bmp, 0, 0, 0, 0 }, is_bmp},
    ftype_checker_t{0x42, ftype_magic_t{ ft_bzip2, 0, 0, 0, 0 }, is_bzip2}, /* ft_bzip2, ft_dmg */
    ftype_checker_t{0x30, ftype_magic_t{ ft_ms_asf, 0, 0, 0, 0 }, is_ms_advanced_stream},
    ftype_checker_t{0x1F, ftype_magic_t{ ft_gzip, 0, 0, 0, 0 }, is_gzip},
    ftype_checker_t{0x1A, ftype_magic_t{ ft_mkv, 0, 0, 0, 0 }, is_mkv},
    ftype_checker_t{0x1A, ftype_magic_t{ ft_arc, 0, 0, 0, 0 }, is_arc},
    ftype_checker_t{0x0A, ftype_magic_t{ ft_pcx, 0, 0, 0, 0 }, is_pcx},
    ftype_checker_t{0x01, ftype_magic_t{ ft_emf, 0, 0, 0, 0 }, is_emf},
    ftype_checker_t{0x01, ftype_magic_t{ ft_wmf, 0, 0, 0, 0 }, is_wmf},
    ftype_checker_t{0x00, ftype_magic_t{ ft_zip, 0, 0, 0, 0 }, is_zip}, /* ft_apk, ft_jar */
    ftype_checker_t{0x00, ftype_magic_t{ ft_tar, 0, 0, 0, 0 }, is_tar},
    ftype_checker_t{0x00, ftype_magic_t{ ft_rar, 0, 0, 0, 0 }, is_rar},
    ftype_checker_t{0x00, ftype_magic_t{ ft_pdf, 0, 0, 0, 0 }, is_pdf},
    ftype_checker_t{0x00, ftype_magic_t{ ft_mp3, 0, 0, 0, 0 }, is_mp3},
    ftype_checker_t{0x00, ftype_magic_t{ ft_ace, 0, 0, 0, 0 }, is_ace},
    ftype_checker_t{0x00, ftype_magic_t{ ft_cpio, 0, 0, 0, 0 }, is_cpio},
    ftype_checker_t{0x00, ftype_magic_t{ ft_ico, 0, 0, 0, 0 }, is_ico},
    ftype_checker_t{0x00, ftype_magic_t{ ft_cur, 0, 0, 0, 0 }, is_cur},
    ftype_checker_t{0x00, ftype_magic_t{ ft_dmg, 0, 0, 0, 0 }, is_dmg},
    ftype_checker_t{0x00, ftype_magic_t{ ft_hfs, 0, 0, 0, 0 }, is_hfs},
    ftype_checker_t{0x00, ftype_magic_t{ ft_lzh, 0, 0, 0, 0 }, is_lzh},
    ftype_checker_t{0x00, ftype_magic_t{ ft_iso, 0, 0, 0, 0 }, is_iso}, /* ft_iso, ft_udf */
    ftype_checker_t{0x00, ftype_magic_t{ ft_inno, 0, 0, 0, 0 }, is_inno},
    ftype_checker_t{0x00, ftype_magic_t{ ft_pdb_vcx, 0, 0, 0, 0 }, is_vcx_pdb},
    ftype_checker_t{0x00, ftype_magic_t{ ft_macho, 0, 0, 0, 0 }, is_macho},
    ftype_checker_t{0x00, ftype_magic_t{ ft_macho, 0, 0, 0, 0 }, is_macho_fat},
    ftype_checker_t{0x00, ftype_magic_t{ ft_ms_pif, 0, 0, 0, 0 }, is_pif},
    ftype_checker_t{0x00, ftype_magic_t{ ft_ttf, 0, 0, 0, 0 }, is_ttf},
    ftype_checker_t{0x00, ftype_magic_t{ ft_uue, 0, 0, 0, 0 }, is_uue},
    ftype_checker_t{0x00, ftype_magic_t{ ft_wps, 0, 0, 0, 0 }, is_wps},
    ftype_checker_t{0x00, ftype_magic_t{ ft_sbr, 0, 0, MAGIC_SBR_A, sizeof(MAGIC_SBR_A) }, nullptr},
    ftype_checker_t{0x00, ftype_magic_t{ ft_sbr, 0, 0, MAGIC_SBR_B, sizeof(MAGIC_SBR_B) }, nullptr},
    ftype_checker_t{0x00, ftype_magic_t{ ft_ms_wma, 0, 4, MAGIC_WMA, sizeof(MAGIC_WMA) }, nullptr},
    ftype_checker_t{0x00, ftype_magic_t{ ft_mdb, 0, 4, MAGIC_MDB, sizeof(MAGIC_MDB) }, nullptr},
    ftype_checker_t{0x00, ftype_magic_t{ ft_nsis, 0, 4, MAGIC_NSIS, sizeof(MAGIC_NSIS) }, nullptr},
    ftype_checker_t{0x00, ftype_magic_t{ ft_sis, 0, 6, MAGIC_SIS, sizeof(MAGIC_SIS) }, nullptr},
    ftype_checker_t{0x00, ftype_magic_t{ ft_zoo, 0, 20, MAGIC_ZOO, sizeof(MAGIC_ZOO) }, nullptr},
    ftype_checker_t{0x00, ftype_magic_t{ ft_ms_activemine, 1, 0, MAGIC_MSO_ACTIVEMINE, sizeof(MAGIC_MSO_ACTIVEMINE) - 1 }, nullptr},
    ftype_checker_t{0xA3, ftype_magic_t{ ft_autoit, 0, 0, MAGIC_AutoIt, sizeof(MAGIC_AutoIt) }, nullptr},
    // text
    ftype_checker_t{0x00, ftype_magic_t{ ft_ini, 0, 0, 0, 0 }, is_ini},
    ftype_checker_t{0x00, ftype_magic_t{ ft_xml, 0, 0, 0, 0 }, is_xml },
    ftype_checker_t{0x00, ftype_magic_t{ ft_html, 0, 0, 0, 0 }, is_html },
    ftype_checker_t{0x00, ftype_magic_t{ ft_email, 0, 0, 0, 0 }, is_email},
    // TODO
    ftype_checker_t{0x00, ftype_magic_t{ ft_morphine, 0, 0, 0, 0 }, is_morphine},
    ftype_checker_t{0x00, ftype_magic_t{ ft_ms_lrh, 0, 0, 0, 0 }, is_ms_lrh},
    ftype_checker_t{0x00, ftype_magic_t{ ft_aiff, 0, 0, 0, 0 }, is_aiff },
    ftype_checker_t{0x00, ftype_magic_t{ ft_mpeg, 0, 0, 0, 0 }, is_mpeg },
};

int do_detect(const uint8_t* data, uint32_t size, ifile* file) {
    if (data == nullptr || 0 == size) {
        return ft_unknown;
    }

    int rst = ft_unknown;
    for (uint32_t i = 0, total = sizeof(ftype_checker) / sizeof(ftype_checker[0]); i < total; i++) {
        auto& checker = ftype_checker[i];
        if (!(*data == checker.chan || checker.chan == 0)) {
            continue;
        }

        if (checker.checker != nullptr) {
            rst = checker.checker(data, size, file);
        }
        else if (size >= checker.magic.offset + checker.magic.size) {
            auto& magic = checker.magic;
            if (magic.type == 0) {
                rst = 0 == memcmp(data + magic.offset, magic.magic, magic.size) ? magic.filetype : ft_unknown;
            }
            else {
                rst = 0 == ucs::nicmp((const char*)data + magic.offset, (const char*)magic.magic, magic.size) ? magic.filetype : ft_unknown;
            }
        }

        if (rst != ft_unknown) {
            break;
        }
    }

    return rst;
}

int common_ftype::detect_mem(const uint8_t* data, uint32_t size) {
    return do_detect(data, size, 0);
}

int common_ftype::detect(ifile* reader) {
    if (reader == nullptr) {
        return ft_unknown;
    }

    uint8_t* buffer = nullptr;
    uint32_t buffer_size = 2 * 1024 * 1024;
    ON_SCOPE_EXIT([&]() { if (buffer) { free(buffer); buffer = nullptr; } });

    uint32_t size = (uint32_t)reader->size();
    const uint8_t* data = reader->mapping();
    if (data == nullptr) {
        buffer_size = std::min<uint32_t>(buffer_size, size);
        buffer = (uint8_t*)malloc(buffer_size);
        if (nullptr == buffer) {
            return ft_unknown;
        }
        memset(buffer, 0, buffer_size);
        if (0 > reader->read(0, buffer, buffer_size, &buffer_size)) {
            return ft_unknown;
        }
        data = buffer;
        size = buffer_size;
    }

    return do_detect(data, size, reader);
}
