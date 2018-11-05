#include <string>
#include "ins_types.h"

namespace tana {

    std::string Inst::get_opcode_operand() const {
        std::string instruction_operand;
        instruction_operand += x86::insn_id2string(instruction_id);
        instruction_operand += " ";
        for (const auto &opr: oprs) {
            instruction_operand += opr;
            instruction_operand += ",";
        }

        instruction_operand = instruction_operand.substr(0, instruction_operand.size() - 1);

        return instruction_operand;
    }

    Inst::Inst()
    {
        id = 0;
        addrn = 0;
        memory_address = 0;
        instruction_id = x86::X86_INS_INVALID;
        oprd[0] = nullptr;
        oprd[1] = nullptr;
        oprd[2] = nullptr;
        for(int i = 0; i < GPR_NUM; ++i)
        {
            vcpu.gpr[i] = 0;
        }
    }

    uint32_t Inst::get_operand_number() const {
        return this->oprs.size();
    }

    namespace instruction {
        OprID getOprID(std::string opcstr) {
            if (opcstr == "mov") return t_mov;
            if (opcstr == "add") return t_add;
            if (opcstr == "xor") return t_xor;
            if (opcstr == "push") return t_push;
            if (opcstr == "pop") return t_pop;
            if (opcstr == "sub") return t_sub;
            if (opcstr == "imul") return t_imul;
            if (opcstr == "or") return t_or;
            if (opcstr == "shl") return t_shl;
            if (opcstr == "shr") return t_shr;
            if (opcstr == "neg") return t_neg;
            if (opcstr == "not") return t_not;
            if (opcstr == "inc") return t_inc;
            if (opcstr == "lea") return t_lea;
            if (opcstr == "and") return t_and;
            if (opcstr == "div") return t_div;
            if (opcstr == "sar") return t_sar;
            if (opcstr == "rol") return t_rol;
            if (opcstr == "ror") return t_ror;
            if (opcstr == "dec") return t_dec;
            if (opcstr == "xchg") return t_xchg;
            if (opcstr == "cmovb") return t_cmovb;
            if (opcstr == "cmovz") return t_cmovz;
            if (opcstr == "movsx") return t_movsx;
            if (opcstr == "movzx") return t_movzx;
            if (opcstr == "shrd") return t_shrd;
            if (opcstr == "shld") return t_shld;
            if (opcstr == "sbb") return t_sbb;
            return t_none;
        }

        bool seNoEffect(std::string opcstr) {
            std::set<std::string> noeffectinst = {"test", "jmp", "jz", "jbe", "jo", "jno", "js", "jns", "je", "jne",
                                                  "jnz", "jb", "jnae", "jc", "jnb", "jae",
                                                  "jnc", "jna", "ja", "jnbe", "jl",
                                                  "jnge", "jge", "jnl", "jle", "jng", "jg",
                                                  "jnle", "jp", "jpe", "jnp", "jpo", "jcxz",
                                                  "jecxz", "cmp", "ret", "call", "rep", "nop", "data16", "int", "lock"};
            if (noeffectinst.find(opcstr) != noeffectinst.end()) return true;
            return false;
        }


        bool SymbolicExecutionNoEffect(x86::x86_insn insn) {
            using namespace x86;
            std::set<x86::x86_insn> no_effect_inst{X86_INS_TEST, X86_INS_JMP, X86_INS_CMP, X86_INS_CALL, X86_INS_RET,
                                                   X86_INS_NOP, X86_INS_INT, X86_INS_JA, X86_INS_JAE, X86_INS_JAE,
                                                   X86_INS_JB, X86_INS_JBE, X86_INS_JCXZ, X86_INS_JE, X86_INS_JECXZ,
                                                   X86_INS_JG, X86_INS_JGE, X86_INS_JS, X86_INS_JNE, X86_INS_JNO,
                                                   X86_INS_JNP, X86_INS_JNS};
            if (no_effect_inst.find(insn) != no_effect_inst.end())
                return true;
            else
                return false;
        }

        bool isSysCall(std::string opcstr) {
            std::set<std::string> syscallinst = {"int", "sysenter"};
            if (syscallinst.find(opcstr) != syscallinst.end()) return true;
            return false;
        }

        bool isExchangeable(std::string opcstr) {
            std::set<std::string> commutableinst = {"add", "xor", "and"};
            if (commutableinst.find(opcstr) != commutableinst.end()) return true;
            return false;
        }
    }
}