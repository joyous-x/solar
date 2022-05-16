#pragma once
#include <stdint.h>

namespace biff {

/* biff8 and later */
#define BIFF8_MAX_RECORD_DATA_LENGTH (8224)
/* biff7 and earily */
#define BIFF7_MAX_RECORD_DATA_LENGTH (2080)

const int unknown = 0;
const int v2 = 0x0009;
const int v3 = 0x0209;
const int v4 = 0x0409;
const int v5 = 0x0809;
const int v8 = 0x0809;

enum opcode {
	OPC_Invalid = 0x0,
	OPC_BOF = 0x809,			/* BOF, specifies the beginning of the individual substreams */
	OPC_EOF = 0x0A,             /* EOF, specifies the end of a collection of records, eg. substreams、revision stream... */
	OPC_Codepage = 0x42,
	OPC_Formula = 0x06,
	OPC_Array = 0x221,			/* Array record */
	OPC_ShrFmla = 0x4BC,		/* ShrFmla record */
	OPC_Continue = 0x3C,
	OPC_BoundSheet = 0x85,
	OPC_SST = 252,
	OPC_Lbl = 24,				/* Lbl record, specifies a defined name */
	OPC_MsoDrawingGroup = 235,  /* specifies basic information about a sheet */
	OPC_LabelSst = 253,         /* Cell Value, String Constant/ SST */
	OPC_Dimensions = 0x200,     /* Cell Table Size */
	OPC_Blank = 0x201,          /* Cell Value, Blank Cell */
	OPC_Number = 515,           /* Cell Value, Floating-Point Number */
	OPC_Label = 516,			/* Cell Value, String Constant */
	OPC_BoolErr = 0x205,        /* Cell Value, Boolean or Error */
	OPC_String = 0x207,			/* string value of a formula */
	OPC_Row = 0x208,
	OPC_Index = 0x20B,
	OPC_RK = 638,               /* Cell Value, RK Number */
	OPC_Dconn = 0x876,			/* specifies information for a single data connection */
	OPC_Password = 0x13,
	OPC_FilePass = 0x2F,
	OPC_Prot4RevPass = 0x1BC,   /* Shared Workbook Protection Password */ 
	OPC_Note = 28,
	OPC_ExternSheet = 23,
	OPC_SupBook = 430,
	/* for encryption*/
	OPC_UsrExcl = 404,
	OPC_FileLock = 405,
	OPC_RRDInfo = 406,
	OPC_InterfaceHdr = 225,
	OPC_RRDHead = 312,
	/* for shared workbook */
	OPC_RRDInsDel = 311,
	OPC_RRDInsDelBegin = 336,
	OPC_RRDInsDelEnd = 337,
	OPC_RRDMove = 320,
	OPC_RRDMoveBegin = 334,
	OPC_RRDMoveEnd = 335,
	OPC_RRDChgCell = 315,
	OPC_RRDUserView = 428,
	OPC_RRDRenSheet = 318,
	OPC_RRInsertSh = 333,
	OPC_RRDDefName = 339,
	OPC_RRDConflict = 338,
	OPC_RRDTQSIF = 2056,
	OPC_RRFormat = 330,
	OPC_RRAutoFmt = 331,
	/*
	* # LABEL record #a# difference BIFF4 and BIFF5+
	* # WRITEACCESS record
	* # RK record
	*/
};

/*
* There MUST be exactly one Globals Substream in a Workbook Stream,
* and the Globals Substream MUST be the first substream in the Workbook Stream.
*/
enum bof_type {
	BT_INVALID = 0x0,
	BT_WORKBOOK = 0x0005,			/* workbook substream */
	BT_VBA_MODULE = 0x0006,			/* Visual Basic module */
	BT_WORKS_DIALOG_HEET = 0x0010,	/* the dialog sheet substream or the worksheet substream */
	BT_CHART_SHEET = 0x0020,
	BT_MACRO_SHEET = 0x0040,
	BT_WORKSPACE = 0x0100,
};

/*
* @brief    Record
* @detail   type(2 bytes) + size(2 bytes, 0 <= size <= 8224) + data(OPTIONAL, variable)
*/
typedef struct __st_record_header {
    uint16_t opcode;
    uint16_t length;
} record_header_t;

int version(const uint8_t* data, uint32_t size, uint16_t* bof_type, bool little_endian = true);

}; //> namespace biff
