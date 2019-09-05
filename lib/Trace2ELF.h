#include <memory>
#include "cmd.h"
#include "DebugInfo.h"
#include "ELFInfo.h"
#include "Function.h"


namespace tana {

    class Trace2ELF {
    private:
        std::unique_ptr<DebugInfo> debug_info;
        std::unique_ptr<ELF::ELF_Symbols> elf_sym_info;
        std::unique_ptr<Function> func;
        int offset_from_trace_to_elf;

    public:
        explicit Trace2ELF(const std::string &obj_name, const std::string &function_file_name);
        std::shared_ptr<DebugSymbol> locateSym(uint32_t mem_addr);
    };

}