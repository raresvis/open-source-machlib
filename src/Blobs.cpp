#include "Blobs.hpp"

SuperBlob::SuperBlob()
{
    isConstructMode = true;
    areBlobsSet = false;
}

SuperBlob::SuperBlob(FILE *file, LinkEditCmd sigCmd)
    : isConstructMode(false), areBlobsSet(false)
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

void SuperBlob::setBlobs(std::vector<uint32_t> types, std::vector<uint32_t> sizes)
{
    uint32_t currentOffset = 0;

    if (types.size() != sizes.size()) {
        printf("Size mismatch!\n");
        exit(1);
    }

    this->types = types;

    currentOffset = 3 * sizeof(uint32_t) + types.size() * 2 * sizeof(uint32_t);

    for (int i = 0; i < types.size(); i++) {
        offsets.push_back(currentOffset);
        currentOffset += sizes[i];
    }

    this->length = currentOffset;
    this->numBlobs = types.size();
}

void SuperBlob::serialize(FILE *file)
{
    FileUtils::writeNetworkUint32(file, CSMAGIC_EMBEDDED_SIGNATURE);
    FileUtils::writeNetworkUint32(file, length);
    FileUtils::writeNetworkUint32(file, numBlobs);

    for (int i = 0; i < types.size(); i++) {
        FileUtils::writeNetworkUint32(file, types[i]);
        FileUtils::writeNetworkUint32(file, offsets[i]);
    }
}

CodeDirectoryBlob::CodeDirectoryBlob()
{
    isConstructMode = true;
    isLengthSet = false;
    isVersionSet = false;
    areFlagsSet = false;
    isHashOffsetSet = false;
    isIdentOffsetSet = false;
    isSpecialSlotsNrSet = false;
    isCodeSlotsNrSet = false;
    isCodeLimitSet = false;
    isHashSizeSet = false;
    isHashTypeSet = false;
    isPlatformSet = false;
    isSpare2Set = false;
    isScatterOffsetSet = false;
    isTeamOffsetSet = false;
    isIdentitySet = false;
    areHashesSet = false;
}

CodeDirectoryBlob::CodeDirectoryBlob(FILE *file, LinkEditCmd sigCmd,
				     SuperBlob sb) :
    isConstructMode(false)
{
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
	hashesBuffer = (char*) malloc(hashSize * (nSpecialSlots + nCodeSlots));
	for (uint32_t i = 0; i < nSpecialSlots; i++) {
		char *cur_buf = hashesBuffer + i * hashSize;
		FileUtils::readBytes(file, cur_buf, hashSize);
		hashes.push_back(cur_buf);
	}

	for (uint32_t i = 0; i < nCodeSlots; i++) {
		char *cur_buf = hashesBuffer + i * hashSize + nSpecialSlots * hashSize;
		FileUtils::readBytes(file, cur_buf, hashSize);
		hashes.push_back(cur_buf);
	}
	
    // Applicable only if isConstructMode is true
    isConstructMode = false;
    isLengthSet = false;
    isVersionSet = false;
    areFlagsSet = false;
    isHashOffsetSet = false;
    isIdentOffsetSet = false;
    isSpecialSlotsNrSet = false;
    isCodeSlotsNrSet = false;
    isCodeLimitSet = false;
    isHashSizeSet = false;
    isHashTypeSet = false;
    isPlatformSet = false;
    isSpare2Set = false;
    isScatterOffsetSet = false;
    isTeamOffsetSet = false;
    isIdentitySet = false;
    areHashesSet = false;
}

uint32_t CodeDirectoryBlob::getLength()
{
    if (isConstructMode && !isLengthSet) {
        printf("Length is not set in construct mode!\n");
        exit(1);
    }

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

void CodeDirectoryBlob::setVersion(uint32_t version)
{
    this->version = version;
    isVersionSet = true;

    if (isConstructMode && isIdentitySet && isSpecialSlotsNrSet
            && isCodeSlotsNrSet && isHashSizeSet && isHashTypeSet) {
        autoSetLength();
        autoSetHashOffset();
        autoSetIdentOffset();
    }
}

void CodeDirectoryBlob::setFlags(uint32_t flags)
{
    this->flags = flags;
    areFlagsSet = true;
}

void CodeDirectoryBlob::setNSpecialSlots(uint32_t nSpecialSlots)
{
    this->nSpecialSlots = nSpecialSlots;
    isSpecialSlotsNrSet = true;

    if (isConstructMode && isIdentitySet && isCodeSlotsNrSet
            && isHashSizeSet && isHashTypeSet && isVersionSet) {
        autoSetLength();
        autoSetHashOffset();
        autoSetIdentOffset();
    }
}

void CodeDirectoryBlob::setNCodeSlots(uint32_t nCodeSlots)
{
    this->nCodeSlots = nCodeSlots;
    isCodeSlotsNrSet = true;

    if (isConstructMode && isIdentitySet && isSpecialSlotsNrSet
            && isHashSizeSet && isHashTypeSet && isVersionSet) {
        autoSetLength();
        autoSetHashOffset();
        autoSetIdentOffset();
    }
}

void CodeDirectoryBlob::setCodeLimit(uint32_t codeLimit)
{
    this->codeLimit = codeLimit;
    isCodeLimitSet = true;
}

void CodeDirectoryBlob::setHashSize(uint8_t hashSize)
{
    this->hashSize = hashSize;
    isHashSizeSet = true;

    if (isConstructMode && isIdentitySet && isSpecialSlotsNrSet
            && isCodeSlotsNrSet && isHashTypeSet && isVersionSet) {
        autoSetLength();
        autoSetHashOffset();
        autoSetIdentOffset();
    }
}

void CodeDirectoryBlob::setHashType(uint8_t hashType)
{
    this->hashType = hashType;
    isHashTypeSet = true;

    if (isConstructMode && isIdentitySet && isSpecialSlotsNrSet
            && isCodeSlotsNrSet && isHashSizeSet && isVersionSet) {
        autoSetLength();
        autoSetHashOffset();
        autoSetIdentOffset();
    }
}

void CodeDirectoryBlob::setPlatform(uint8_t platform)
{
    this->platform = platform;
    isPlatformSet = true;
}

void CodeDirectoryBlob::setPageSize(uint8_t pageSize)
{
    this->pageSize = pageSize;
    isPageSizeSet = true;
}

void CodeDirectoryBlob::setSpare2(uint32_t spare2)
{
    this->spare2 = spare2;
    isSpare2Set = true;
}

void CodeDirectoryBlob::setScatterOffset(uint32_t scatterOffset)
{
    this->scatterOffset = scatterOffset;
    isScatterOffsetSet = true;
}

void CodeDirectoryBlob::setTeamOffset(uint32_t teamOffset)
{
    this->teamOffset = teamOffset;
    isTeamOffsetSet = true;
}

void CodeDirectoryBlob::autoSetLength()
{
    if (!isConstructMode || !isIdentitySet || !isSpecialSlotsNrSet
            || !isCodeSlotsNrSet || !isHashSizeSet || !isHashTypeSet || !isVersionSet) {
        printf("Required fields are not all filled!\n");
        return;
    }

    if (isVersionSet && (version != CODE_DIRECTORY_BLOB_VERSION1 && version != CODE_DIRECTORY_BLOB_VERSION2)) {
        printf("Invalid Code Directory Version field %x!\n", version);
        return;
    }

    uint32_t codeDirBlobStructureSize = (version == CODE_DIRECTORY_BLOB_VERSION1) ? 48 : 52;
    // length = sizeof code dir structure + strlen(identity) + null terminator + sizeof hash slots
    length = codeDirBlobStructureSize + strlen(identity) + 1
                                + (nSpecialSlots + nCodeSlots) * hashSize;

    isLengthSet = true;
}

void CodeDirectoryBlob::autoSetHashOffset()
{
    if (!isLengthSet) {
        printf("Length needs to be set before setting the HashOffset!\n");
        return;
    }

    uint32_t codeDirBlobStructureSize = (version == CODE_DIRECTORY_BLOB_VERSION1) ? 48 : 56;
    hashOffset = codeDirBlobStructureSize + strlen(identity) + 1 + nSpecialSlots * hashSize;

    isHashOffsetSet = true;
}

void CodeDirectoryBlob::autoSetIdentOffset()
{
    if (!isLengthSet) {
        printf("Length needs to be set before setting the HashOffset!\n");
        return;
    }

    identOffset = (version == CODE_DIRECTORY_BLOB_VERSION1) ? 48 : 56;

    isIdentOffsetSet = true;
}

void CodeDirectoryBlob::setIdentity(char *identity)
{
    this->identity = identity;
    isIdentitySet = true;

    if (isConstructMode && isSpecialSlotsNrSet && isCodeSlotsNrSet
            && isHashSizeSet && isHashTypeSet && isVersionSet) {
        autoSetLength();
        autoSetHashOffset();
        autoSetIdentOffset();
    }
}

void CodeDirectoryBlob::setHashes(std::vector<char *> hashes)
{
    this->hashes = hashes;
}

void CodeDirectoryBlob::serialize(FILE *file)
{
    FileUtils::writeNetworkUint32(file, CSMAGIC_DIRECTORY_BLOB);
    FileUtils::writeNetworkUint32(file, length);
    FileUtils::writeNetworkUint32(file, version);
    FileUtils::writeNetworkUint32(file, flags);
    FileUtils::writeNetworkUint32(file, hashOffset);
    FileUtils::writeNetworkUint32(file, identOffset);
    FileUtils::writeNetworkUint32(file, nSpecialSlots);
    FileUtils::writeNetworkUint32(file, nCodeSlots);
    FileUtils::writeNetworkUint32(file, codeLimit);
    FileUtils::writeUint8(file, hashSize);
    FileUtils::writeUint8(file, hashType);
    FileUtils::writeUint8(file, platform);
    FileUtils::writeUint8(file, pageSize);
    FileUtils::writeNetworkUint32(file, spare2);
    FileUtils::writeNetworkUint32(file, scatterOffset);
    //FileUtils::writeNetworkUint32(file, teamOffset);
    FileUtils::writeBytes(file, identity, strlen(identity) + 1);

    for (int i = 0; i < hashes.size(); i++)
        FileUtils::writeBytes(file, hashes[i], hashSize);
}

CodeDirectoryBlob::~CodeDirectoryBlob()
{
    if (isConstructMode)
        for (int i = 0; i < hashes.size(); i++)
            delete hashes[i];
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
