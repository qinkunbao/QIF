/*************************************************************************
	> File Name: x86.cpp
	> Author: Qinkun Bao
	> Mail: qinkunbao@gmail.com
	> Created Time: Mon Apr 22 21:56:51 2019
 ************************************************************************/

#include <set>
#include "x86.h"
#include "error.h"
#define ERROR(MESSAGE) tana::default_error_handler(__FILE__, __LINE__, MESSAGE)



namespace tana {
    namespace x86 {
        x86_insn insn_string2id(std::string insn_name) {
            x86_insn insn_id = X86_INS_INVALID;
            for (const auto &inst_map : insn_name_maps) {
                if (inst_map.name == insn_name) {
                    insn_id = inst_map.id;
                }
            }
            if(insn_id == X86_INS_INVALID) {
                ERROR(insn_name.c_str());
            }
            return insn_id;

        }

        x86_reg reg_string2id(std::string reg_name) {
            x86_reg reg_id = X86_REG_INVALID;
            for (const auto &reg_map : reg_name_maps) {
                if (reg_map.name == reg_name) {
                    reg_id = reg_map.id;
                }
            }

            return reg_id;

        }

        std::string insn_id2string(const x86_insn id) {
            std::string x86_opc = "None";
            for (const auto &insn_map : insn_name_maps) {
                if (insn_map.id == id) {
                    x86_opc = insn_map.name;
                }
            }

            return x86_opc;
        }

        std::string reg_id2string(x86_reg id) {
            std::string x86_reg;
            for (const auto &reg_map : reg_name_maps) {
                if (reg_map.id == id) {
                    x86_reg = reg_map.name;
                }
            }
            return x86_reg;
        }

        uint32_t get_reg_size(x86_reg reg_id) {
            int id = static_cast<int>(reg_id);
            uint8_t reg_size = regsize_map_32[id];
            return static_cast<uint32_t >(reg_size);
        }


        bool SymbolicExecutionNoEffect(x86::x86_insn insn) {
            using namespace x86;
            std::set<x86::x86_insn> no_effect_inst{X86_INS_TEST, X86_INS_JMP, X86_INS_CMP,
                                                   X86_INS_NOP, X86_INS_INT, X86_INS_JA, X86_INS_JAE, X86_INS_JAE,
                                                   X86_INS_JB, X86_INS_JBE, X86_INS_JCXZ, X86_INS_JE, X86_INS_JECXZ,
                                                   X86_INS_JG, X86_INS_JGE, X86_INS_JS, X86_INS_JNE, X86_INS_JNO,
                                                   X86_INS_JNP, X86_INS_JNS, X86_INS_JZ, X86_INS_JNZ, X86_INS_INVALID,
                                                   X86_INS_DATA16, X86_INS_JLE, X86_INS_JL};
            const bool found = no_effect_inst.find(insn) != no_effect_inst.end();

            if (found)
                return true;
            else
                return false;
        }
    }
}