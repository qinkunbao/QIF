#include <string>
#include "Trace2ELF.h"

using namespace tana;
using namespace std;

Trace2ELF::Trace2ELF(const std::string &obj_name, const std::string &function_file_name)
{
    string debug;
    string objdump_cmd = "objdump --dwarf=decodedline " + obj_name;

    string objdump_result = cmd::exec(objdump_cmd.c_str());

    debug_info = std::make_unique<DebugInfo>(objdump_result);

    func = std::make_unique<Function>(function_file_name);

    elf_sym_info = std::make_unique<ELF::ELF_Symbols>(obj_name);

    int offset1 = 1, offset2 = 2;
    while(true)
    {
        auto fun = func->pickOneRandomElement();
        string func_name = fun->rtn_name;
        auto sym_elf = elf_sym_info->findSymbol(func_name);

        if(sym_elf != nullptr)
        {
            offset1 = fun->start_addr - sym_elf->value;
            break;
        }

    }

    while(true)
    {
        auto fun = func->pickOneRandomElement();
        string func_name = fun->rtn_name;
        auto sym_elf = elf_sym_info->findSymbol(func_name);

        if(sym_elf != nullptr)
        {
            offset2 = fun->start_addr - sym_elf->value;
            break;
        }

    }

    assert(offset1 == offset2);
    offset_from_trace_to_elf = offset1;

}


std::shared_ptr<DebugSymbol> Trace2ELF::locateSym(uint32_t mem_addr)
{
    uint32_t elf_addr = mem_addr - offset_from_trace_to_elf;

    return debug_info->locateSym(elf_addr);
}