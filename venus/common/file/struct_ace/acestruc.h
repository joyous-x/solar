#ifndef _BASE_ACESTRUC_H_INCLUDED
#define _BASE_ACESTRUC_H_INCLUDED

#include "./lfn/lfn.h"

#ifdef INCL_BASE_ACESTRUC_EXCLUSIVE
  #define DECLEXT
#else
  #define DECLEXT extern
#endif


#define BASE_ACESTRUC_EXTRACTVERSION 20

#define BASE_ACESTRUC_HOST_MSDOS  0
#define BASE_ACESTRUC_HOST_OS2    1
#define BASE_ACESTRUC_HOST_WIN32  2
#define BASE_ACESTRUC_HOST_LINUX  12

#define BASE_ACESTRUC_ACESIGNLEN 7

#if defined(__OS2__)
  #define BASE_ACESTRUC_CURRENTHOST BASE_ACESTRUC_HOST_OS2
#else
  #if defined(__NT__)
    #define BASE_ACESTRUC_CURRENTHOST BASE_ACESTRUC_HOST_WIN32
  #else
    #define BASE_ACESTRUC_CURRENTHOST BASE_ACESTRUC_HOST_MSDOS
  #endif
#endif

#define BASE_ACESTRUC_FLAG_ADDSIZE           1       //all
#define BASE_ACESTRUC_FLAG_COMMENT           2       //main, file
#define BASE_ACESTRUC_FLAG_64BIT             4       //file, recovery
#define BASE_ACESTRUC_FLAG_V20FORMAT         256     //main
#define BASE_ACESTRUC_FLAG_SFX               512     //main
#define BASE_ACESTRUC_FLAG_LIMITSFXJR        1024    //main
#define BASE_ACESTRUC_FLAG_SECURITY          1024    //NTFS security
#define BASE_ACESTRUC_FLAG_MULTIVOLUME       2048    //main
#define BASE_ACESTRUC_FLAG_AV                4096    //main
#define BASE_ACESTRUC_FLAG_SPLITBEFORE       4096    //file
#define BASE_ACESTRUC_FLAG_RECOVERYREC       8192    //main
#define BASE_ACESTRUC_FLAG_SPLITAFTER        8192    //file
#define BASE_ACESTRUC_FLAG_LOCK              16384   //main
#define BASE_ACESTRUC_FLAG_PASSWORD          16384   //file
#define BASE_ACESTRUC_FLAG_SOLID             32768   //main, file
//
#define BASE_ACESTRUC_BLOCK_MAIN             0
#define BASE_ACESTRUC_BLOCK_FILE32           1
#define BASE_ACESTRUC_BLOCK_RECOVERY32       2
#define BASE_ACESTRUC_BLOCK_FILE             3
#define BASE_ACESTRUC_BLOCK_RECOVERY         4
#define BASE_ACESTRUC_BLOCK_RECOVERY2        5
//
#define BASE_ACESTRUC_TECHNIQUE_STORE        0
#define BASE_ACESTRUC_TECHNIQUE_LZ77_1       1
#define BASE_ACESTRUC_TECHNIQUE_BLOCKED_1    2
//
#define BASE_ACESTRUC_BLOCKEDTYPE_LZ77_NORM         0
#define BASE_ACESTRUC_BLOCKEDTYPE_LZ77_DELTA        1
#define BASE_ACESTRUC_BLOCKEDTYPE_LZ77_EXE          2
#define BASE_ACESTRUC_BLOCKEDTYPE_SOUND_8           3
#define BASE_ACESTRUC_BLOCKEDTYPE_SOUND_16          4
#define BASE_ACESTRUC_BLOCKEDTYPE_SOUND_32_1        5
#define BASE_ACESTRUC_BLOCKEDTYPE_SOUND_32_2        6
#define BASE_ACESTRUC_BLOCKEDTYPE_PIC               7


#define SETNOALIGNMENT
#include "packprag.h"

typedef struct
{
  uint8_t   TYPE;
  uint8_t   QUAL;
  uint16_t  PARM;
} tBASE_ACESTRUC_TECHSTRUC;

typedef struct
{
  uint16_t  HEAD_CRC;
  uint16_t  HEAD_SIZE;
  uint8_t   HEAD_TYPE;
  uint16_t  HEAD_FLAGS;
  uint32_t   ADDSIZE;
  uint8_t   other[32768];
} tBASE_ACESTRUC_BASICHEADER32,
 *pBASE_ACESTRUC_BASICHEADER32;

typedef struct
{
  uint16_t  HEAD_CRC;
  uint16_t  HEAD_SIZE;
  uint8_t   HEAD_TYPE;
  uint16_t  HEAD_FLAGS;
  uint64_t
          ADDSIZE;
  uint8_t   other[32768];
} tBASE_ACESTRUC_BASICHEADER,
 *pBASE_ACESTRUC_BASICHEADER;

typedef struct
{
  uint16_t  HEAD_CRC;
  uint16_t  HEAD_SIZE;
  uint8_t   HEAD_TYPE;
  uint16_t  HEAD_FLAGS;

  char    ACESIGN[BASE_ACESTRUC_ACESIGNLEN];
  uint8_t   VER_EXTR;
  uint8_t   VER_CR;
  uint8_t   HOST_CR;
  uint8_t   VOL_NUM;
  uint32_t   TIME_CR;
  uint16_t  RES1;       //undocumented:AV-CHECK:AV_CRC16^(!TIME_CR)^(TIME_CR>>16)
  uint16_t  RES2;
  uint32_t   RES;
  uint8_t   AV_SIZE;
  char    AV[256];
  uint16_t  COMM_SIZE;
  char    COMM[32768];
  char    RES3[17];   //undocumented:Archive-CRC, wenn AV; stuff wenn registr.
} tBASE_ACESTRUC_MAINHEADER,
 *pBASE_ACESTRUC_MAINHEADER;

typedef struct
{
  uint16_t  HEAD_CRC;
  uint16_t  HEAD_SIZE;
  uint8_t   HEAD_TYPE;
  uint16_t  HEAD_FLAGS;

  uint32_t   PSIZE;
  uint32_t   SIZE;
  uint32_t   FTIME;
  uint32_t   ATTR;
  uint32_t   CRC32;
  tBASE_ACESTRUC_TECHSTRUC
          TECH;
  uint16_t  RESERVED;
  uint16_t  FNAME_SIZE;
  tLFN    FNAME;
  uint16_t  COMM_SIZE;
  char    COMM[32768];
  uint16_t  SECURITY_SIZE;
  char    SECURITY[512];
} tBASE_ACESTRUC_FILEHEADER32,
 *pBASE_ACESTRUC_FILEHEADER32;

typedef struct
{
  uint16_t  HEAD_CRC;
  uint16_t  HEAD_SIZE;
  uint8_t   HEAD_TYPE;
  uint16_t  HEAD_FLAGS;

  uint64_t          PSIZE;
  uint64_t          SIZE;
  uint32_t   FTIME;
  uint32_t   ATTR;
  uint32_t   CRC32;
  tBASE_ACESTRUC_TECHSTRUC
          TECH;
  uint16_t  RESERVED;
  uint16_t  FNAME_SIZE;
  tLFN    FNAME;
  uint16_t  COMM_SIZE;
  char    COMM[32768];
  uint16_t  SECURITY_SIZE;
  char    SECURITY[512];
} tBASE_ACESTRUC_FILEHEADER,
 *pBASE_ACESTRUC_FILEHEADER;

typedef struct
{
  uint16_t  HEAD_CRC;
  uint16_t  HEAD_SIZE;
  uint8_t   HEAD_TYPE;
  uint16_t  HEAD_FLAGS;

  uint32_t   REC_BLK_SIZE;
  uint8_t   ACESIGN[BASE_ACESTRUC_ACESIGNLEN];
  uint32_t   REL_STRT;
  uint32_t   CLUSTER;
  uint32_t   CL_SIZE;
  uint16_t  REC_CRC;
//addsize:blks-crc16, rec-data (size=cl_size)
} tBASE_ACESTRUC_RECOVERYHEADER32,
 *pBASE_ACESTRUC_RECOVERYHEADER32;

typedef struct
{
  uint16_t  HEAD_CRC;
  uint16_t  HEAD_SIZE;
  uint8_t   HEAD_TYPE;
  uint16_t  HEAD_FLAGS;

  uint64_t
          REC_BLK_SIZE;
  uint8_t   ACESIGN[BASE_ACESTRUC_ACESIGNLEN];
  uint64_t
          REL_STRT;
  uint32_t   CLUSTER;
  uint32_t   CL_SIZE;
  uint16_t  REC_CRC;
//addsize:blks-crc16, rec-data (size=cl_size)
} tBASE_ACESTRUC_RECOVERYHEADER,
 *pBASE_ACESTRUC_RECOVERYHEADER;

typedef struct
{
  uint16_t  HEAD_CRC;
  uint16_t  HEAD_SIZE;
  uint8_t   HEAD_TYPE;
  uint16_t  HEAD_FLAGS;

  uint64_t
          REC_BLK_SIZE;
  uint8_t   ACESIGN[BASE_ACESTRUC_ACESIGNLEN];
  uint64_t
          REL_STRT;
  uint16_t  SECTORS;
  uint16_t  SECTORSPERCLUSTER;
  uint32_t   CL_SIZE;
//addsize:blks-crc16, rec-data (size=cl_size)
} tBASE_ACESTRUC_RECOVERYHEADER2,
 *pBASE_ACESTRUC_RECOVERYHEADER2;

typedef union {
  tBASE_ACESTRUC_BASICHEADER32    Basic32;
  tBASE_ACESTRUC_BASICHEADER      Basic;
  tBASE_ACESTRUC_MAINHEADER       Main;
  tBASE_ACESTRUC_FILEHEADER32     File32;
  tBASE_ACESTRUC_FILEHEADER       File;
  tBASE_ACESTRUC_RECOVERYHEADER32 Recovery32;
  tBASE_ACESTRUC_RECOVERYHEADER   Recovery;
  tBASE_ACESTRUC_RECOVERYHEADER2  Recovery2;
} tBASE_ACESTRUC_HEADER,
 *pBASE_ACESTRUC_HEADER;

#define RESTOREALIGNMENT
#include "packprag.h"

DECLEXT const char* const BASE_ACESTRUC_ACESIGN
#ifdef INCL_BASE_ACESTRUC_EXCLUSIVE
  #ifdef USELIMITED
        = "USELTD"
  #else
        = "**ACE**"
  #endif
#endif
;

DECLEXT const char* const BASE_ACESTRUC_EXTENSION
#ifdef INCL_BASE_ACESTRUC_EXCLUSIVE
  #ifdef USELIMITED
        = ".acl"
  #else
        = ".ace"
  #endif
#endif
;

DECLEXT int const BASE_ACESTRUC_IsACL
#ifdef INCL_BASE_ACESTRUC_EXCLUSIVE
  #ifdef USELIMITED
        = 1
  #else
        = 0
  #endif
#endif
;


#undef DECLEXT

#endif
