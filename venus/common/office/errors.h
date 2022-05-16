#pragma once

namespace BQ {
    const int OK = 0;
    const int ERR = 0x80010001;
    const int INVALID_ARG = 0x80010002;
    const int NOT_IMPLEMENT = 0x80010003;
    const int ERR_READ = 0x80010004;
    const int ERR_WRITE = 0x80010005;
    const int ERR_FILE_FORMAT = 0x80010006;
    const int ERR_MALLOC = 0x80010007;
    const int ERR_FORMAT = 0x80010008;
    const int ERR_PASSWD = 0x80010009;
    const int ERR_ENCRYPT = 0x8001000A;
    const int ERR_DECRYPT = 0x8001000B;
    const int ERR_ENCRYPTED = 0x8001000C;
    const int ERR_CRYPTION_UNKNOWN = 0x8001000D;
    const int ERR_FILE_CREATOR = 0x8001000E;
    const int ERR_PARSE_XML = 0x8001000F;
    const int ERR_UNCOMPRESS = 0x80010010;
    const int ERR_USER_INTERRUPT = 0x80010011;
    const int ERR_OPTION_DENY = 0x80010012;
    const int ERR_MAGIC = 0x80010013;

    const int ERR_OLE_HEADER = 0x80011001;
    const int ERR_OLE_SID = 0x80011002;
    const int ERR_OLE_DIRECTORY_ENTRY_ID = 0x80011003;
    const int ERR_OLE_DIR_PROP_FROMAT = 0x80011004;
    const int ERR_BIFF_LEN = 0x80011005;
    const int ERR_VBA_COMPRESSED_CONTAINER = 0x80011006;
    const int ERR_VBA_COMPRESSED_CHUNK_SIGN = 0x80011007;
    const int ERR_VBA_DECRYPT_CHUNK = 0x8001100E;
    const int ERR_OFFICE_CRYPT_FORMAT = 0x80011008;
    const int ERR_OFFICE_CRYPT_DS_INFO = 0x80011009;
    const int ERR_OFFICE_CRYPT_DS_MAP = 0x8001100A;
    const int ERR_OFFICE_UNICODE_LP_P4 = 0x8001100B;
    const int ERR_OFFICE_CRYPT_INFO_FMT = 0x8001100C;
    const int ERR_OFFICE_CRYPT_INFO = 0x8001100D;
    const int ERR_FORMAT_XLM = 0x8001100E;
    const int ERR_OOXML_VBAPROJECT_ID = 0x8001100F;
    const int ERR_XL_CELL_POS = 0x80011010;
    const int ERR_XL_CELL_VAL = 0x80011011;
}
