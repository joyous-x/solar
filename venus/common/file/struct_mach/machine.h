#ifndef _MACH_O_MACHINE_H
#define _MACH_O_MACHINE_H

typedef int	cpu_type_t;
typedef int	cpu_subtype_t;

/*
 * Capability bits used in the definition of cpu_type.
 */
#define CPU_ARCH_MASK           0xff000000      /* mask for architecture bits */
#define CPU_ARCH_ABI64          0x01000000      /* 64 bit ABI */
#define CPU_ARCH_ABI64_32       0x02000000      /* ABI for 64-bit hardware with 32-bit types; LP32 */

 /*
  *	Machine types known by all.
  */

#define CPU_TYPE_ANY            ((cpu_type_t) -1)

#define CPU_TYPE_VAX            ((cpu_type_t) 1)
  /* skip				((cpu_type_t) 2)	*/
  /* skip				((cpu_type_t) 3)	*/
  /* skip				((cpu_type_t) 4)	*/
  /* skip				((cpu_type_t) 5)	*/
#define CPU_TYPE_MC680x0        ((cpu_type_t) 6)
#define CPU_TYPE_X86            ((cpu_type_t) 7)
#define CPU_TYPE_I386           CPU_TYPE_X86            /* compatibility */
#define CPU_TYPE_X86_64         (CPU_TYPE_X86 | CPU_ARCH_ABI64)

/* skip CPU_TYPE_MIPS		((cpu_type_t) 8)	*/
/* skip                         ((cpu_type_t) 9)	*/
#define CPU_TYPE_MC98000        ((cpu_type_t) 10)
#define CPU_TYPE_HPPA           ((cpu_type_t) 11)
#define CPU_TYPE_ARM            ((cpu_type_t) 12)
#define CPU_TYPE_ARM64          (CPU_TYPE_ARM | CPU_ARCH_ABI64)
#define CPU_TYPE_ARM64_32       (CPU_TYPE_ARM | CPU_ARCH_ABI64_32)
#define CPU_TYPE_MC88000        ((cpu_type_t) 13)
#define CPU_TYPE_SPARC          ((cpu_type_t) 14)
#define CPU_TYPE_I860           ((cpu_type_t) 15)
/* skip	CPU_TYPE_ALPHA		((cpu_type_t) 16)	*/
/* skip				((cpu_type_t) 17)	*/
#define CPU_TYPE_POWERPC                ((cpu_type_t) 18)
#define CPU_TYPE_POWERPC64              (CPU_TYPE_POWERPC | CPU_ARCH_ABI64)
/* skip				((cpu_type_t) 19)	*/
/* skip				((cpu_type_t) 20 */
/* skip				((cpu_type_t) 21 */
/* skip				((cpu_type_t) 22 */

#endif //_MACH_O_MACHINE_H