#include "FatArchitecture.hpp"
#include "FileUtils.hpp"
#include <stdio.h>


FatArchitecture::FatArchitecture(FILE *file, FatHeader *fatHeader)
{
	FileUtils::clearSwap();
	if (fatHeader->getMagic() == FAT_CIGAM)
		FileUtils::setSwap();

	FileUtils::readUint32(file, (unsigned int *)&(this->cputype));
	FileUtils::readUint32(file, (unsigned int *)&(this->cpusubtype));
	FileUtils::readUint32(file, &(this->offset));
	FileUtils::readUint32(file, &(this->size));
	FileUtils::readUint32(file, &(this->align));
}

cpu_type_t FatArchitecture::getCpuType()
{
	return this->cputype;
}

cpu_subtype_t FatArchitecture::getCpuSubtype()
{
	return this->cpusubtype;
}

uint32_t FatArchitecture::getOffset()
{
	return this->offset;
}

uint32_t FatArchitecture::getSize()
{
	return this->size;
}

uint32_t FatArchitecture::getAlign()
{
	return this->align;
}

void FatArchitecture::print()
{
    printf("cputype: 0x%x, cpusubtype: 0x%x, offset: %u, size: %u, align: 2 ^ %u",
            this->cputype, this->cpusubtype, this->offset, this->size, this->align);
}

FatArchitecture::~FatArchitecture()
{
}
