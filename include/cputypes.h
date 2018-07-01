/*
 * the integer_t typedef is taken from
 * ./mach/i386/vm_types.h.
 */

typedef int			integer_t;

/*
 * The cpy_type_t and cpu_subtype_t are taken from
 * /usr/include/mach/machine.h
 */

typedef integer_t       cpu_type_t;
typedef integer_t       cpu_subtype_t;

/*
 * The cputypes are taken from
 * usr/include/mach/machine.h
 */

#define CPU_ARCH_ABI64	0x01000000		/* 64 bit ABI */

#define CPU_TYPE_I386           ((cpu_type_t) 7)
#define CPU_TYPE_X86		    (CPU_TYPE_I386)
#define CPU_TYPE_X86_64		    (CPU_TYPE_X86 | CPU_ARCH_ABI64)
#define CPU_TYPE_ARM		    ((cpu_type_t) 12)
#define CPU_TYPE_ARM64          (CPU_TYPE_ARM | CPU_ARCH_ABI64)
#define CPU_TYPE_POWERPC		((cpu_type_t) 18)
#define CPU_TYPE_POWERPC64		(CPU_TYPE_POWERPC | CPU_ARCH_ABI64)

/*
 * The cpusubtype defines are skipped.
 */
