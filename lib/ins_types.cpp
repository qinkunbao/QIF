#include <string>
#include "ins_types.h"
#include "ins_parser.h"

namespace tana {

    std::string Inst_Dyn::get_opcode_operand() const {
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

    Inst_Dyn::Inst_Dyn(): memory_address(0)
    {
    }

    Inst_Base::Inst_Base():id(0), addrn(0),
                         instruction_id(x86::X86_INS_INVALID),
                         oprd{nullptr, nullptr, nullptr}
    {
    }

    Inst_Static::Inst_Static()
    {
    }

    uint32_t Inst_Base::get_operand_number() const {
        return static_cast<uint32_t >(this->oprs.size());
    }


    void Inst_Dyn::parseOperand()
    {
        for (uint32_t i = 0; i < this->get_operand_number(); ++i)
        {
            this->oprd[i] = createOperand(this->oprs[i], this->addrn);
        }
    }

    void Inst_Static::parseOperand()
    {
        for (uint32_t i = 0; i < this->get_operand_number(); ++i)
        {
            this->oprd[i] = createOperand(this->oprs[i], this->addrn);
        }
    }

}