#include <stdio.h>
#include <stdint.h>
#include <vector>
#include <iostream>
#include "MachO.hpp"
#include "FileReader.hpp"
#include "UniversalBinary.hpp"


void print_header(UniversalBinary &binary)
{
        if (binary.getIsUniversal()) {
            printf("Universal Binary\n");

            printf("Fat Header\n");
            printf(" ");
            binary.getFatHeader()->print();
            printf("\n");

            printf("Fat Architectures:\n");
            std::vector<FatArchitecture *> fatArches = binary.getFatArches();
            for (int i = 0; i < fatArches.size(); i++) {
                printf(" ");
                fatArches[i]->print();
                printf("\n");
            }

            printf("Architecture Headers:\n");
            std::vector<MachO *> machOs = binary.getMachOs();
            for (int i = 0; i < machOs.size(); i++) {
                    printf("Architecture %d header:\n", i);
                    machOs[i]->getHeader().print();
                    printf("\n");
            }
        } else {
            MachO *bin = binary.getMachOs()[0];
            printf("Single MachO, Not Universal\n");
            if (bin->getHeader().getIs32())
                printf("MACH-O 32\n");
            else
                printf("MACH-O 64\n");

            printf(" ");
            bin->getHeader().print();
            printf("\n");
        }
}
void print_section(Section *section)
{
        printf("Segment name: %s\nSection name %s\n",
        section->getSegmentName(), section->getSectionName());

        printf("virtual address: %llu\nsize: %llu\n",
        section->getVirtualAddress(), section->getSize());

        printf("offset: %x\nnumber of relocations: %d\nalign: %d\n" ,
        section->getOffset(), section->getNumberRelocations(), section->getAlign());

        printf("relocation offset: %d\nflags: %u\n",
        section->getRelocationOffset(), section->getFlags());

        printf("reserved1: %d\nreserved2: %d\n",
        section->getReserved1(), section->getReserved2());
}

void print_segment(Segment *segment)
{
        printf("Segment Name: %s\n", segment->getName());

        printf("virtual address: %lld\nvirtual size: %lld\n",
                segment->getVirtualAddress(), segment->getVirtualSize());

        printf("file offset: %llx\nfile size: %lld\n",
                segment->getFileOffset(), segment->getFileSize() );

        printf("init prot: 0x%x\nmax prot: %x\n",
                segment->getInitProtection(), segment->getMaxProtection());

        printf("flags: %x\nnumber of sections: %d\n",
                segment->getFlags(), segment->getNumberSections());

        std::vector<Section *> sections = segment->getSections();
        for(uint32_t i = 0; i < segment->getNumberSections(); i++)
                print_section(sections[i]);
}

void print_symbol_header(SymbolTableHeader header)
{
        printf("tableOffset: %u\nnumber symbols: %u\n",
        header.getTableOffset(), header.getNumberofSymbols());

        printf("string table offset: %u\nstring table size: %u\n",
        header.getStringTableOffset(), header.getStringTableSize());
}

void print_symbol(SymbolTableEntry *entry)
{
        printf("string table index: %u\n", entry->getStringTableIndex());

        printf("symbol name: %s\n", entry->getName());

        printf("type: 0x%x Debug:%d PrivateExternal:%d External:%d\nUndefined:%d Absolute: %d DefinSection: %d Prebound:%d Indirect %d\n",
        entry->getType(), entry->isDebug(), entry->isPrivateExternal(), entry->isExternal(),
        entry->isUndefined(), entry->isAbsolute(), entry->isDefinedInSection(),
        entry->isPrebound(), entry->isIndirect());

        printf("section index: %u\n", entry->getSectionIndex());

        printf("description: %u\n", entry->getDescription());
        if(entry->isUndefined())
        printf("library ordinal: %d\n", entry->getLibraryOrdinal());

        printf("value: 0x%llx\n", entry->getValue());
}
void print_lib(LibraryInfo * lib)
{
        printf("version:%u\n name: %s\n", lib->getCurrentVersion(), lib->getName());

        printf("timestamp: %u\n Comp: %u\n", lib->getTimestamp(), lib->getCompatibilityVersion());
}
/*listing of segments and sections*/
int main(int argc, char *argv[])
{

       // int option;

        if (argc != 3) {
                printf("Usage: %s <binary> <option>\n", argv[0]);
                return -1;
        }
        //sscanf(argv[2], "%d", &option);


        UniversalBinary bin(argv[1]);

        if (strstr(argv[2], "head")) {
                print_header(bin);
        }

        if(strstr(argv[2], "seglc")) {
                std::vector<MachO *> machOs = bin.getMachOs();

                for (int m = 0; m < machOs.size(); m++) {
                        printf("Segments of architecture number %d:\n", m);

                        std::vector<Segment *> segments = machOs[m]->getSegments();

                        for(uint32_t i = 0; i < segments.size(); i++) {
                                print_segment(segments[i]);
                        }
                }
        }

        if(strstr(argv[2], "symlc")) {
                std::vector<MachO *> machOs = bin.getMachOs();

                for (int m = 0; m < machOs.size(); m++) {
                        printf("Symbol table header of architecture number %d:\n", m);

                        SymbolTableHeader symbolTableHeader = machOs[m]->getSymbolTableHeader();
                        print_symbol_header(symbolTableHeader);
                }
        }

        if(strstr(argv[2], "strtab")) {
                std::vector<MachO *> machOs = bin.getMachOs();

                for (int m = 0; m < machOs.size(); m++) {
                        printf("String table of architecture number %d:\n", m);

                        StringTable *stringTable = machOs[m]->getStringTable();

                        for(uint32_t i = 0; i < stringTable->getNumberOfStrings(); i++) {
                                printf("%d---%s\n", i,  stringTable->get(i) );
                        }
                }
        }

        if(strstr(argv[2], "symtab")) {
                std::vector<MachO *> machOs = bin.getMachOs();

                for (int m = 0; m < machOs.size(); m++) {
                        printf("\n\nSymbol table of architecture number %d:\n", m);

                        std::vector<SymbolTableEntry *> symbolTable = machOs[m]->getSymbolTable();
                        for(uint32_t i = 0; i < symbolTable.size(); i++) {
                                print_symbol(symbolTable[i]);
                                printf("\n\n");
                        }
                        break;
                }
        }

        if(strstr(argv[2], "misc")) {
                std::vector<MachO *> machOs = bin.getMachOs();

                for (int m = 0; m < machOs.size(); m++) {
                        printf("Misc for architecture number %d:\n", m);

                        LoadDyLinkerCmd *cmd = machOs[m]->getLoadDyLinkerCmd();
                        if(cmd != NULL)
                                printf(" the linker name is %s\n", cmd->getLinkerName() );

                        uint8_t *uuid = machOs[m]->getUUID();
                        printf("the uuid is:\n");
                        for(int i = 0; i < 16; i++) {
                                printf("%x", uuid[i]);
                                if((i + 1) % 4 == 0)
                                        printf("-");
                        }

                        LoadMainCmd mainCmd = machOs[m]->getLoadMainCmd();

                        printf("\nthe main command\n");
                        printf ("entryOffset: %llu, stacksize: %llu\n", mainCmd.getEntryOffset(),
                                mainCmd.getStackSize());
                }
        }

        if(strstr(argv[2], "liblc")) {
                std::vector<MachO *> machOs = bin.getMachOs();

                for (int m = 0; m < machOs.size(); m++) {
                        printf("Dynamic libraries of architecture number %d:\n", m);

                        std::vector<char *> names = machOs[m]->listDynamicLibraries();

                        for(uint32_t i = 0; i < names.size(); i++)
                                printf("%s\n", names[i]);
                }
        }

        if(strstr(argv[2], "fc_starts")) {
                std::vector<MachO *> machOs = bin.getMachOs();

                for (int m = 0; m < machOs.size(); m++) {
                        printf("Functions of architecture number %d:\n", m);

                        LinkEditCmd fcstart = machOs[m]->getFunctionStartsCmd();

                        printf("Function starts Cmd ------------\n");
                        printf("dataOffset: %u\ndataSize: %u\n", fcstart.getDataOffset(),
                        fcstart.getDataSize());

                        std::map<uint64_t, char *> starts = machOs[m]->getFunctionsOffset();
                        std::map<uint64_t, char *>::iterator it;

                        for(it = starts.begin(); it != starts.end(); ++it)
                                printf("0x%llx ----- %s\n", it->first, it->second);
                }
        }

        if(strstr(argv[2], "dis_all")) {
                std::vector<MachO *> machOs = bin.getMachOs();

                for (int m = 0; m < machOs.size(); m++) {
                        printf("Disassembly of architecture number %d:\n", m);

                        printf("1\n");
                        FileReader fileReader(machOs[m]);
                        printf("2\n");
                        fileReader.Disassemble();
                }
        }

        if(strstr(argv[2], "dis_main")) {
                std::vector<MachO *> machOs = bin.getMachOs();

                for (int m = 0; m < machOs.size(); m++) {
                    FileReader fileReader(machOs[m]);
                    fileReader.Disassemble((char *)"_main");
                }
        }

        if(strstr(argv[2], "dis_offset")) {
                std::vector<MachO *> machOs = bin.getMachOs();

                for (int m = 0; m < machOs.size(); m++) {
                    FileReader fileReader(machOs[m]);
                    fileReader.Disassemble(7030);
                }
        }

        //TODO: test with fat binary, does a fat binary make sense in this context
        if(strstr(argv[2], "kexts")) {
                std::vector<MachO *> machOs = bin.getMachOs();

                for (int m = 0; m < machOs.size(); m++) {
                    std::vector<std::map<char *, char *, myKextComp> > map = machOs[m]->getKextsInfo();
                    std::map<char *, char *, myKextComp>::iterator it;
                    for (uint32_t i = 0; i < map.size(); i++) {
                            printf("kext %d\n", i);
                            for (it = map[i].begin(); it != map[i].end(); ++it) {
                                    printf("%s -- %s\n", it->first, it->second);
                            }
                    }
                    printf("got %lu kernel extensions\n", map.size() );
                }
        }

        //TODO: test with fat binary, does a fat binary make sense in this context
        if(strstr(argv[2], "find_kext")) {
                std::vector<MachO *> machOs = bin.getMachOs();

                for (int m = 0; m < machOs.size(); m++) {
                    std::map<char *, char *, myKextComp> map2;
                    map2 = machOs[m]->getKextByBundleId((char *)"com.apple.kpi.mach");
                    std::map<char *, char *, myKextComp>::iterator it;
                    for (it = map2.begin(); it != map2.end(); ++it) {
                            printf("%s --- %s\n", it->first, it->second);
                    }
                }
        }

        //TODO: test with fat binary, does a fat binary make sense in this context
        if(strstr(argv[2], "dump_section")) {
                std::vector<MachO *> machOs = bin.getMachOs();

                for (int m = 0; m < machOs.size(); m++) {
                    FileReader fileReader(machOs[m]);
                    uint64_t size;
                    char * raw = fileReader.dumpSection((char *)"__TEXT", (char *)"__cstring", &size);
                    for (uint64_t i = 0; i < size; i++)
                            printf("%c", raw[i]);
                    delete raw;
                }
        }

        //TODO: test with fat binary, does a fat binary make sense in this context
        if (strstr(argv[2], "dump_kext")) {
                std::vector<MachO *> machOs = bin.getMachOs();

                for (int m = 0; m < machOs.size(); m++) {
                    machOs[m]->dumpKext((char *)"com.apple.iokit.IOTimeSyncFamily", (char *) "kpi");
                }
        }

        if (strstr(argv[2], "indsymtab")) {
                std::vector<MachO *> machOs = bin.getMachOs();

                for (int m = 0; m < machOs.size(); m++) {
                    std::vector<DynamicSymbolTableEntry *> index = machOs[m]->getDynamicSymbolTable();
                    for (uint32_t  i = 0; i < index.size(); i++) {
                            printf("%u---", index[i]->getIndex());
                            printf("%llx --- ", index[i]->getIndirectAdress() );
                            printf("%s\n", index[i]->getName());
                    }
                }
        }

    //TODO: test with fat binary
	if (strstr(argv[2], "siglc")) {
        std::vector<MachO *> machOs = bin.getMachOs();

        for (int m = 0; m < machOs.size(); m++) {
            LinkEditCmd codeSignatureCmd = machOs[m]->getCodeSignatureCmd();
            puts("LC_CODE_SIGNATURE");
            printf("Offset: %d, Size: %d\n", codeSignatureCmd.getDataOffset(),
                codeSignatureCmd.getDataSize());

            puts("---------------------------");

            SuperBlob sb = machOs[m]->getSuperBlob();
            puts("SuperBlob");
            printf("Length: %d, NumBlobs: %d\n", sb.getLength(), sb.getNumBlobs());

            std::vector<struct subblob> sbs = sb.getSubBlobs();
            for (unsigned int i = 0; i < sb.getNumBlobs(); i++) {
                printf("\tType: %d, Offset: %d\n", sbs[i].type, sbs[i].offset);
            }
        }
	}

    //TODO: test with fat binary
	if (strstr(argv[2], "ent")) {
        std::vector<MachO *> machOs = bin.getMachOs();

        for (int m = 0; m < machOs.size(); m++) {
            Entitlements ent = machOs[m]->getEntitlements();
            puts("Entitlements");
            puts("------------");
            std::cout<<ent.getXml()<<"\n";
        }
	}

    //TODO: test with fat binary
	if (strstr(argv[2], "cdb")) {
        std::vector<MachO *> machOs = bin.getMachOs();

        for (int m = 0; m < machOs.size(); m++) {
            CodeDirectoryBlob cdb = machOs[m]->getCodeDirectoryBlob();
            puts("Code Directory Blob");
            puts("------------");
            printf("length: %d\n", cdb.getLength());
            printf("version: %#08X\n", cdb.getVersion());
            printf("flags: %#08X\n", cdb.getFlags());
            printf("hashOffset: %d\n", cdb.getHashOffset());
            printf("identOffset: %d\n", cdb.getIdentOffset());
            printf("nSpecialSlots: %d\n", cdb.getNSpecialSlots());
            printf("nCodeSlots: %d\n", cdb.getNCodeSlots());
            printf("codeLimit: %d\n", cdb.getCodeLimit());
            printf("hashType: %d\n", cdb.getHashType());
            printf("hashSize: %d\n", cdb.getHashSize());
            printf("pageSize: %d\n", cdb.getPageSize());

            uint32_t nSpecialSlots = cdb.getNSpecialSlots();
            uint32_t nCodeSlots = cdb.getNCodeSlots();
            uint32_t hashSize = cdb.getHashSize();
            std::vector<char*> hashes = cdb.getHashes();
            for (uint32_t i = 0; i < nSpecialSlots; i++) {
                printf("%d: ", -nSpecialSlots + i);
                for (uint32_t j = 0; j < hashSize; j++)
                    printf("%02hhx", hashes[i][j]);
                puts("");
            }
            for (uint32_t i = nSpecialSlots; i < nSpecialSlots + nCodeSlots; i++) {
                printf("%d: ", i - nSpecialSlots);
                for (uint32_t j = 0; j < hashSize; j++)
                    printf("%02hhx", hashes[i][j]);
                puts("");
            }
        }
	}

    if (strstr(argv[2], "validate")) {
        std::vector<MachO *> machOs = bin.getMachOs();

        for (int m = 0; m < machOs.size(); m++)
            if (machOs[m]->areSignaturesValid())
                printf("Signature of architecture %d is valid.\n", m);
            else
                printf("Signature of architecture %d is NOT valid.\n", m);
    }
        return 0;
}
