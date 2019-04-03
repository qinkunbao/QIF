#include <string>
#include <memory>
#include "ins_types.h"
#include "ins_parser.h"

namespace tana {

    std::string Inst_Base::get_opcode_operand() const {
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

    void Inst_Base::print() const
    {
        std::stringstream inst;
        inst << std::hex << this->addrn << std::dec;
        inst << "   ";
        inst << this->get_opcode_operand() << " ";
        std::cout << inst.str() << std::endl;
    }

    vcpu_ctx::vcpu_ctx():eflags(0), eflags_state(false)
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
            this->oprd[i] = createOperandStatic(this->oprs[i], this->addrn);
        }
    }

    std::unique_ptr<Inst_Dyn> Inst_Dyn_Factory::makeInst(tana::x86::x86_insn id)
    {

        if(id == x86::x86_insn::X86_INS_PUSH)
            return std::make_unique<Dyn_X86_INS_PUSH>();

        if(id == x86::x86_insn::X86_INS_POP)
            return std::make_unique<Dyn_X86_INS_POP>();

        if(id == x86::x86_insn::X86_INS_NEG)
            return std::make_unique<Dyn_X86_INS_NEG>();

        if(id == x86::x86_insn::X86_INS_NOT)
            return std::make_unique<Dyn_X86_INS_NOT>();

        if(id == x86::x86_insn::X86_INS_INC)
            return std::make_unique<Dyn_X86_INS_INC>();

        if(id == x86::x86_insn::X86_INS_DEC)
            return std::make_unique<Dyn_X86_INS_DEC>();

        if(id == x86::x86_insn::X86_INS_MOVZX)
            return std::make_unique<Dyn_X86_INS_MOVZX>();

        if(id == x86::x86_insn::X86_INS_MOVSX)
            return std::make_unique<Dyn_X86_INS_MOVSX>();

        if(id == x86::x86_insn::X86_INS_CMOVB)
            return std::make_unique<Dyn_X86_INS_CMOVB>();

        if(id == x86::x86_insn::X86_INS_MOV)
            return std::make_unique<Dyn_X86_INS_MOV>();

        if(id == x86::x86_insn::X86_INS_LEA)
            return std::make_unique<Dyn_X86_INS_LEA>();

        if(id == x86::x86_insn::X86_INS_XCHG)
            return std::make_unique<Dyn_X86_INS_XCHG>();

        if(id == x86::x86_insn::X86_INS_SBB)
            return std::make_unique<Dyn_X86_INS_SBB>();

        if(id == x86::x86_insn::X86_INS_IMUL)
            return std::make_unique<Dyn_X86_INS_IMUL>();

        if(id == x86::x86_insn::X86_INS_SHLD)
            return std::make_unique<Dyn_X86_INS_SHLD>();

        if(id == x86::x86_insn::X86_INS_SHRD)
            return std::make_unique<Dyn_X86_INS_SHRD>();

        if(id == x86::x86_insn::X86_INS_ADD)
            return std::make_unique<Dyn_X86_INS_ADD>();

        if(id == x86::x86_insn::X86_INS_SUB)
            return std::make_unique<Dyn_X86_INS_SUB>();

        if(id == x86::x86_insn::X86_INS_AND)
            return std::make_unique<Dyn_X86_INS_AND>();

        if(id == x86::x86_insn::X86_INS_ADC)
            return std::make_unique<Dyn_X86_INS_ADC>();

        if(id == x86::x86_insn::X86_INS_ROR)
            return std::make_unique<Dyn_X86_INS_ROR>();

        if(id == x86::x86_insn::X86_INS_ROL)
            return std::make_unique<Dyn_X86_INS_ROL>();

        if(id == x86::x86_insn::X86_INS_OR)
            return std::make_unique<Dyn_X86_INS_OR>();

        if(id == x86::x86_insn::X86_INS_XOR)
            return std::make_unique<Dyn_X86_INS_XOR>();

        if(id == x86::x86_insn::X86_INS_SHL)
            return std::make_unique<Dyn_X86_INS_SHL>();

        if(id == x86::x86_insn::X86_INS_SHR)
            return std::make_unique<Dyn_X86_INS_SHR>();

        if(id == x86::x86_insn::X86_INS_SAR)
            return std::make_unique<Dyn_X86_INS_SAR>();

        if(id == x86::x86_insn::X86_INS_CALL)
            return std::make_unique<Dyn_X86_INS_CALL>();

        if(id == x86::x86_insn::X86_INS_RET)
            return std::make_unique<Dyn_X86_INS_RET>();

        if(id == x86::x86_insn::X86_INS_LEAVE)
            return std::make_unique<Dyn_X86_INS_LEAVE>();

        if(id == x86::x86_insn::X86_INS_ENTER)
            return std::make_unique<Dyn_X86_INS_ENTER>();

        return std::make_unique<Inst_Dyn>();
    }



}