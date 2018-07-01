#ifndef __FAT_ARCHITECTURE
#define __FAT_ARCHITECTURE

#include "FatHeader.hpp"
#include "cputypes.h"


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
