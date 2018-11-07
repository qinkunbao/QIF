/*************************************************************************
2     > File Name: BitTaint.cpp
3     > Author: Qinkun Bao
4     > Mail: qinkunbao@gmail.com
5     > Created Time: Thu 01 Nov 2018 03:28:51 PM EDT
6  ************************************************************************/


#include "ins_parser.h"
#include "BitTaint.h"

namespace bittaint {

    BitTaint::BitTaint(std::string symbol_name, uint32_t m_addr, \
                       uint32_t size, std::vector<tana::Inst>::iterator s, \
                       std::vector<tana::Inst>::iterator e) :  mem(m_addr, size), reg(), eip()
    {
        for (uint32_t index = 0; index < size * BYTESIZE; ++index)
        {
            BitDefUseChain bool_var = BitDefUseChain(symbol_name, m_addr, index);
            DefUse.push_back(bool_var);
        }
        start = s;
        end = e;
    }

    BitTaint::BitTaint(std::string symbol_name, uint32_t m_addr, \
                       uint32_t size, bool direction) :  mem(m_addr, size), reg(), eip()
    {
        forward = direction;
        for (uint32_t index = 0; index < size * BYTESIZE; ++index)
        {
            BitDefUseChain bool_var = BitDefUseChain(symbol_name, m_addr, index);
            DefUse.push_back(bool_var);
        }
    }

    int BitTaint::DO_X86_INS_CALL(const tana::Inst &it)
    {
        uint32_t opr_num = it.get_operand_number();
        assert(opr_num == 1);
        auto op0 = it.oprd[0];
        switch (op0->type)
        {
            case tana::Operand::ImmValue:
                return 1;
            case tana::Operand::Reg:
                break;
            case tana::Operand::Mem:
                break;
            default:
                std::cout << "Error in " << __LINE__ << __FILE__ << std::endl;
                exit(0);
        }
        return 1;

    }

    int BitTaint::DO_X86_INS_PUSH(const tana::Inst &it)
    {
        uint32_t opr_num = it.get_operand_number();
        assert(opr_num == 1);
        auto op0 = it.oprd[0];
        switch (op0->type)
        {
            case tana::Operand::ImmValue:
                break;
            case tana::Operand::Reg:
                break;
            case tana::Operand::Mem:
                break;
            default:
                std::cout << "Error in " << __LINE__ << __FILE__ << std::endl;
                exit(0);
        }
        return 1;
    }

    int BitTaint::DO_X86_INS_POP(const tana::Inst &it)
    {
        uint32_t opr_num = it.get_operand_number();
        assert(opr_num == 1);
        auto op0 = it.oprd[0];
        assert(op0->type == tana::Operand::Reg);
        std::string reg_name(op0->field[0]);
        auto reg_size = op0->bit / BYTESIZE;
        auto reg_id = reg.str2id(reg_name);

        auto reg_data = reg.read_register(reg_id);
        assert(reg_data.size() == reg_size);

        mem.write_data(it.memory_address, reg_data);
        return 1;
    }

    int BitTaint::DO_X86_INS_NEG(const tana::Inst &it)
    {

        uint32_t opr_num = it.get_operand_number();
        assert(opr_num == 1);
        auto op0 = it.oprd[0];

        switch (op0->type)
        {
            case tana::Operand::Mem:
                break;
            case tana::Operand::Reg:
                break;
            default:
                exit(0);
        }
        return 1;
    }

    int BitTaint::DO_X86_INS_NOT(const tana::Inst &it)
    {
        uint32_t opr_num = it.get_operand_number();
        assert(opr_num == 1);
        auto op0 = it.oprd[0];

        switch (op0->type)
        {
            case tana::Operand::Mem:
                break;
            case tana::Operand::Reg:
                break;
            default:
                exit(0);
        }
        return 1;
    }

    int BitTaint::DO_X86_INS_INC(const tana::Inst &it)
    {
        uint32_t opr_num = it.get_operand_number();
        assert(opr_num == 1);
        auto op0 = it.oprd[0];

        switch (op0->type)
        {
            case tana::Operand::Mem:
                break;
            case tana::Operand::Reg:
                break;
            default:
                exit(0);
        }
        return 1;
    }

    int BitTaint::DO_X86_INS_DEC(const tana::Inst &it)
    {
        uint32_t opr_num = it.get_operand_number();
        assert(opr_num == 1);
        auto op0 = it.oprd[0];

        switch (op0->type)
        {
            case tana::Operand::Mem:
                break;
            case tana::Operand::Reg:
                break;
            default:
                exit(0);
        }
        return 1;
    }

    int BitTaint::DO_X86_INS_MOVSX(const tana::Inst &it)
    {
        return 1;
    }

    int BitTaint::DO_X86_INS_MOVZX(const tana::Inst &it)
    {
        return 1;
    }

    int BitTaint::DO_X86_INS_CMOVB(const tana::Inst &it)
    {
        return 1;
    }

    int BitTaint::DO_X86_INS_MOV(const tana::Inst &it)
    {
        return 1;
    }

    int BitTaint::DO_X86_INS_LEA(const tana::Inst &it)
    {
        return 1;
    }

    int BitTaint::DO_X86_INS_XCHG(const tana::Inst &it)
    {
        return 1;
    }


    int BitTaint::DO_X86_INS_SUB(const tana::Inst &it)
    {
        return 1;
    }

    int BitTaint::DO_X86_INS_SBB(const tana::Inst &it)
    {
        return 1;
    }

    int BitTaint::DO_X86_INS_IMUL(const tana::Inst &it)
    {
        return 1;
    }

    int BitTaint::DO_X86_INS_SHLD(const tana::Inst &it)
    {
        return 1;
    }

    int BitTaint::DO_X86_INS_SHL(const tana::Inst &it)
    {
        return 1;
    }


    int BitTaint::DO_X86_INS_SHR(const tana::Inst &it)
    {
        return 1;
    }

    int BitTaint::DO_X86_INS_SHRD(const tana::Inst &it)
    {
        return 1;
    }

    int BitTaint::DO_X86_INS_ADD(const tana::Inst &it)
    {
        return 1;
    }


    int BitTaint::DO_X86_INS_ADC(const tana::Inst &it)
    {
        return 1;
    }

    int BitTaint::DO_X86_INS_RET(const tana::Inst &it)
    {
        return 1;
    }

    int BitTaint::DO_X86_INS_AND(const tana::Inst &it)
    {
        return 1;
    }

    int BitTaint::DO_X86_INS_LEAVE(const tana::Inst &it)
    {
        return 1;
    }


    int BitTaint::DO_X86_INS_XOR(const tana::Inst &it)
    {
        return 1;
    }

    void BitTaint::run()
    {
        int ret_status;
        for(auto it = start; it != end; ++it)
        {
            ret_status = execute_insn(it);
            std::cout << "Insn: " << it->id << std::endl;
            if(ret_status == 0)
            {
                break;
            }
        }
    }

    int BitTaint::execute_insn(std::vector<tana::Inst>::iterator it)
    {
        auto insn = it->instruction_id;
        int ret = 0;
        auto it_insn = *it;
        switch (insn)
        {
            case tana::x86::X86_INS_CALL:
                ret = DO_X86_INS_CALL(it_insn);
                break;

            case tana::x86::X86_INS_PUSH:
                ret = DO_X86_INS_PUSH(it_insn);
                break;

            case tana::x86::X86_INS_POP:
                ret = DO_X86_INS_POP(it_insn);
                break;

            case tana::x86::X86_INS_NEG:
                ret = DO_X86_INS_NEG(it_insn);
                break;

            case tana::x86::X86_INS_NOT:
                ret = DO_X86_INS_NOT(it_insn);
                break;

            case tana::x86::X86_INS_INC:
                ret = DO_X86_INS_INC(it_insn);
                break;

            case tana::x86::X86_INS_DEC:
                ret = DO_X86_INS_DEC(it_insn);
                break;

            case tana::x86::X86_INS_MOVSX:
                ret = DO_X86_INS_MOVSX(it_insn);
                break;

            case tana::x86::X86_INS_MOVZX:
                ret = DO_X86_INS_MOVZX(it_insn);
                break;

            case tana::x86::X86_INS_CMOVB:
                ret = DO_X86_INS_CMOVB(it_insn);
                break;

            case tana::x86::X86_INS_MOV:
                ret = DO_X86_INS_MOV(it_insn);
                break;

            case tana::x86::X86_INS_LEA:
                ret = DO_X86_INS_LEA(it_insn);
                break;

            case tana::x86::X86_INS_XCHG:
                ret = DO_X86_INS_XCHG(it_insn);
                break;

            case tana::x86::X86_INS_SUB:
                ret = DO_X86_INS_SUB(it_insn);
                break;

            case tana::x86::X86_INS_SBB:
                ret = DO_X86_INS_SBB(it_insn);
                break;

            case tana::x86::X86_INS_IMUL:
                ret = DO_X86_INS_IMUL(it_insn);
                break;

            case tana::x86::X86_INS_SHL:
                ret = DO_X86_INS_SHL(it_insn);
                break;

            case tana::x86::X86_INS_SHLD:
                ret = DO_X86_INS_SHLD(it_insn);
                break;

            case tana::x86::X86_INS_SHR:
                ret = DO_X86_INS_SHR(it_insn);
                break;

            case tana::x86::X86_INS_SHRD:
                ret = DO_X86_INS_SHRD(it_insn);
                break;

            case tana::x86::X86_INS_ADD:
                ret = DO_X86_INS_ADD(it_insn);
                break;

            case tana::x86::X86_INS_ADC:
                ret = DO_X86_INS_ADC(it_insn);
                break;

            case tana::x86::X86_INS_RET:
                ret = DO_X86_INS_RET(it_insn);
                break;


            case tana::x86::X86_INS_AND:
                ret = DO_X86_INS_AND(it_insn);
                break;

            case tana::x86::X86_INS_LEAVE:
                ret = DO_X86_INS_LEAVE(it_insn);
                break;

            case tana::x86::X86_INS_XOR:
                ret = DO_X86_INS_XOR(it_insn);
                break;

            case tana::x86::X86_INS_JLE:
            case tana::x86::X86_INS_CMP:
            case tana::x86::X86_INS_JMP:
            case tana::x86::X86_INS_JB:
            case tana::x86::X86_INS_NOP:
            case tana::x86::X86_INS_JZ:
            case tana::x86::X86_INS_JNZ:
            case tana::x86::X86_INS_TEST:
            case tana::x86::X86_INS_DATA16:
                ret = 1;
                break;

            default:
                std::cout << "Error in "<< __FILE__ <<  " Line :"<< __LINE__
                << " Instruction ID: " << it->id << std::endl;
                ret = 1;
        }
        if(ret)
        {
            auto next_inst = std::next(it);
            if(next_inst == end)
            {
                return 0;
            }
            eip = *std::next(it, 1);
            return 1;
        }
        else{
            return 0;
        }
    }

}