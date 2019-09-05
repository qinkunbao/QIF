/*************************************************************************
	> File Name: ELFInfo.cpp
	> Author: Qinkun Bao
	> Mail: qinkunbao@gmail.com
	> Created Time: Thu Sep  5 13:08:04 EDT 2019
 ************************************************************************/


#include <iostream>
#include <memory>
#include <cassert>
#include <elfio/elfio.hpp>
#include "ELFInfo.h"
#include "error.h"


using namespace std;
using namespace tana::ELF;
using namespace ELFIO;


#define ERROR(MESSAGE) tana::default_error_handler(__FILE__, __LINE__, MESSAGE)


ELF_Symbols::ELF_Symbols(const std::string &filePWD)
{

    elfio reader;

    if(!reader.load(filePWD))
    {
        std::cout << "Can't find or process ELF file " << filePWD << std::endl;
        ERROR("Wrong ELF format");
        exit(0);
    }

    assert(reader.get_class() == ELFCLASS32);
    Elf_Half sec_num = reader.sections.size();

    for ( int i = 0; i < sec_num; ++i )
    {
        section* psec = reader.sections[i];
        // Check section type
        if ( psec->get_type() == SHT_SYMTAB ) {
            const symbol_section_accessor symbols( reader, psec );
            for ( unsigned long j = 0; j < symbols.get_symbols_num(); ++j ) {
                std::string   name;
                Elf64_Addr    value;
                Elf_Xword     size;
                unsigned char bind;
                unsigned char type;
                Elf_Half      section_index;
                unsigned char other;

                // Read symbol properties
                symbols.get_symbol( j, name, value, size, bind,
                                    type, section_index, other );

                auto sym = std::make_shared<Symbol>(name, value, size, bind,
                                                    type, section_index, other);
                elf_s.push_back(sym);
            }
        }
    }

}


std::string ELF_Symbols::findSymName(uint32_t addr)
{
    for(const auto& s : elf_s)
    {
        if(s->value == addr){
            return s->name;
        }
    }

    return "Not Found";
}


uint32_t ELF_Symbols::findSymAddr(const std::string &name)
{
    for(const auto& s : elf_s)
    {
        if(s->name == name){
            return s->value;
        }
    }

    return 0;
}


std::shared_ptr<Symbol> ELF_Symbols::findSymbol(uint32_t addr)
{
    for(auto &s : elf_s)
    {
        if(s->value == addr)
        {
            return s;
        }
    }

    return nullptr;
}

std::shared_ptr<Symbol> ELF_Symbols::findSymbol(const std::string &name)
{
    for(auto &s : elf_s)
    {
        if(s->name == name)
        {
            return s;
        }
    }

    return nullptr;
}