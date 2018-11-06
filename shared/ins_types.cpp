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

    vcpu_ctx::vcpu_ctx():eflags(0)
    {
        for(int i = 0; i < GPR_NUM; ++i)
        {
            gpr[i] = 0;
        }
    };

    bool vcpu_ctx::CF()
    {
        assert(eflags>0);
        return static_cast<bool>(eflags & 0x1);
    }

    bool vcpu_ctx::PF()
    {
        assert(eflags>0);
        return static_cast<bool>(eflags & 0x4);
    }

    bool vcpu_ctx::AF()
    {
        assert(eflags>0);
        return static_cast<bool>(eflags & 0x10);
    }

    bool vcpu_ctx::ZF()
    {
        assert(eflags>0);
        return static_cast<bool>(eflags & 0x40);
    }

    bool vcpu_ctx::SF()
    {
        assert(eflags>0);
        return static_cast<bool>(eflags & 0x80);
    }

    bool vcpu_ctx::TF()
    {
        assert(eflags>0);
        return static_cast<bool>(eflags & 0x100);
    }

    bool vcpu_ctx::DF()
    {
        assert(eflags>0);
        return static_cast<bool>(eflags & 0x400);
    }

    bool vcpu_ctx::OF()
    {
        assert(eflags>0);
        return static_cast<bool>(eflags & 0x800);
    }

    Inst::Inst():id(0), addrn(0),
                 memory_address(0), instruction_id(x86::X86_INS_INVALID), 
                 oprd{nullptr, nullptr, nullptr}
    {
    }

    uint32_t Inst::get_operand_number() const {
        return this->oprs.size();
    }

    namespace instruction {




    }
}