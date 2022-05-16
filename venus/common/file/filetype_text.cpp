#pragma once
#include <stdint.h>
#include <string.h>
#include "./ifile.h"
#include "./chartype.h"
#include "./filetype_define.h"
#include "../utils/utils.h"
#include "../utils/scope_guard.h"

typedef enum __EM_TEXT_ENCODING {
	em_encoding_unknown,
	em_encoding_ansi,
	em_encoding_utf8,
	em_encoding_utf16_le,
	em_encoding_utf16_be,
	em_encoding_utf32_le,
	em_encoding_utf32_be,
} EmTextEncoding;

/*
* @brief check the given text's encoding
* @param[in ] pText
* @param[in ] uBytes
* @return int, EmTextEncoding
*/
int check_text_encoding(const uint8_t* pText, uint32_t uBytes) {
	if (nullptr == pText || uBytes < 0x4) {
		return em_encoding_unknown;
	}

	const uint8_t utf8_bom[] = { 0xEF, 0xBB, 0xBF };
	const uint8_t utf16_bom_be[] = { 0xFE, 0xFF };
	const uint8_t utf16_bom_le[] = { 0xFF, 0xFE };
	const uint8_t utf32_bom_be[] = { 0x00, 0x00, 0xFE, 0xFF };
	const uint8_t utf32_bom_le[] = { 0xFF, 0xFE, 0x00, 0x00 };

	EmTextEncoding emRst = em_encoding_unknown;
	if (0 == memcmp(pText, utf32_bom_be, sizeof(utf32_bom_be))) {
		emRst = em_encoding_utf32_be;
	}
	else if (0 == memcmp(pText, utf32_bom_le, sizeof(utf32_bom_le))) {
		emRst = em_encoding_utf32_le;
	}
	else if (0 == memcmp(pText, utf8_bom, sizeof(utf8_bom))) {
		emRst = em_encoding_utf8;
	}
	else if (0 == memcmp(pText, utf16_bom_be, sizeof(utf16_bom_be))) {
		emRst = em_encoding_utf16_be;
	}
	else if (0 == memcmp(pText, utf16_bom_le, sizeof(utf16_bom_le))) {
		emRst = em_encoding_utf16_le;
	}
	else
	{
		auto try_ucs2_le = [](const uint8_t* pb, uint32_t size) {
			bool bRet = false;
			for (uint32_t i = 0, val = 0; i + 4 < size; i += 4) {
				val = XsUtils::byteorder2host<uint32_t>(*(const uint32_t*)(pb + i), false);
				if ((val & 0x00FF00FF) == 0x00000000) continue;
				bRet = false;
			}
			return bRet;
		};

		uint32_t uBOM = XsUtils::byteorder2host<uint32_t>(*(const uint32_t*)pText, false);
		if ((uBOM & 0x00FF00FF) == 0x00000000) {  // xx 00 xx 00, try UTF-16 little-endian without BOM
			emRst = em_encoding_utf16_le;
		}
		else if ((uBOM & 0xFF00FF00) == 0x00000000) {  // 00 xx 00 xx, try UTF-16 big-endian without BOM
			emRst = em_encoding_utf16_be;
		}
		else {
			// maybe utf-8 without BOM, or others
		}
	}

	return emRst;
}

/*
* @brief get all ascii characters from the utf8 text
*		, and replace every remaining character with a special ascii
* @param[in ] pText
* @param[in ] uBytes
* @return int, return 0 if success, otherwise fail
*/
int utf8_to_alpha(uint8_t* pBuf, uint32_t uSize, char cSpecial, uint8_t** out, uint32_t& out_size) {
	if (nullptr == pBuf || uSize == 0 || (out && 0 == out_size)) {
		return -1;
	}

	int nRet = 0;
	uint32_t uTmpSize = 0;
	uint8_t* pTmp = (uint8_t*)malloc(uSize);
	if (nullptr == pTmp) {
		return -2;
	}
	ON_SCOPE_EXIT([&]() { if (pTmp) free(pTmp); });

	for (uint32_t i = 0, uTmpSize = 0, uSkip = 0; i < uSize; i++, uTmpSize++) {
		uint8_t c = *(pBuf + i);
		if (c < 0x80) {
			*(pTmp + uTmpSize) = c;
			continue;
		}

		uSkip = 0;
		if ((c & 0xE0) == 0xC0) { // 110xxxxx 10xxxxxx
			if (i + 1 >= uSize) {
				i += 1;
				continue;
			}

			if ((*(pBuf + i + 1) & 0xC0) == 0x80) {
				uSkip += 1;
			}
			else {
				// something error
			}
		}
		else if ((c & 0xF0) == 0xE0) {   // 1110xxxx 10xxxxxx 10xxxxxx
			if (i + 2 >= uSize) {
				i += 2;
				continue;
			}

			auto uBom = XsUtils::byteorder2host<uint16_t>(*(uint16_t*)(pBuf + i), false);
			if ((uBom & 0xC0C0) == 0x8080) {
				uSkip += 2;
			}
			else {
				// something error
			}
		}
		else if ((c & 0xF8) == 0xF0) {   // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
			if (i + 3 >= uSize) {
				i += 3;
				continue;
			}

			auto uBom = XsUtils::byteorder2host<uint32_t>(*(uint32_t*)(pBuf + i), false);
			if ((uBom & 0xC0C0C000) == 0x80808000) {   // !!!不比较最后一个字节
				uSkip += 3;
			}
			else {
				// something error
			}
		}

		if (uSkip) {
			*(pTmp + uTmpSize) = cSpecial;
		}
		else {
			nRet = -3;
			break;
		}
	}

	if (nRet != 0) {
		return nRet;
	}

	if (nullptr == out) {
		out_size = uTmpSize;
	}
	else {
		out_size = std::min(uTmpSize, out_size);
		memcpy(pBuf, pTmp, out_size);
	}
	return nRet;
}

bool is_text(const uint8_t* data, uint32_t size, ifile* file = nullptr) {
	uint32_t nTotalCharCount = 0, nNotPrintCharCount = 0;
	for (uint32_t i = 0; i < size; i++) {
		if (!IS_PRINT_SPACE_EOL(*(data + i))) nNotPrintCharCount++;
		nTotalCharCount++;
	}

	if (nNotPrintCharCount == 0 || nTotalCharCount / nNotPrintCharCount > 6) {
		return true;
	}

	// modified for some script virus file.
	uint32_t uRandomStartPos = 32;
	if (size < uRandomStartPos) {
		return false;
	}
	
	nNotPrintCharCount = 0, nTotalCharCount = 0;
	for (uint32_t i = uRandomStartPos; i < size; i++) {
		if (!IS_PRINT_SPACE_EOL(*(data + i))) nNotPrintCharCount++;
		nTotalCharCount++;
	}

	if (nNotPrintCharCount == 0 || nTotalCharCount / nNotPrintCharCount > 6) {
		return true;
	}

	return false;
}

int detect_text_type(const uint8_t* data, uint32_t size) {

	is_text(data, size);
	check_text_encoding(data, size);
	// TODO:
	return -1;
}