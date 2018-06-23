#include "Blobs.hpp"

SuperBlob::SuperBlob()
{
}

SuperBlob::SuperBlob(FILE *file, LinkEditCmd sigCmd)
{
	struct subblob sb;
	uint32_t buf;

    realOffset = sigCmd.getDataRealOffset();

	fseek(file, realOffset, SEEK_SET);
	FileUtils::readNetworkUint32(file, &buf);		
	if (buf != CSMAGIC_EMBEDDED_SIGNATURE) {
                throw std::runtime_error("Signature not valid.");
	}

	FileUtils::readNetworkUint32(file, &length);		
	FileUtils::readNetworkUint32(file, &numBlobs);		

	for (unsigned int i = 0; i < numBlobs; i++) {
		FileUtils::readNetworkUint32(file, &sb.type);		
		FileUtils::readNetworkUint32(file, &sb.offset);		
		subblobs.push_back(sb);
	}

}

uint32_t SuperBlob::getLength()
{
	return length;
}

uint32_t SuperBlob::getNumBlobs()
{
	return numBlobs;
}

std::vector<struct subblob> SuperBlob::getSubBlobs()
{
	return subblobs;
}

uint32_t SuperBlob::getRealOffset()
{
    return realOffset;
}

CodeDirectoryBlob::CodeDirectoryBlob()
{
}

CodeDirectoryBlob::CodeDirectoryBlob(FILE *file, LinkEditCmd sigCmd,
				     SuperBlob sb)
{
	char *buf;
	uint32_t magic;
	uint32_t sbOffset = sigCmd.getDataRealOffset();
	uint32_t cdbOffset = 0;
	std::vector<struct subblob> sbs = sb.getSubBlobs();

	for (unsigned int i = 0; i < sbs.size(); i++) {
		if (sbs[i].type == CODE_DIRECTORY_BLOB) {
			cdbOffset = sbs[i].offset;
			break;
		}
	}

	fseek(file, sbOffset + cdbOffset, SEEK_SET);
	FileUtils::readNetworkUint32(file, &magic);		
	if (magic != CSMAGIC_DIRECTORY_BLOB) {
                throw std::runtime_error("Code Directory Blob wrong magic");
	}

	/* Read metadata */
	FileUtils::readNetworkUint32(file, &length);
	FileUtils::readNetworkUint32(file, &version);
	FileUtils::readNetworkUint32(file, &flags);
	FileUtils::readNetworkUint32(file, &hashOffset);
	FileUtils::readNetworkUint32(file, &identOffset);
	FileUtils::readNetworkUint32(file, &nSpecialSlots);
	FileUtils::readNetworkUint32(file, &nCodeSlots);
	FileUtils::readNetworkUint32(file, &codeLimit);
	FileUtils::readUint8(file, &hashSize);
	FileUtils::readUint8(file, &hashType);
	FileUtils::readUint8(file, &platform);
	FileUtils::readUint8(file, &pageSize);
	FileUtils::readNetworkUint32(file, &spare2);
	FileUtils::readNetworkUint32(file, &scatterOffset);
	FileUtils::readNetworkUint32(file, &teamOffset);

	/* Read hashes */
	uint32_t hashesStart = sbOffset + cdbOffset + hashOffset - hashSize * nSpecialSlots;
	fseek(file, hashesStart, SEEK_SET);
	buf = (char*) malloc(hashSize * (nSpecialSlots + nCodeSlots));
	for (uint32_t i = 0; i < nSpecialSlots; i++) {
		char *cur_buf = buf + i * hashSize;
		FileUtils::readBytes(file, cur_buf, hashSize);
		hashes.push_back(cur_buf);
	}

	for (uint32_t i = 0; i < nCodeSlots; i++) {
		char *cur_buf = buf + i * hashSize + nSpecialSlots * hashSize;
		FileUtils::readBytes(file, cur_buf, hashSize);
		hashes.push_back(cur_buf);
	}
	
}

uint32_t CodeDirectoryBlob::getLength()
{
	return length;
}

uint32_t CodeDirectoryBlob::getVersion()
{
	return version;
}

uint32_t CodeDirectoryBlob::getFlags()
{
	return flags;
}

uint32_t CodeDirectoryBlob::getHashOffset()
{
	return hashOffset;
}

uint32_t CodeDirectoryBlob::getIdentOffset()
{
	return identOffset;
}

uint32_t CodeDirectoryBlob::getNSpecialSlots()
{
	return nSpecialSlots;
}

uint32_t CodeDirectoryBlob::getNCodeSlots()
{
	return nCodeSlots;
}

uint32_t CodeDirectoryBlob::getCodeLimit()
{
	return codeLimit;
}

uint8_t CodeDirectoryBlob::getHashSize()
{
	return hashSize;
}

uint8_t CodeDirectoryBlob::getHashType()
{
	return hashType;
}

uint8_t CodeDirectoryBlob::getPlatform()
{
	return platform;
}

uint8_t CodeDirectoryBlob::getPageSize()
{
	return pageSize;
}

uint32_t CodeDirectoryBlob::getSpare2()
{
	return spare2;
}

uint32_t CodeDirectoryBlob::getScatterOffset()
{
	return scatterOffset;
}

uint32_t CodeDirectoryBlob::getTeamOffset()
{
	return teamOffset;
}

std::vector<char *>
	CodeDirectoryBlob::getHashes()
{
	return hashes;
}


RequirementSet::RequirementSet(FILE *file, uint32_t realOffset)
    : isConstructMode(false)
{
    uint32_t magic = 0;
	struct subblob sb;

    this->realOffset = realOffset;

	fseek(file, realOffset, SEEK_SET);
	FileUtils::readNetworkUint32(file, &magic);
	if (magic != CSMAGIC_REQUIREMENT_SET)
        throw std::runtime_error("Requirement Set magic number not valid!");

	FileUtils::readNetworkUint32(file, &length);
	FileUtils::readNetworkUint32(file, &numBlobs);

	for (unsigned int i = 0; i < numBlobs; i++) {
		FileUtils::readNetworkUint32(file, &sb.type);
		FileUtils::readNetworkUint32(file, &sb.offset);
		subblobs.push_back(sb);
	}

    // Applicable only if isConstructMode is true
    isLengthSet = false;
    isNumBlobsSet = false;;
}

RequirementSet::RequirementSet()
{
    isConstructMode = true;
    isLengthSet = false;
    isNumBlobsSet = false;;
}

uint32_t RequirementSet::getLength()
{
    if (isConstructMode && !isLengthSet) {
        printf("Length is not set in construct mode!\n");
        exit(1);
    }

    return length;
}

uint32_t RequirementSet::getNumBlobs()
{
	return numBlobs;
}

uint32_t RequirementSet::getRealOffset()
{
    return realOffset;
}

std::vector<struct subblob> RequirementSet::getSubBlobs()
{
    return subblobs;
}

void RequirementSet::autoSetLength()
{
    if (!isNumBlobsSet) {
        printf("Not all parameters have been set!\n");
        return;
    }

    // TODO: So far, the RequirementSet has minumum functionality, it does not
    // support RequirementBlob additions. So the length is set to accomodate
    // only for the magic number, length and numBlobs fields.
    this->length = 3 * sizeof(uint32_t);
    isLengthSet = true;
}

void RequirementSet::setNumBlobs(uint32_t numBlobs)
{
    this->numBlobs = numBlobs;
    isNumBlobsSet = true;

    autoSetLength();
}

void RequirementSet::serialize(FILE *file)
{
    FileUtils::writeNetworkUint32(file, CSMAGIC_REQUIREMENT_SET);
    FileUtils::writeNetworkUint32(file, length);
    FileUtils::writeNetworkUint32(file, numBlobs);
}
