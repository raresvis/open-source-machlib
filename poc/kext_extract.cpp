#include <stdio.h>
#include <stdint.h>
#include <vector>
#include <iostream>
#include "MachO.hpp"
#include "FileReader.hpp"

int main(int argc, char *argv[])
{

    if (argc < 3) {
        printf("./kext_extract <kext_name> <kernel_path> [<output_dir>]\n");
        return -1;
    }

    MachO bin(argv[2]);
    std::string output_filename;

    if (argc == 3)
        output_filename = std::string("./" + std::string(argv[1]) + ".kext");
    else
        output_filename = std::string(std::string(argv[3]) + "/" + std::string(argv[1]) + ".kext");
    std::cout<<output_filename<<std::endl;

    bin.dumpKext((char *) argv[1], (char *) output_filename.c_str());

    return 0;
}
