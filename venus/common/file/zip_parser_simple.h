#pragma once
#include <vector>
#include <string>
#include "ifile.h"

#pragma pack(push, 1)

typedef struct tagZIPDIRENTRY
{
	/* 0x0000 */ uint32_t Magic;          // central file header signature (0x02014b50)
	/* 0x0004 */ uint16_t MakeVersion;    // version made by
	/* 0x0006 */ uint16_t NeedVersion;    // version needed to extract
	/* 0x0008 */ uint16_t Flag;           // general purpose bit flag
	/* 0x000A */ uint16_t Method;         // compression method
	/* 0x000C */ uint16_t Time;           // last mod file time
	/* 0x000E */ uint16_t Date;           // last mod file date
	/* 0x0010 */ uint32_t CRC;            // CRC-32
	/* 0x0014 */ uint32_t CompressSize;   // compressed size
	/* 0x0018 */ uint32_t OriginSize;     // uncompressed size
	/* 0x001C */ uint16_t NameSize;       // file name length
	/* 0x001E */ uint16_t ExtraSize;      // extra field length
	/* 0x0020 */ uint16_t CommentSize;    // file comment length
	/* 0x0022 */ uint16_t DiskStart;      // disk number start
	/* 0x0024 */ uint16_t InterAttr;      // internal file attributes
	/* 0x0026 */ uint32_t ExterAttr;      // external file attributes
	/* 0x002A */ uint32_t Offset;         // relative offset of local header
} ZIPDIRENTRY;

typedef struct tagZIPENDLOCATOR
{
	/* 0x0000 */ uint32_t Magic;          // end of central dir signature (0x06054b50)
	/* 0x0004 */ uint16_t CurrentDN;      // number of this disk
	/* 0x0006 */ uint16_t CDStartDN;      // number of the disk with the start of the central directory
	/* 0x0008 */ uint16_t CurrentCD;      // total number of entries in the central directory on this disk
	/* 0x000A */ uint16_t TotalCD;        // total number of entries in the central directory
	/* 0x000C */ uint32_t CDSize;         // size of the central directory
	/* 0x0010 */ uint32_t CDOffset;       // offset of start of central directory with respect to the starting disk number
	/* 0x0014 */ uint16_t CommentSize;    // .ZIP file comment length
} ZIPENDLOCATOR;

#pragma pack(pop)

#define ZIP_FLAG_ENCRYPTED  0x0001 // file is encrypted

#define ZIP_MAGIC_CFH		0x02014b50 // central file header signature
#define ZIP_MAGIC_ECD		0x06054b50 // end of central dir signature


typedef struct __ST_ZIP_ITEM_NAME_POS {
	uint32_t uOffset;
	uint16_t uBytes;
} StZipItemNamePos;

int parse_zip_items(ifile* file, std::vector<std::string>& items) {
	if (nullptr == file) { // 整个文件中三个特定目录项所占空间总长度(包括ecd)
		return -1;
	}

	int nRet = 0;
	uint32_t nReaded = 0, uSkipSize = 0;
	std::vector<StZipItemNamePos> vecNamesPos;

	do {
		ZIPENDLOCATOR ecd = { 0 };
		ZIPDIRENTRY direntry;

		uint64_t uFileSize = file->size();
		if (uFileSize < 300) {
			nRet = -2;
			break;
		}
		
		// 读入end of central dir
		uFileSize -= 22;  // end of central dir位置(不考虑存在附加信息的情况)
		nRet = file->read(uFileSize, (uint8_t*)&ecd, sizeof(ecd), &nReaded);
		if (nRet < 0) {
			break;
		}
		if (nReaded != sizeof(ZIPENDLOCATOR)) {
			nRet = -3;
			break;
		}

		// 检查ecd : 此外应满足：ecd.CDSize > (ecd.CurrentCD * ZIPDIRENTRY + 20 + 19 + 11)
		if (ecd.Magic != ZIP_MAGIC_ECD || ecd.CommentSize != 0 // 存在附加信息
			|| ecd.CDOffset >= uFileSize // central directory偏移越界
			|| ecd.CDSize + ecd.CDOffset > uFileSize // central directory总长度越界
			) { 
			nRet = -4;
			break;
		}

		// 特别处理某种压缩软件生成的Zip包: central directory的结束位置, 应该就是end of central dir的开始位置(不考虑Zip64的情况)
		uSkipSize = ecd.CDSize + ecd.CDOffset;  // 借用uSkipSize
		if (uSkipSize < uFileSize) {  // CD的结束位置在ECD的开始位置之前, 说明Zip包的真实开始位置不是从文件头开始
			ecd.CDOffset += uFileSize - uSkipSize;
		}

		// 处理 central directory
		for (uint32_t i = 0, offset = ecd.CDOffset; i < ecd.CurrentCD; i++) {
			// 读入central file header
			nRet = file->read(offset, (uint8_t*)&direntry, sizeof(direntry), &nReaded);
			if (nRet < 0) {
				break;
			}

			if (nReaded != sizeof(ZIPDIRENTRY)) {
				nRet = -5;
				break;
			}
			offset += sizeof(direntry);

			// 文件不能加密
			if ((direntry.Flag & ZIP_FLAG_ENCRYPTED) == ZIP_FLAG_ENCRYPTED)
			{
				nRet = -6;
				break;
			}

			vecNamesPos.push_back(StZipItemNamePos{ offset, direntry.NameSize });
			offset += direntry.NameSize;
			offset += direntry.ExtraSize + direntry.CommentSize;
		}
	} while (false);

	if (nRet < 0) {
		return nRet;
	}

	uint8_t uFileName[0x100] = { 0 };
	for (uint32_t i = 0; i < vecNamesPos.size(); i++) {
		memset(uFileName, 0, sizeof(uFileName));

		nRet = file->read(vecNamesPos[i].uOffset, (uint8_t*)uFileName, vecNamesPos[i].uBytes, &nReaded);
		if (nRet < 0) {
			break;
		}

		items.push_back(std::string((char*)uFileName));
	}

    return 0;
}
