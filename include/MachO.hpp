#ifndef __MACHO_HPP
#define __MACHO_HPP

#include <vector>
#include <map>
#include <cstring>
#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include "FileUtils.hpp"
#include "MachHeader.hpp"
#include "Section.hpp"
#include "Segment.hpp"
#include "Section32.hpp"
#include "Segment32.hpp"
#include "Section64.hpp"
#include "Segment64.hpp"
#include "SymbolTableHeader.hpp"
#include "StringTable.hpp"
#include "SymbolTableEntry.hpp"
#include "SymbolTableEntry32.hpp"
#include "SymbolTableEntry64.hpp"
#include "SimpleLoadCommands.hpp"
#include "LibraryInfo.hpp"
#include "DynamicSymbolTable.hpp"
#include "Blobs.hpp"
#include "Entitlements.hpp"


#define LC_SEGMENT32            0x01
#define LC_SEGMENT64            0x19
#define LC_SYMTAB               0x02
#define LC_LOAD_DYLINKER        0x0E

#define LC_UUID                 0x1B
#define UUID_SIZE               16

#define LC_REQ_DYLD             0x80000000
#define LC_MAIN                 (0x28|LC_REQ_DYLD)

#define LC_LOAD_DYLIB           0x0C

#define LC_FUNCTION_STARTS      0x26

#define LC_DYSYMTAB             0x0B

#define LC_CODE_SIGNATURE	0x1D

#define NAMEPREFIX              "func_"

struct myKextComp {
         bool operator()(char *a, char *b) const {
                 return std::strcmp(a, b) < 0;
         }
};

/*high level class*/
/*entry point of the library*/
class MachO
{
private:
        char *fileName;
        FILE *file;

        MachHeader header;

        std::vector<Segment *> segments;

        SymbolTableHeader symbolTableHeader;
        bool symbolTableHeaderPresent;

        DynamicSymbolTableHeader dynamicSymbolTableHeader;

        /*flags to avoid recomputing*/
        bool stringTableComputed;
        bool symbolTableComputed;

        /*the string table*/
        StringTable *stringTable;

        /*entries in the symbol table*/
        std::vector<SymbolTableEntry *> symbolTable;
        std::map<uint64_t, char *> symbolsFileOffset;

        /*the dynamic symbol table*/
        std::vector<DynamicSymbolTableEntry*> dynamicSymbolTable;
        bool DynamicSymbolTableComputed;


        /*dinmaic linker load command*/
        LoadDyLinkerCmd *loadDyLinkerCmd;

        /*entry point command*/
        LoadMainCmd loadMainCmd;
        bool loadMainCmdPresent;

        /*the uuid is a 128-bit unique random number that*/
        /*identifies an object produced by the static link editor*/
        uint8_t uuid[UUID_SIZE];

        /*Dynamicly linked shared libraries*/
        std::vector<LibraryInfo *> dynamicLibraries;

        /*information about where to find the table that includes
        the start of the functions*/
        LinkEditCmd functionStartsCmd;
        bool functionStartsCmdPresent;

        std::map<uint64_t, char *> functionsOffset;
        bool functionsOffsetComputed;

        std::vector<std::map<char *, char *, myKextComp> > kextsInfo;
        bool kextsInfoComputed;
        std::map<char *, MachHeader> kextsHeader;

        void computeSymbolsFileOffset();
        char *getFunctionName(uint64_t functionFileOffset);

	/* information about where to find signature */
	LinkEditCmd codeSignatureCmd;
	bool codeSignatureCmdPresent;

	/* superblob */
	SuperBlob superblob;
	bool isSuperBlobFetched;

	/* code directory blob */
	CodeDirectoryBlob codeDirectoryBlob;
	bool isCodeDirectoryBlobFetched;

	/* entitlements */
	Entitlements entitlements;
	bool isEntitlementsFetched;

public:
        MachO(char  *fileName);

        char *getFileName();


        MachHeader getHeader();

        std::vector<Segment *> getSegments();

        Segment *getSegmentByName(char *name);
        Section *getSectionByIndex(uint32_t index);
        Section *getSectionByName(char * segmentName, char *sectionName);

        SymbolTableHeader getSymbolTableHeader();

        StringTable *getStringTable();

        std::vector<SymbolTableEntry *> getSymbolTable();
        uint64_t getSymbolFileOffset(SymbolTableEntry *symbol);

        std::vector<DynamicSymbolTableEntry *> getDynamicSymbolTable();

        LoadDyLinkerCmd *getLoadDyLinkerCmd();

        uint8_t *getUUID();

        LoadMainCmd getLoadMainCmd();

        std::vector<LibraryInfo *> getDynamicLibrariesInfo();
        std::vector<char *> listDynamicLibraries();

        LinkEditCmd getFunctionStartsCmd();

        std::map<uint64_t, char *> getFunctionsOffset();

        std::vector<std::map<char *, char *, myKextComp> > getKextsInfo();

        std::vector<std::map<char *, char *, myKextComp> > getKextByProperty(char * key, char * value);

        std::map<char *, char *, myKextComp> getKextByBundleId(char * bundleId);

        uint64_t getVirtToFile(uint64_t virtualAddress);

        void dumpKext(char * bundleId, char *fileName);
        
	LinkEditCmd getCodeSignatureCmd();
	SuperBlob getSuperBlob();
	CodeDirectoryBlob getCodeDirectoryBlob();
	Entitlements getEntitlements();

        ~MachO();
};


#endif
