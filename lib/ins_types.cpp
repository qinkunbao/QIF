#include <string>
#include <memory>
#include "ins_types.h"
#include "ins_parser.h"
#include "error.h"

#define ERROR(MESSAGE) tana::default_error_handler(__FILE__, __LINE__, MESSAGE)


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
        assert(eflags>=0);
        return static_cast<bool>(eflags & 0x1u);
    }

    bool vcpu_ctx::PF()
    {
        assert(eflags>=0);
        return static_cast<bool>(eflags & 0x4u);
    }

    bool vcpu_ctx::AF()
    {
        assert(eflags>=0);
        return static_cast<bool>(eflags & 0x10u);
    }

    bool vcpu_ctx::ZF()
    {
        assert(eflags>=0);
        return static_cast<bool>(eflags & 0x40u);
    }

    bool vcpu_ctx::SF()
    {
        assert(eflags>=0);
        return static_cast<bool>(eflags & 0x80u);
    }

    bool vcpu_ctx::TF()
    {
        assert(eflags>=0);
        return static_cast<bool>(eflags & 0x100u);
    }

    bool vcpu_ctx::DF()
    {
        assert(eflags>=0);
        return static_cast<bool>(eflags & 0x400u);
    }

    bool vcpu_ctx::OF()
    {
        assert(eflags>=0);
        return static_cast<bool>(eflags & 0x800u);
    }


    Inst_Base::Inst_Base(bool inst_type):id(0), addrn(0),
                         instruction_id(x86::X86_INS_INVALID),
                         oprd{nullptr, nullptr, nullptr},memory_address(0), is_static(inst_type),
                         mem_data(0), mem_data_available(false)
    {
    }


    uint32_t Inst_Base::get_operand_number() const
    {
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

    void Inst_Base::set_mem_data(uint32_t data)
    {
        mem_data_available = true;
        this->mem_data = data;
    }

    uint32_t Inst_Base::read_mem_data()
    {
        if(!mem_data_available) {
            ERROR("memory data not available");
            return 0;
        }

        if(this->instruction_id == x86::X86_INS_POP
        || this->instruction_id == x86::X86_INS_PUSH)
            return mem_data;
        uint32_t mem_size = 0;
        for(int i = 0; i < 3; ++i)
        {
            if(oprd[i] == nullptr)
                continue;
            if(oprd[i]->type == Operand::Mem)
            {
                mem_size = oprd[i]->bit;
            }

        }

        switch (mem_size)
        {
            case 32:
            case 16:
            case 8:
                return mem_data;
            default:
                return 0;
        }

    }





}