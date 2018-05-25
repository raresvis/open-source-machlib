#ifndef __FAT_ARCHITECTURE
#define __FAT_ARCHITECTURE

#include "FatHeader.hpp"


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

class FatArchitecture {
private:
	cpu_type_t	    cputype;	/* cpu specifier (int) */
	cpu_subtype_t	cpusubtype;	/* machine specifier (int) */
	uint32_t	    offset;		/* file offset to this object file */
	uint32_t	    size;		/* size of this object file */
	uint32_t	    align;		/* alignment as a power of 2 */

public:
	FatArchitecture(FILE *file, FatHeader *fatHeader);
	cpu_type_t getCpuType();
	cpu_subtype_t getCpuSubtype();
	uint32_t getOffset();
	uint32_t getSize();
	uint32_t getAlign();
    void print();
	~FatArchitecture();
};

#endif
