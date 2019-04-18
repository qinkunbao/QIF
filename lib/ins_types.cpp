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
        return static_cast<bool>(eflags & 0x1u);
    }

    bool vcpu_ctx::PF()
    {
        assert(eflags>0);
        return static_cast<bool>(eflags & 0x4u);
    }

    bool vcpu_ctx::AF()
    {
        assert(eflags>0);
        return static_cast<bool>(eflags & 0x10u);
    }

    bool vcpu_ctx::ZF()
    {
        assert(eflags>0);
        return static_cast<bool>(eflags & 0x40u);
    }

    bool vcpu_ctx::SF()
    {
        assert(eflags>0);
        return static_cast<bool>(eflags & 0x80u);
    }

    bool vcpu_ctx::TF()
    {
        assert(eflags>0);
        return static_cast<bool>(eflags & 0x100u);
    }

    bool vcpu_ctx::DF()
    {
        assert(eflags>0);
        return static_cast<bool>(eflags & 0x400u);
    }

    bool vcpu_ctx::OF()
    {
        assert(eflags>0);
        return static_cast<bool>(eflags & 0x800u);
    }


    Inst_Base::Inst_Base(bool inst_type):id(0), addrn(0),
                         instruction_id(x86::X86_INS_INVALID),
                         oprd{nullptr, nullptr, nullptr},memory_address(0), is_static(inst_type)
    {
    }


    uint32_t Inst_Base::get_operand_number() const {
        return static_cast<uint32_t >(this->oprs.size());
    }


    void Inst_Base::parseOperand()
    {
        if(is_static)
        {
            for (uint32_t i = 0; i < this->get_operand_number(); ++i)
            {
                this->oprd[i] = createOperandStatic(this->oprs[i], this->addrn);
            }
        }

        else{
            for (uint32_t i = 0; i < this->get_operand_number(); ++i)
            {
                this->oprd[i] = createOperand(this->oprs[i], this->addrn);
            }
        }
    }

    std::string Inst_Base::get_memory_address()
    {
        if(!is_static)
        {
            std::stringstream sstream;
            sstream << std::hex << memory_address << std::dec;
            std::string result = sstream.str();
            return result;
        }


        else
        {
            std::string reg_name;
            uint32_t num_opd = this->get_operand_number();
            for(uint32_t count = 0; count < num_opd; ++ count)
            {
                reg_name = oprs[count];
                if(this->oprd[count]->type == Operand::Mem)
                    return oprs[count];
            }
            return reg_name;
        }
    }


    std::unique_ptr<Inst_Base> Inst_Dyn_Factory::makeInst(tana::x86::x86_insn id, bool isStatic)
    {

        if(id == x86::x86_insn::X86_INS_PUSH)
            return std::make_unique<Dyn_X86_INS_PUSH>(isStatic);

        if(id == x86::x86_insn::X86_INS_POP)
            return std::make_unique<Dyn_X86_INS_POP>(isStatic);

        if(id == x86::x86_insn::X86_INS_NEG)
            return std::make_unique<Dyn_X86_INS_NEG>(isStatic);

        if(id == x86::x86_insn::X86_INS_NOT)
            return std::make_unique<Dyn_X86_INS_NOT>(isStatic);

        if(id == x86::x86_insn::X86_INS_INC)
            return std::make_unique<Dyn_X86_INS_INC>(isStatic);

        if(id == x86::x86_insn::X86_INS_DEC)
            return std::make_unique<Dyn_X86_INS_DEC>(isStatic);

        if(id == x86::x86_insn::X86_INS_MOVZX)
            return std::make_unique<Dyn_X86_INS_MOVZX>(isStatic);

        if(id == x86::x86_insn::X86_INS_MOVSX)
            return std::make_unique<Dyn_X86_INS_MOVSX>(isStatic);

        if(id == x86::x86_insn::X86_INS_CMOVB)
            return std::make_unique<Dyn_X86_INS_CMOVB>(isStatic);

        if(id == x86::x86_insn::X86_INS_MOV)
            return std::make_unique<Dyn_X86_INS_MOV>(isStatic);

        if(id == x86::x86_insn::X86_INS_LEA)
            return std::make_unique<Dyn_X86_INS_LEA>(isStatic);

        if(id == x86::x86_insn::X86_INS_XCHG)
            return std::make_unique<Dyn_X86_INS_XCHG>(isStatic);

        if(id == x86::x86_insn::X86_INS_SBB)
            return std::make_unique<Dyn_X86_INS_SBB>(isStatic);

        if(id == x86::x86_insn::X86_INS_IMUL)
            return std::make_unique<Dyn_X86_INS_IMUL>(isStatic);

        if(id == x86::x86_insn::X86_INS_SHLD)
            return std::make_unique<Dyn_X86_INS_SHLD>(isStatic);

        if(id == x86::x86_insn::X86_INS_SHRD)
            return std::make_unique<Dyn_X86_INS_SHRD>(isStatic);

        if(id == x86::x86_insn::X86_INS_ADD)
            return std::make_unique<Dyn_X86_INS_ADD>(isStatic);

        if(id == x86::x86_insn::X86_INS_SUB)
            return std::make_unique<Dyn_X86_INS_SUB>(isStatic);

        if(id == x86::x86_insn::X86_INS_AND)
            return std::make_unique<Dyn_X86_INS_AND>(isStatic);

        if(id == x86::x86_insn::X86_INS_ADC)
            return std::make_unique<Dyn_X86_INS_ADC>(isStatic);

        if(id == x86::x86_insn::X86_INS_ROR)
            return std::make_unique<Dyn_X86_INS_ROR>(isStatic);

        if(id == x86::x86_insn::X86_INS_ROL)
            return std::make_unique<Dyn_X86_INS_ROL>(isStatic);

        if(id == x86::x86_insn::X86_INS_OR)
            return std::make_unique<Dyn_X86_INS_OR>(isStatic);

        if(id == x86::x86_insn::X86_INS_XOR)
            return std::make_unique<Dyn_X86_INS_XOR>(isStatic);

        if(id == x86::x86_insn::X86_INS_SHL)
            return std::make_unique<Dyn_X86_INS_SHL>(isStatic);

        if(id == x86::x86_insn::X86_INS_SHR)
            return std::make_unique<Dyn_X86_INS_SHR>(isStatic);

        if(id == x86::x86_insn::X86_INS_SAR)
            return std::make_unique<Dyn_X86_INS_SAR>(isStatic);

        if(id == x86::x86_insn::X86_INS_CALL)
            return std::make_unique<Dyn_X86_INS_CALL>(isStatic);

        if(id == x86::x86_insn::X86_INS_RET)
            return std::make_unique<Dyn_X86_INS_RET>(isStatic);

        if(id == x86::x86_insn::X86_INS_LEAVE)
            return std::make_unique<Dyn_X86_INS_LEAVE>(isStatic);

        if(id == x86::x86_insn::X86_INS_ENTER)
            return std::make_unique<Dyn_X86_INS_ENTER>(isStatic);

        if(id == x86::x86_insn::X86_INS_DIV)
            return std::make_unique<Dyn_X86_INS_DIV>(isStatic);

        return std::make_unique<Inst_Base>(isStatic);
    }



}