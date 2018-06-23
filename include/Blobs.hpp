#ifndef __BLOBS_HPP
#define __BLOBS_HPP

#include <stdint.h>
#include <stdlib.h>
#include <vector>

#include "FileUtils.hpp"
#include "SimpleLoadCommands.hpp"

#define CSMAGIC_EMBEDDED_SIGNATURE 	0xFADE0CC0
#define CSMAGIC_DIRECTORY_BLOB		0xFADE0C02
#define CSMAGIC_REQUIREMENT_SET		0xFADE0C01
#define CSMAGIC_REQUIREMENT_BLOB	0xFADE0C00 //NOT USED
#define CSMAGIC_ENTITLEMENTS_BLOB	0xFADE7171

#define CODE_DIRECTORY_BLOB     0x00000
#define REQUIREMENTS            0x00002
#define ENTITLEMENTS            0x00005
#define CMS_BLOB                0x10000
#define IDENTIFICATION          0x10001 //NOT USED

#define CODE_DIRECTORY_BLOB_VERSION0 0x20001
#define CODE_DIRECTORY_BLOB_VERSION1 0x20100
#define CODE_DIRECTORY_BLOB_VERSION2 0x20200

#define CDB_FLAG_ADHOC 0x0002

struct subblob {
	uint32_t type;
	uint32_t offset;
};

class SuperBlob
{
private:
	uint32_t length;
	uint32_t numBlobs;
	std::vector<struct subblob> subblobs;

    uint32_t realOffset;

    bool isConstructMode;
    bool areBlobsSet;
    std::vector<uint32_t> types;
    std::vector<uint32_t> offsets;

public:
	SuperBlob(FILE *file, LinkEditCmd sigCmd);
	SuperBlob();
	uint32_t getLength();
	uint32_t getNumBlobs();
	std::vector<struct subblob> getSubBlobs();
	
    uint32_t getRealOffset();

    void setBlobs(std::vector<uint32_t> types, std::vector<uint32_t> sizes);

    void serialize(FILE *file);
};

class CodeDirectoryBlob
{
private:
    bool isConstructMode;
	uint32_t length;
	uint32_t version;
	uint32_t flags;
	uint32_t hashOffset;
	uint32_t identOffset;
	uint32_t nSpecialSlots;
	uint32_t nCodeSlots;
	uint32_t codeLimit;
	uint8_t hashSize;
	uint8_t hashType;
	uint8_t platform;
	uint8_t pageSize;
	uint32_t spare2;
	uint32_t scatterOffset;
	uint32_t teamOffset;
	std::vector<char *> hashes;

    char *hashesBuffer;

    // Applicable only if isConstructMode is true
    bool isLengthSet;
    bool isVersionSet;
    bool areFlagsSet;
    bool isHashOffsetSet;
    bool isIdentOffsetSet;
    bool isSpecialSlotsNrSet;
    bool isCodeSlotsNrSet;
    bool isCodeLimitSet;
    bool isHashSizeSet;
    bool isHashTypeSet;
    bool isPlatformSet;
    bool isPageSizeSet;
    bool isSpare2Set;
    bool isScatterOffsetSet;
    bool isTeamOffsetSet;
    char *identity;
    bool isIdentitySet;
    bool areHashesSet;

    void autoSetLength();
    void autoSetHashOffset();
    void autoSetIdentOffset();
public:
	CodeDirectoryBlob(FILE *file, LinkEditCmd sigCmd, SuperBlob sb);
	CodeDirectoryBlob();
	~CodeDirectoryBlob();
	uint32_t getLength();
	uint32_t getVersion();
	uint32_t getFlags();
	uint32_t getHashOffset();
	uint32_t getIdentOffset();
	uint32_t getNSpecialSlots();
	uint32_t getNCodeSlots();
	uint32_t getCodeLimit();
	uint8_t getHashSize();
	uint8_t getHashType();
	uint8_t getPlatform();
	uint8_t getPageSize();
	uint32_t getSpare2();
	uint32_t getScatterOffset();
	uint32_t getTeamOffset();
	std::vector<char *> getHashes();

    // Applicable only if isConstructMode is true
    void setVersion(uint32_t version);
    void setFlags(uint32_t flags);
    void setNSpecialSlots(uint32_t nSpecialSlots);
    void setNCodeSlots(uint32_t nCodeSlots);
    void setCodeLimit(uint32_t codeLimit);
    void setHashSize(uint8_t hashSize);
    void setHashType(uint8_t hashType);
    void setPlatform(uint8_t platform);
    void setPageSize(uint8_t pageSize);
    void setSpare2(uint32_t spare2);
    void setScatterOffset(uint32_t scatterOffset);
    void setTeamOffset(uint32_t teamOffset);

    void setIdentity(char *identity);
    void setHashes(std::vector<char *> specialHashes);

    void serialize(FILE *file);
};


class RequirementSet
{
private:
	uint32_t length;
	uint32_t numBlobs;
	std::vector<struct subblob> subblobs;

    uint32_t realOffset;

    // Applicable only if isConstructMode is true
    bool isConstructMode;
    bool isLengthSet;
    bool isNumBlobsSet;

    void autoSetLength();
public:
    RequirementSet(FILE *file, uint32_t realOffset);
    RequirementSet();
	uint32_t getLength();
	uint32_t getNumBlobs();
	uint32_t getRealOffset();
	std::vector<struct subblob> getSubBlobs();

    void setNumBlobs(uint32_t numBlobs);

    void serialize(FILE *file);
};

#endif
