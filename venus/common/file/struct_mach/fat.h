#ifndef _MACH_O_FAT_H_
#define _MACH_O_FAT_H_

#include "../types.h"
#include "machine.h"
/*
 * This header file describes the structures of the file format for "fat"
 * architecture specific file (wrapper design).  At the begining of the file
 * there is one fat_header structure followed by a number of fat_arch
 * structures.  For each architecture in the file, specified by a pair of
 * cputype and cpusubtype, the fat_header describes the file offset, file
 * size and alignment in the file of the architecture specific member.
 * The padded bytes in the file to place each member on it's specific alignment
 * are defined to be read as zeros and can be left as "holes" if the file system
 * can support them as long as they read as zeros.
 *
 * All structures defined here are always written and read to/from disk
 * in big-endian order.
 */

#define FAT_MAGIC	0xcafebabe
#define FAT_CIGAM	0xbebafeca	/* NXSwapLong(FAT_MAGIC) */

typedef struct fat_header {
	uint32_t	magic;		/* FAT_MAGIC or FAT_MAGIC_64 */
	uint32_t	nfat_arch;	/* number of structs that follow */
} MACH_FAT_HEADER;

typedef struct fat_arch {
	cpu_type_t	cputype;	/* cpu specifier (int) */
	cpu_subtype_t	cpusubtype;	/* machine specifier (int) */
	uint32_t	offset;		/* file offset to this object file */
	uint32_t	size;		/* size of this object file */
	uint32_t	align;		/* alignment as a power of 2 */
} MACH_FAT_ARCH;

/*
 * The support for the 64-bit fat file format described here is a work in
 * progress and not yet fully supported in all the Apple Developer Tools.
 *
 * When a slice is greater than 4mb or an offset to a slice is greater than 4mb
 * then the 64-bit fat file format is used.
 */
#define FAT_MAGIC_64	0xcafebabf
#define FAT_CIGAM_64	0xbfbafeca	/* NXSwapLong(FAT_MAGIC_64) */

typedef struct fat_arch_64 {
	cpu_type_t	cputype;	/* cpu specifier (int) */
	cpu_subtype_t	cpusubtype;	/* machine specifier (int) */
	uint64_t	offset;		/* file offset to this object file */
	uint64_t	size;		/* size of this object file */
	uint32_t	align;		/* alignment as a power of 2 */
	uint32_t	reserved;	/* reserved */
} MACH_FAT_ARCH_64;

#endif /* _MACH_O_FAT_H_ */