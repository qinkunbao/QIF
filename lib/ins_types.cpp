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





}