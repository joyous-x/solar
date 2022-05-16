#ifndef __STRUCT_MS_LNK_
#define __STRUCT_MS_LNK_

#pragma pack(push, 1)

/* 00021401-0000-0000-C000-000000000046 */
#define MS_LNK_CLSID { 0x01, 0x14, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46 }

#define bitLinkFlags_HasLinkTargetIDList	0
#define bitLinkFlags_HasLinkInfo			1
#define bitLinkFlags_HasName				2
#define bitLinkFlags_HasRelativePath		3
#define bitLinkFlags_HasWorkingDir			4
#define bitLinkFlags_HasArguments			5
#define bitLinkFlags_HasIconLocation		6
#define bitLinkFlags_IsUnicode				7
#define bitLinkFlags_ForceNoLinkInfo		8
#define bitLinkFlags_HasExpString			9
#define bitLinkFlags_RunInSeparateProcess	10
#define bitLinkFlags_Unused1				11
#define bitLinkFlags_HasDarwinID			12
#define bitLinkFlags_RunAsUser				13
#define bitLinkFlags_HasExpIcon				14
#define bitLinkFlags_NoPidlAlias			15
#define bitLinkFlags_Unused2				16
#define bitLinkFlags_RunWithShimLayer		17
#define bitLinkFlags_ForceNoLinkTrack		18
#define bitLinkFlags_EnableTargetMetadata	19
#define bitLinkFlags_DisableLinkPathTracking	20
#define bitLinkFlags_DisableKnownFolderTracking 21
#define bitLinkFlags_DisableKnownFolderAlias	22
#define bitLinkFlags_AllowLinkToLink		23
#define bitLinkFlags_UnaliasOnSave			24
#define bitLinkFlags_PreferEnvironmentPath  25
#define bitLinkFlags_KeepLocalIDListForUNCTarget 26

#define bitAttributes_READONLY		0
#define bitAttributes_HIDDEN		1
#define bitAttributes_SYSTEM		2
#define bitAttributes_Reserved1		3
#define bitAttributes_DIRECTORY		4
#define bitAttributes_ARCHIVE		5
#define bitAttributes_Reserved2		6
#define bitAttributes_NORMAL		7
#define bitAttributes_TEMPORARY		8
#define bitAttributes_SPARSE_FILE	9
#define bitAttributes_REPARSE_POINT	10
#define bitAttributes_COMPRESSED	11
#define bitAttributes_OFFLINE		12
#define bitAttributes_NOT_CONTENT_INDEXED 13
#define bitAttributes_ENCRYPTED		14

#define virtualPathType_Root			1
#define virtualPathType_Volume			2
#define virtualPathType_File			3
#define virtualPathType_Network			4
#define virtualPathType_Compressed		5
#define virtualPathType_Uri				6
#define virtualPathType_ControlPanel	7

// 如果 VirtualPathType 为 ROOT 时，SortIndex 的值
#define VPT_ROOT_DataSortIndex_internet_explorer_1	0x00
#define VPT_ROOT_DataSortIndex_libraries			0x42
#define VPT_ROOT_DataSortIndex_users				0x44
#define VPT_ROOT_DataSortIndex_my_documents			0x48
#define VPT_ROOT_DataSortIndex_my_computer			0x50
#define VPT_ROOT_DataSortIndex_my_network_places	0x58
#define VPT_ROOT_DataSortIndex_recycle_bin			0x60
#define VPT_ROOT_DataSortIndex_internet_explorer_2	0x68
#define VPT_ROOT_DataSortIndex_unknown				0x70
#define VPT_ROOT_DataSortIndex_my_games				0x80

#define ClsidType_Invalid				0
#define ClsidType_Unknown				99
#define ClsidType_NetworkPlaces			1
#define ClsidType_NetworkDomain			2
#define ClsidType_NetworkServer			3
#define ClsidType_NetworkShare			4
#define ClsidType_MyComputer			5
#define ClsidType_Internet				6
#define ClsidType_ShellFSFolder			7
#define ClsidType_RecycleBin			8
#define ClsidType_ControlPanel			9
#define ClsidType_MyDocuments			10
#define ClsidType_Download				11
#define ClsidType_Pictures				12
#define ClsidType_Music					13
#define ClsidType_Desktop				14
#define ClsidType_Documents				15
#define ClsidType_Videos				16
#define ClsidType_3DObjects				17
#define ClsidType_Download7				18

typedef struct __st_MS_ShellLinkHeader {
	uint32_t header_size; // MUST be 0x0000004C.
	uint8_t  clsid[16];   // MUST be 00021401-0000-0000-C000-000000000046
	uint32_t link_flags;
	uint32_t attributes;
	uint64_t creation_time;
	uint64_t access_time;
	uint64_t write_time;
	uint32_t filesize;
	int32_t  icon_index;
	uint32_t show_command;
	struct HotKeyFlags {
		uint8_t low;  // 范围0-9A-Z(0x30-0x5A)、VK_F1-VK_F24(0x70-0x87)、VK_NUMLOCK(0x90)、VK_SCROLL(0x91)
		uint8_t	high; // HOTKEYF_SHIFT(0x01)、HOTKEYF_CONTROL(0x02)、HOTKEYF_ALT(0x04),组合键时为+值，如快捷键为:CTRL+ALT+1，对应值为(0x31,0x06)
	} hot_key;
	uint16_t reserved1;
	uint32_t reserved2;
	uint32_t reserved3;
} ms_shellink_header_t;

typedef struct __ST_NormalExtraDataBlock {
	uint16_t	size;
	uint16_t	version;
	uint32_t	signature;
	uint8_t		data[1]; /* variable */
} itemid_norm_extrablock_t;

typedef struct __ST_ItemIDTypeROOT {
	uint8_t					 sort_index; /* VPT_ROOT_DataSortIndex_ */
	uint8_t					 clsid[16];
	itemid_norm_extrablock_t extra_block; /* variable, may not exist */
	uint8_t					 unknown_data[1]; /* variable */
} itemid_root_t;

/* name or (sort_index + clsid) or unknown_data */
typedef struct __ST_ItemIDTypeVOLUME { 
	uint8_t		   name[256];
	uint8_t		   sort_index;
	uint8_t		   clsid[16];
	uint8_t		   unknown_data[1]; /* variable */
} itemid_volume_t;

typedef struct __ST_ItemIDTypeFile {
	uint8_t		   unknown;
	uint32_t	   filesize;
	uint32_t	   modified;
	uint16_t	   attributes;
	uint8_t        primary_name[256];
	itemid_norm_extrablock_t extra_block; /* variable, may not exist */
} itemid_file_t;

typedef struct __ST_ItemIDCplApplet {
	uint8_t		   flags;
	uint8_t		   sort_index;
	uint8_t		   clsid[16];
	uint16_t       path_size; /* unicode characters count */
	uint16_t       display_name_size; /* unicode characters count */
	uint8_t        path[1];
	uint8_t        display_name[1];
} itemid_cpl_applet_t;

typedef struct __MS_LinkTarget_ItemID {
	uint16_t size; /* specifies the size, in bytes, of the ItemID structure, including the ItemIDSize field*/
	uint8_t  type;
	union ItemIData {
		itemid_root_t root;
		itemid_volume_t volume;
		itemid_file_t file;
	} data;

	__MS_LinkTarget_ItemID() { memset((void*)this, 0, sizeof(__MS_LinkTarget_ItemID)); }
	uint8_t type_cata() { return (type & 0xF0) >> 4; }
	uint8_t type_data() { return type & 0x0F; }
} link_target_item_id_t;

typedef struct __MS_LinkTargetIDList {
	uint16_t size;  /* The size, in bytes, of the IDList field. */
	struct IDList {
		link_target_item_id_t* list; /* An array of zero or more ItemID structures */
		uint16_t terminal_id; /* MUST be 0 */
	} id_list;
} link_targetid_list_t;


#pragma pack(pop)
#endif // __STRUCT_MS_LNK_