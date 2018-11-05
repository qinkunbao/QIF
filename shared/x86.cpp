#include "x86.h"

namespace tana {
    namespace x86{
        x86_insn insn_string2id(std::string insn_name)
        {
            x86_insn insn_id = X86_INS_INVALID;
            for(const auto& inst_map : insn_name_maps)
            {
                if (inst_map.name == insn_name)
                {
                    insn_id = inst_map.id;
                }
            }

            return insn_id;

        }

        x86_reg  reg_string2id(std::string reg_name)
        {
            x86_reg reg_id = X86_REG_INVALID;
            for(const auto& reg_map : reg_name_maps)
            {
                if (reg_map.name == reg_name)
                {
                    reg_id = reg_map.id;
                }
            }

            return reg_id;

        }

        std::string insn_id2string(const x86_insn id)
        {
            std::string x86_opc = "None";
            for(const auto& insn_map : insn_name_maps)
            {
                if (insn_map.id == id)
                {
                    x86_opc = insn_map.name;
                }
            }

            return x86_opc;
        }

        std::string reg_id2string(x86_reg id)
        {
            std::string x86_reg = "None";
            for(const auto& reg_map : reg_name_maps)
            {
                if(reg_map.id == id)
                {
                    x86_reg = reg_map.name;
                }
            }
            return x86_reg;
        }

        uint32_t get_reg_size(x86_reg reg_id)
        {
            int id = static_cast<int>(reg_id);
            uint8_t reg_size = regsize_map_32[id];
            return static_cast<uint32_t >(reg_size);
        }



    }
}