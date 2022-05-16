#pragma once
#include <stdint.h>
#include <stdlib.h>

/*
* 
* @Reference 
*       : https://docs.microsoft.com/en-gb/windows/desktop/Intl/code-page-identifiers
* 
*/

#if (_WIN32 || _WIN64) 
#include <windows.h>
#else
#include <iconv.h>
#define HAVE_ICONV_H 1
#endif

/* string conversion */
struct codepage_entry {
    uint16_t codepage;
    const char* encoding;
};

#define NUMCODEPAGES (sizeof(codepage_entries) / sizeof(struct codepage_entry))
/* 
* MAINTAIN - the array in codepage value sorted order 
* 
* MAC CodePage : http://stackoverflow.com/questions/1592925/decoding-mac-os-text-in-python
*/
static const struct codepage_entry codepage_entries[] = {
    {37, "IBM037"},           /* IBM EBCDIC US-Canada */
    {437, "IBM437"},          /* OEM United States */
    {500, "IBM500"},          /* IBM EBCDIC International */
    {708, "ASMO-708"},        /* Arabic (ASMO 708) */
    {709, NULL},              /* Arabic (ASMO-449+, BCON V4) */
    {710, NULL},              /* Arabic - Transparent Arabic */
    {720, NULL},              /* Arabic (Transparent ASMO); Arabic (DOS) */
    {737, NULL},              /* OEM Greek (formerly 437G); Greek (DOS) */
    {775, "IBM775"},          /* OEM Baltic; Baltic (DOS) */
    {850, "IBM850"},          /* OEM Multilingual Latin 1; Western European (DOS) */
    {852, "IBM852"},          /* OEM Latin 2; Central European (DOS) */
    {855, "IBM855"},          /* OEM Cyrillic (primarily Russian) */
    {857, "IBM857"},          /* OEM Turkish; Turkish (DOS) */
    {858, NULL},              /* OEM Multilingual Latin 1 + Euro symbol */
    {860, "IBM860"},          /* OEM Portuguese; Portuguese (DOS) */
    {861, "IBM861"},          /* OEM Icelandic; Icelandic (DOS) */
    {862, NULL},              /* OEM Hebrew; Hebrew (DOS) */
    {863, "IBM863"},          /* OEM French Canadian; French Canadian (DOS) */
    {864, "IBM864"},          /* OEM Arabic; Arabic (864) */
    {865, "IBM865"},          /* OEM Nordic; Nordic (DOS) */
    {866, "CP866"},           /* OEM Russian; Cyrillic (DOS) */
    {869, "IBM869"},          /* OEM Modern Greek; Greek, Modern (DOS) */
    {870, "IBM870"},          /* IBM EBCDIC Multilingual/ROECE (Latin 2); IBM EBCDIC Multilingual Latin 2 */
    {874, "WINDOWS-874"},     /* ANSI/OEM Thai (ISO 8859-11); Thai (Windows) */
    {875, "CP875"},           /* IBM EBCDIC Greek Modern */
    {932, "SHIFT_JIS"},       /* ANSI/OEM Japanese; Japanese (Shift-JIS) */
    {936, "GB2312"},          /* ANSI/OEM Simplified Chinese (PRC, Singapore); Chinese Simplified (GB2312) */
    {949, "CP949"},           /* ANSI/OEM Korean (Unified Hangul Code) */
    {950, "BIG5"},            /* ANSI/OEM Traditional Chinese (Taiwan; Hong Kong SAR, PRC); Chinese Traditional (Big5) */
    {1026, "IBM1026"},        /* IBM EBCDIC Turkish (Latin 5) */
    {1047, NULL},             /* IBM EBCDIC Latin 1/Open System */
    {1140, NULL},             /* IBM EBCDIC US-Canada (037 + Euro symbol); IBM EBCDIC (US-Canada-Euro) */
    {1141, NULL},             /* IBM EBCDIC Germany (20273 + Euro symbol); IBM EBCDIC (Germany-Euro) */
    {1142, NULL},             /* IBM EBCDIC Denmark-Norway (20277 + Euro symbol); IBM EBCDIC (Denmark-Norway-Euro) */
    {1143, NULL},             /* IBM EBCDIC Finland-Sweden (20278 + Euro symbol); IBM EBCDIC (Finland-Sweden-Euro) */
    {1144, NULL},             /* IBM EBCDIC Italy (20280 + Euro symbol); IBM EBCDIC (Italy-Euro) */
    {1145, NULL},             /* IBM EBCDIC Latin America-Spain (20284 + Euro symbol); IBM EBCDIC (Spain-Euro) */
    {1146, NULL},             /* IBM EBCDIC United Kingdom (20285 + Euro symbol); IBM EBCDIC (UK-Euro) */
    {1147, NULL},             /* IBM EBCDIC France (20297 + Euro symbol); IBM EBCDIC (France-Euro) */
    {1148, NULL},             /* IBM EBCDIC International (500 + Euro symbol); IBM EBCDIC (International-Euro) */
    {1149, NULL},             /* IBM EBCDIC Icelandic (20871 + Euro symbol); IBM EBCDIC (Icelandic-Euro) */
    {1200, "UTF-16LE"},       /* Unicode UTF-16, little endian byte order (BMP of ISO 10646); available only to managed applications */
    {1201, "UTF-16BE"},       /* Unicode UTF-16, big endian byte order; available only to managed applications */
    {1250, "WINDOWS-1250"},   /* ANSI Central European; Central European (Windows) */
    {1251, "WINDOWS-1251"},   /* ANSI Cyrillic; Cyrillic (Windows) */
    {1252, "WINDOWS-1252"},   /* ANSI Latin 1; Western European (Windows) */
    {1253, "WINDOWS-1253"},   /* ANSI Greek; Greek (Windows) */
    {1254, "WINDOWS-1254"},   /* ANSI Turkish; Turkish (Windows) */
    {1255, "WINDOWS-1255"},   /* ANSI Hebrew; Hebrew (Windows) */
    {1256, "WINDOWS-1256"},   /* ANSI Arabic; Arabic (Windows) */
    {1257, "WINDOWS-1257"},   /* ANSI Baltic; Baltic (Windows) */
    {1258, "WINDOWS-1258"},   /* ANSI/OEM Vietnamese; Vietnamese (Windows) */
    {1361, "JOHAB"},          /* Korean (Johab) */
    {10000, "MACINTOSH"},     /* MAC Roman; Western European (Mac) */
    {10001, NULL},            /* Japanese (Mac) */
    {10002, NULL},            /* MAC Traditional Chinese (Big5); Chinese Traditional (Mac) */
    {10003, NULL},            /* Korean (Mac) */
    {10004, NULL},            /* Arabic (Mac) */
    {10005, NULL},            /* Hebrew (Mac) */
    {10006, NULL},            /* Greek (Mac) */
    {10007, NULL},            /* Cyrillic (Mac) */
    {10008, NULL},            /* MAC Simplified Chinese (GB 2312); Chinese Simplified (Mac) */
    {10010, NULL},            /* Romanian (Mac) */
    {10017, NULL},            /* Ukrainian (Mac) */
    {10021, NULL},            /* Thai (Mac) */
    {10029, NULL},            /* MAC Latin 2; Central European (Mac) */
    {10079, NULL},            /* Icelandic (Mac) */
    {10081, NULL},            /* Turkish (Mac) */
    {10082, NULL},            /* Croatian (Mac) */
    {12000, "UTF-32LE"},      /* Unicode UTF-32, little endian byte order; available only to managed applications */
    {12001, "UTF-32BE"},      /* Unicode UTF-32, big endian byte order; available only to managed applications */
    {20000, NULL},            /* CNS Taiwan; Chinese Traditional (CNS) */
    {20001, NULL},            /* TCA Taiwan */
    {20002, NULL},            /* Eten Taiwan; Chinese Traditional (Eten) */
    {20003, NULL},            /* IBM5550 Taiwan */
    {20004, NULL},            /* TeleText Taiwan */
    {20005, NULL},            /* Wang Taiwan */
    {20105, NULL},            /* IA5 (IRV International Alphabet No. 5, 7-bit); Western European (IA5) */
    {20106, NULL},            /* IA5 German (7-bit) */
    {20107, NULL},            /* IA5 Swedish (7-bit) */
    {20108, NULL},            /* IA5 Norwegian (7-bit) */
    {20127, "US-ASCII"},      /* US-ASCII (7-bit) */
    {20261, NULL},            /* T.61 */
    {20269, NULL},            /* ISO 6937 Non-Spacing Accent */
    {20273, "IBM273"},        /* IBM EBCDIC Germany */
    {20277, "IBM277"},        /* IBM EBCDIC Denmark-Norway */
    {20278, "IBM278"},        /* IBM EBCDIC Finland-Sweden */
    {20280, "IBM280"},        /* IBM EBCDIC Italy */
    {20284, "IBM284"},        /* IBM EBCDIC Latin America-Spain */
    {20285, "IBM285"},        /* IBM EBCDIC United Kingdom */
    {20290, "IBM290"},        /* IBM EBCDIC Japanese Katakana Extended */
    {20297, "IBM297"},        /* IBM EBCDIC France */
    {20420, "IBM420"},        /* IBM EBCDIC Arabic */
    {20423, "IBM423"},        /* IBM EBCDIC Greek */
    {20424, "IBM424"},        /* IBM EBCDIC Hebrew */
    {20833, NULL},            /* IBM EBCDIC Korean Extended */
    {20838, NULL},            /* IBM EBCDIC Thai */
    {20866, "KOI8-R"},        /* Russian (KOI8-R); Cyrillic (KOI8-R) */
    {20871, "IBM871"},        /* IBM EBCDIC Icelandic */
    {20880, "IBM880"},        /* IBM EBCDIC Cyrillic Russian */
    {20905, "IBM905"},        /* IBM EBCDIC Turkish */
    {20924, NULL},            /* IBM EBCDIC Latin 1/Open System (1047 + Euro symbol) */
    {20932, "EUC-JP"},        /* Japanese (JIS 0208-1990 and 0212-1990) */
    {20936, NULL},            /* Simplified Chinese (GB2312); Chinese Simplified (GB2312-80) */
    {20949, NULL},            /* Korean Wansung */
    {21025, "CP1025"},        /* IBM EBCDIC Cyrillic Serbian-Bulgarian */
    {21027, NULL},            /* (deprecated) */
    {21866, "KOI8-U"},        /* Ukrainian (KOI8-U); Cyrillic (KOI8-U) */
    {28591, "ISO-8859-1"},    /* ISO 8859-1 Latin 1; Western European (ISO) */
    {28592, "ISO-8859-2"},    /* ISO 8859-2 Central European; Central European (ISO) */
    {28593, "ISO-8859-3"},    /* ISO 8859-3 Latin 3 */
    {28594, "ISO-8859-4"},    /* ISO 8859-4 Baltic */
    {28595, "ISO-8859-5"},    /* ISO 8859-5 Cyrillic */
    {28596, "ISO-8859-6"},    /* ISO 8859-6 Arabic */
    {28597, "ISO-8859-7"},    /* ISO 8859-7 Greek */
    {28598, "ISO-8859-8"},    /* ISO 8859-8 Hebrew; Hebrew (ISO-Visual) */
    {28599, "ISO-8859-9"},    /* ISO 8859-9 Turkish */
    {28603, "ISO-8859-13"},   /* ISO 8859-13 Estonian */
    {28605, "ISO-8859-15"},   /* ISO 8859-15 Latin 9 */
    {29001, NULL},            /* Europa 3 */
    {38598, NULL},            /* ISO 8859-8 Hebrew; Hebrew (ISO-Logical) */
    {50220, "ISO-2022-JP"},   /* ISO 2022 Japanese with no halfwidth Katakana; Japanese (JIS) (guess) */
    {50221, "ISO-2022-JP-2"}, /* ISO 2022 Japanese with halfwidth Katakana; Japanese (JIS-Allow 1 byte Kana) (guess) */
    {50222, "ISO-2022-JP-3"}, /* ISO 2022 Japanese JIS X 0201-1989; Japanese (JIS-Allow 1 byte Kana - SO/SI) (guess) */
    {50225, "ISO-2022-KR"},   /* ISO 2022 Korean */
    {50227, NULL},            /* ISO 2022 Simplified Chinese; Chinese Simplified (ISO 2022) */
    {50229, NULL},            /* ISO 2022 Traditional Chinese */
    {50930, NULL},            /* EBCDIC Japanese (Katakana) Extended */
    {50931, NULL},            /* EBCDIC US-Canada and Japanese */
    {50933, NULL},            /* EBCDIC Korean Extended and Korean */
    {50935, NULL},            /* EBCDIC Simplified Chinese Extended and Simplified Chinese */
    {50936, NULL},            /* EBCDIC Simplified Chinese */
    {50937, NULL},            /* EBCDIC US-Canada and Traditional Chinese */
    {50939, NULL},            /* EBCDIC Japanese (Latin) Extended and Japanese */
    {51932, "EUC-JP"},        /* EUC Japanese */
    {51936, "EUC-CN"},        /* EUC Simplified Chinese; Chinese Simplified (EUC) */
    {51949, "EUC-KR"},        /* EUC Korean */
    {51950, NULL},            /* EUC Traditional Chinese */
    {52936, NULL},            /* HZ-GB2312 Simplified Chinese; Chinese Simplified (HZ) */
    {54936, "GB18030"},       /* Windows XP and later: GB18030 Simplified Chinese (4 byte); Chinese Simplified (GB18030) */
    {57002, NULL},            /* ISCII Devanagari */
    {57003, NULL},            /* ISCII Bengali */
    {57004, NULL},            /* ISCII Tamil */
    {57005, NULL},            /* ISCII Telugu */
    {57006, NULL},            /* ISCII Assamese */
    {57007, NULL},            /* ISCII Oriya */
    {57008, NULL},            /* ISCII Kannada */
    {57009, NULL},            /* ISCII Malayalam */
    {57010, NULL},            /* ISCII Gujarati */
    {57011, NULL},            /* ISCII Punjabi */
    {65000, "UTF-7"},         /* Unicode (UTF-7) */
    {65001, "UTF-8"}          /* Unicode (UTF-8) */
};

typedef enum em_codepage {
    cp_japanese_shift_jis = 932,
    cp_korean_unified = 949,
    cp_utf16_le = 1200,
    cp_utf16_be = 1201,
    cp_latin1 = 1252,			/* ANSI Latin 1; Western European(Windows) */
    cp_us_7bit_ascii = 20127,	/* US-ASCII (7-bit)  */
    cp_us_iso8859_1 = 28591,	/* ISO 8859-1 Latin 1; Western European (ISO) */
    cp_us_utf8 = 65001,	/* UTF-8 */
} codepage_t;

class XsCodepage {
public:
    /*
    * @desc   reference to clamav
    */
    static int cp2utf8(const uint8_t* in, size_t in_size, uint16_t codepage, uint8_t** out, uint32_t* out_size) {
        if (nullptr == in || in_size == 0 || nullptr == out || nullptr == out_size) {
            return -1;
        }

        int nret = 0;
        uint8_t* out_utf8 = nullptr;
        uint32_t out_utf8_size = 0;

        if (codepage == cp_us_7bit_ascii || codepage == cp_us_utf8) {
            out_utf8 = (uint8_t*)malloc(in_size + 1);
            if (out_utf8 == nullptr) {
                return -2;
            }
            memcpy(out_utf8, in, in_size);

            uint8_t* track = out_utf8 + in_size - 1;
            if ((codepage == cp_us_utf8) && (*track & 0x80)) {
                /*
                 * UTF-8 with a most significant bit.
                 */

                 /* locate the start of the last character */
                int byte_count, sigbit_count;
                for (byte_count = 1; (track != out_utf8); track--, byte_count++) {
                    if (((uint8_t)*track & 0xC0) != 0x80)
                        break;
                }

                /* count number of set (1) significant bits */
                for (sigbit_count = 0; sigbit_count < (int)(sizeof(uint8_t) * 8); sigbit_count++) {
                    if (((uint8_t)*track & (0x80 >> sigbit_count)) == 0)
                        break;
                }

                if (byte_count != sigbit_count) {
                    // cli_dbgmsg("cli_codepage_to_utf8: cleaning out %d bytes from incomplete utf-8 character length %d\n", byte_count, sigbit_count);
                    for (; byte_count > 0; byte_count--, track++) {
                        *track = '\0';
                    }
                }
            }

            *out = out_utf8;
            *out_size = in_size;
            return nret;
        }

#if defined(WIN32) && !defined(HAVE_ICONV)
        uint8_t* utf16_le = nullptr;
        uint32_t utf16_le_size = 0;

        /* First, Convert from codepage -> UCS-2 LE with MultiByteToWideChar(codepage) */
        for (int i = 0; i < 1 && (cp_utf16_le != codepage); i++) {
            if (cp_utf16_be == codepage) {
                utf16_le_size = in_size / 2 * sizeof(wchar_t);
                /*
                 * MultiByteToWideChar() does not support conversions from UTF-16BE to UTF-16LE.
                 * However, conversion is simply a matter of swapping the bytes for each WCHAR_T.
                 * See: https://stackoverflow.com/questions/29054217/multibytetowidechar-for-unicode-code-pages-1200-1201-12000-12001
                 */
                utf16_le = (uint8_t*)malloc(utf16_le_size);
                if (utf16_le == nullptr) {
                    nret = -3;
                    break;
                }
                memcpy_s(utf16_le, utf16_le_size, in, utf16_le_size);

                wchar_t* dst = (wchar_t*)utf16_le;
                wchar_t* src = (wchar_t*)in;
                for (size_t i = 0; i < utf16_le_size / 2; i++) {
                    dst[i] = (wchar_t)(((src[i] << 8) & 0xFF00) | ((src[i] >> 8) & 0x00FF));
                }

                /* in_size didn't change */
            }
            else {
                int cchWideChar = MultiByteToWideChar(codepage, 0, (const char*)in, (int)in_size, NULL, 0);
                if (0 == cchWideChar) {
                    nret = -4;
                    break;
                }

                /* No need for a null terminator here, we'll deal with the exact size */
                utf16_le = (uint8_t*)malloc(cchWideChar * sizeof(wchar_t));
                if (utf16_le == nullptr) {
                    nret = -5;
                    break;
                }

                cchWideChar = MultiByteToWideChar(codepage, 0, (const char*)in, (int)in_size, (wchar_t*)utf16_le, cchWideChar);
                if (0 == cchWideChar) {
                    nret = -6;
                    break;
                }

                utf16_le_size = cchWideChar * sizeof(WCHAR);
            }

            in = utf16_le;
            in_size = utf16_le_size;
            break;
        }

        for (int i = 0; i < 1 && nret == 0; i++) {
            /*
            * Convert from UCS-2 LE -> UTF8 with WideCharToMultiByte(CP_UTF8)
            */
            out_utf8_size = WideCharToMultiByte(CP_UTF8, 0, (LPCWCH)in, (int)in_size / sizeof(WCHAR), NULL, 0, NULL, NULL);
            if (0 == out_utf8_size) {
                nret = -7;
                break;
            }

            out_utf8 = (uint8_t*)malloc(out_utf8_size + 1); /* Add a null terminator to this string */
            if (NULL == out_utf8) {
                nret = -8;
                break;
            }

            out_utf8_size = WideCharToMultiByte(CP_UTF8, 0, (LPCWCH)in, (int)in_size / sizeof(WCHAR), (char*)out_utf8, (int)out_utf8_size, NULL, NULL);
            if (0 == out_utf8_size) {
                nret = -9;
                break;
            }
            /* Set a null byte, since null-terminator is not provided when in_size is provided */
            out_utf8[out_utf8_size] = '\0';
        }

        if (cp_utf16_le != codepage) {
            if (utf16_le) {
                free(utf16_le);
                utf16_le = nullptr;
            }
            utf16_le_size = 0;
        }

#elif defined(HAVE_ICONV)
        uint32_t attempt, i;
        const char* encoding = NULL;

        for (i = 0; i < NUMCODEPAGES; ++i) {
            if (codepage == codepage_entries[i].codepage) {
                encoding = codepage_entries[i].encoding;
                break;
            }
            else if (codepage < codepage_entries[i].codepage) {
                break; /* fail-out early, requires sorted array */
            }
        }

        if (NULL == encoding) {
            cli_dbgmsg("cli_codepage_to_utf8: Invalid codepage parameter passed in.\n");
            goto done;
        }

        for (attempt = 1; attempt <= 3; attempt++) {
            char* inbuf = in;
            size_t inbufsize = in_size;
            size_t iconvRet = -1;
            size_t outbytesleft = 0;

            char* out_utf8_tmp = NULL;
            char* out_utf8_index = NULL;

            /* Charset to UTF-8 should never exceed in_size * 6;
             * We can shrink final buffer after the conversion, if needed. */
            out_utf8_size = (in_size * 2) * attempt;

            outbytesleft = out_utf8_size;

            out_utf8 = cli_calloc(1, out_utf8_size + 1);
            if (NULL == out_utf8) {
                cli_errmsg("cli_codepage_to_utf8: Failure allocating buffer for utf8 data.\n");
                status = CL_EMEM;
                goto done;
            }
            out_utf8_index = out_utf8;

            conv = iconv_open("UTF-8//TRANSLIT", encoding);
            if (conv == (iconv_t)-1) {
                // Try again w/out the //TRANSLIT, required because musl doesn't supprot it.
                // See: https://github.com/akrennmair/newsbeuter/issues/364#issuecomment-250208235
                conv = iconv_open("UTF-8", encoding);
                if (conv == (iconv_t)-1) {
                    cli_warnmsg("cli_codepage_to_utf8: Failed to open iconv.\n");
                    goto done;
                }
            }

            iconvRet = iconv(conv, &inbuf, &inbufsize, &out_utf8_index, &outbytesleft);
            iconv_close(conv);
            conv = (iconv_t)-1;
            if ((size_t)-1 == iconvRet) {
                switch (errno) {
                case E2BIG:
                    cli_warnmsg("cli_codepage_to_utf8: iconv error: There is not sufficient room at *outbuf.\n");
                    free(out_utf8);
                    out_utf8 = NULL;
                    continue; /* Try again, with a larger buffer. */
                case EILSEQ:
                    cli_warnmsg("cli_codepage_to_utf8: iconv error: An invalid multibyte sequence has been encountered in the input.\n");
                    break;
                case EINVAL:
                    cli_dbgmsg("cli_codepage_to_utf8: iconv error: An incomplete multibyte sequence has been encountered in the input.\n");
                    break;
                default:
                    cli_warnmsg("cli_codepage_to_utf8: iconv error: Unexpected error code %d.\n", errno);
                }
                status = CL_EPARSE;
                goto done;
            }

            /* iconv succeeded, but probably didn't use the whole buffer. Free up the extra memory. */
            out_utf8_tmp = cli_realloc(out_utf8, out_utf8_size - outbytesleft + 1);
            if (NULL == out_utf8_tmp) {
                cli_errmsg("cli_codepage_to_utf8: failure cli_realloc'ing converted filename.\n");
                status = CL_EMEM;
                goto done;
            }
            out_utf8 = out_utf8_tmp;
            out_utf8_size = out_utf8_size - outbytesleft;
            break;
        }
#else
        /*
         * No way to do the conversion.
         */
        nret = -1;
#endif
        if (nret == 0) {
                *out = out_utf8;
                *out_size = out_utf8_size;
        }
            else {
                if (out_utf8) {
                    free(out_utf8);
                    out_utf8 = nullptr;
                }
                out_utf8_size = 0;
            }
            return nret;
    }
};