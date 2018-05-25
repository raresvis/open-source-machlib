#include "FatHeader.hpp"
#include "FileUtils.hpp"

FatHeader::FatHeader(FILE *file)
{
    FileUtils::clearSwap();
	FileUtils::readUint32(file, &(this->magic));

    if (magic == FAT_CIGAM)
        FileUtils::setSwap();

	FileUtils::readUint32(file, &(this->nfat_arch));
}

uint32_t FatHeader::getMagic()
{
	return this->magic;
}

uint32_t FatHeader::getNFatArches()
{
	return this->nfat_arch;
}

void FatHeader::print()
{
    printf("magic: 0x%x nfat_arch: %u",
            this->magic, this->nfat_arch);
}

FatHeader::~FatHeader()
{
}
