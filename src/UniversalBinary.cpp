#include "UniversalBinary.hpp"
#include "FileUtils.hpp"

UniversalBinary::UniversalBinary(char *fileName) :
	fileName(std::string(fileName)), isUniversal(false),
	fatHeader(nullptr),
	fat_arches(std::vector<FatArchitecture *>()),
	machOs(std::vector<MachO *>())
{
	uint32_t magic = 0;

	this->file = fopen(fileName, "rb");

	magic = peekMagic();

	if (magic == FAT_MAGIC || magic == FAT_CIGAM) {
		this->isUniversal = true;

		// Read the fat_header.
		this->fatHeader = new FatHeader(this->file);
		// Read the fat arches.
		for (int i = 0; i < this->fatHeader->getNFatArches(); i++)
			this->fat_arches.push_back(new FatArchitecture(this->file, this->fatHeader));
        // Read the MachOs
        for (int i = 0; i < this->fat_arches.size(); i++)
            this->machOs.push_back(new MachO(fileName, this->file, this->fat_arches[i]->getOffset()));
	} else {
		this->machOs.push_back(new MachO(fileName, this->file, 0));
	}
}

FatHeader *UniversalBinary::getFatHeader()
{
    if (!this->isUniversal)
        return nullptr;

	return this->fatHeader;
}

bool UniversalBinary::getIsUniversal()
{
    return this->isUniversal;
}

std::vector<FatArchitecture *> UniversalBinary::getFatArches()
{
    if (!this->isUniversal)
        return std::vector<FatArchitecture *>();

	return this->fat_arches;
}

std::vector<MachO *> UniversalBinary::getMachOs()
{
	return this->machOs;
}

uint32_t UniversalBinary::peekMagic()
{
	uint32_t magic = 0;

	FileUtils::clearSwap();
	FileUtils::readUint32(this->file, &magic);

	fseek(this->file, 0, SEEK_SET);

	return magic;
}

UniversalBinary::~UniversalBinary()
{
	fclose(this->file);

	for (int i = 0; i < this->machOs.size(); i++)
		free(this->machOs[i]);

	for (int i = 0; i < this->fat_arches.size(); i++)
		free(this->fat_arches[i]);

	if (this->fatHeader != NULL)
		free(this->fatHeader);
}
