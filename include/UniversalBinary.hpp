#ifndef __UNIVERSAL_BINARY
#define __UNIVERSAL_BINARY

#include "MachO.hpp"
#include "FatHeader.hpp"
#include "FatArchitecture.hpp"

class UniversalBinary {
private:
	FILE *file;
	std::string fileName;
	bool isUniversal;
	FatHeader *fatHeader;
	std::vector<FatArchitecture *> fat_arches;
	std::vector<MachO *> machOs;

	uint32_t peekMagic();
public:
	UniversalBinary(char *fileName);
	FatHeader *getFatHeader();
    bool getIsUniversal();
	std::vector<FatArchitecture *> getFatArches();
	std::vector<MachO *> getMachOs();
	~UniversalBinary();
};

#endif
