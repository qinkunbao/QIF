#include <iostream>
#include <elfio/elfio.hpp>

using namespace ELFIO;

int main(int argc, char** argv){

    if(argc != 2) {
        std::cout << "Usage: tutorial <elf_file>" << std::endl;
        return 0;
    }

    elfio reader;

    if(!reader.load(argv[1]))
    {
        std::cout << "Can't find or process ELF file" << argv[1] << std::endl;
        return 1;
    }

    std::cout << "ELF file class :";

    if(reader.get_class() == ELFCLASS32)
        std::cout << "ELF32" << std::endl;
    else
        std::cout << "ELF64" << std::endl;

    return 1;
}