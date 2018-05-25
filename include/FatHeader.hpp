#ifndef __FAT_HEADER
#define __FAT_HEADER

#include <stdio.h>
#include <stdint.h>

#define FAT_MAGIC	0xcafebabe
#define FAT_CIGAM	0xbebafeca	/* NXSwapLong(FAT_MAGIC) */


class FatHeader {
private:
	uint32_t	magic;		    /* FAT_MAGIC or FAT_MAGIC_64 */
	uint32_t	nfat_arch;	    /* number of structs that follow */

public:
	FatHeader(FILE *file);
	uint32_t getMagic();
	uint32_t getNFatArches();
    void print();
	~FatHeader();
};

#endif
