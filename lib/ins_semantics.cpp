#include <sstream>
#include "ins_semantics.h"
#include "ins_types.h"
#include "error.h"
#include "BitVector.h"
#include "Engine.h"
#include "Register.h"
#define ERROR(MESSAGE) tana::default_error_handler(__FILE__, __LINE__, MESSAGE)


namespace tana{

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

    bool inst_dyn_details::two_operand(SEEngine *se, Inst_Base *inst)
    {
        std::shared_ptr<Operand> op0 = inst->oprd[0];
        std::shared_ptr<Operand> op1 = inst->oprd[1];
        std::shared_ptr<BitVector> v1, v0, res;
        auto opcode_id = inst->instruction_id;
        auto opcstr = "bv" + x86::insn_id2string(opcode_id);

        if (op1->type == Operand::ImmValue) {
            uint32_t temp_concrete = stoul(op1->field[0], nullptr, 16);
            v1 = std::make_shared<BitVector>(CONCRETE, temp_concrete, se->isImmSym(temp_concrete));
        } else if (op1->type == Operand::Reg) {
            v1 = se->readReg(op1->field[0]);
        } else if (op1->type == Operand::Mem) {
            v1 = se->readMem(inst->get_memory_address(), op1->bit);
        } else {
            ERROR("other instructions: op1 is not ImmValue, Reg, or Mem!");
            return false;
        }

        if (op0->type == Operand::Reg) { // dest op is reg
            v0 = se->readReg(op0->field[0]);
            res = buildop2(opcstr, v0, v1);
            se->writeReg(op0->field[0], res);
            return true;
        } else if (op0->type == Operand::Mem) { // dest op is mem
            v0 = se->readMem(inst->get_memory_address(), op0->bit);
            res = buildop2(opcstr, v0, v1);
            se->writeMem(inst->get_memory_address(), op0->bit ,res);
            return true;
        } else {
            ERROR("other instructions: op2 is not ImmValue, Reg, or Mem!");
            return false;
        }
    }


    bool Dyn_X86_INS_PUSH::symbolic_execution(SEEngine *se)
    {
        std::shared_ptr<Operand> op0 = this->oprd[0];
        std::shared_ptr<BitVector> v0;

        if (op0->type == Operand::ImmValue) {
            uint32_t temp_concrete = stoul(op0->field[0], nullptr, 16);
            v0 = std::make_shared<BitVector>(CONCRETE, temp_concrete, se->isImmSym(temp_concrete));
            se->writeMem(this->get_memory_address(), op0->bit, v0);
            return true;
        }

        if (op0->type == Operand::Reg) {
            auto regV = se->readReg(op0->field[0]);
            se->writeMem(this->get_memory_address(), op0->bit,regV);
            return true;
        }
        if (op0->type == Operand::Mem) {
            // The memaddr in the trace is the read address
            // We need to compute the write address
            auto reg = Registers::convert2RegID("esp");
            uint32_t esp_index = Registers::getRegIndex(reg);
            uint32_t esp_value = this->vcpu.gpr[esp_index];
            v0 = se->readMem(this->get_memory_address(), op0->bit);
            auto mem_esp_v = esp_value -4;
            std::stringstream sstream;
            sstream << std::hex << mem_esp_v << std::dec;
            se->writeMem(sstream.str(), op0->bit, v0);
            return true;
        }

        ERROR("push error: the operand is not Imm, Reg or Mem!");
        return false;
    }

    bool Dyn_X86_INS_POP::symbolic_execution(SEEngine *se)
    {
        std::shared_ptr<Operand> op0 = this->oprd[0];

        if (op0->type == Operand::Reg) {
            assert(Registers::getRegType(op0->field[0]) == FULL);
            auto v0 = se->readMem(this->get_memory_address(), op0->bit);
            se->writeReg(op0->field[0], v0);
            return true;
        }

        ERROR("pop error: the operand is not Reg!");
        return false;

    }

    bool Dyn_X86_INS_NEG::symbolic_execution(SEEngine *se)
    {
        std::shared_ptr<Operand> op0 = this->oprd[0];
        auto opcode_id = this->instruction_id;
        auto opcstr = "bv" + x86::insn_id2string(opcode_id);


        if (op0->type == Operand::Reg) {
            assert(Registers::getRegType(op0->field[0]) == FULL);
            auto v0 = se->readReg(op0->field[0]);
            auto res = buildop1(opcstr, v0);
            se->writeReg(op0->field[0], res);
            return true;
        }
        ERROR("neg error: the operand is not Reg!");
        return false;
    }

    bool Dyn_X86_INS_NOT::symbolic_execution(SEEngine *se)
    {
        std::shared_ptr<Operand> op0 = this->oprd[0];
        auto opcode_id = this->instruction_id;
        auto opcstr = "bv" + x86::insn_id2string(opcode_id);

        if (op0->type == Operand::Reg) {
            assert(Registers::getRegType(op0->field[0]) == FULL);
            auto v0 = se->readReg(op0->field[0]);
            auto res = buildop1(opcstr, v0);
            se->writeReg(op0->field[0], res);
            return true;
        }
        ERROR("neg error: the operand is not Reg!");
        return false;
    }

    bool Dyn_X86_INS_INC::symbolic_execution(SEEngine *se)
    {
        std::shared_ptr<Operand> op0 = this->oprd[0];
        auto opcode_id = this->instruction_id;
        auto opcstr = "bv" + x86::insn_id2string(opcode_id);
        std::shared_ptr<BitVector> v0, res;

        if (op0->type == Operand::Reg)
        {
            v0 = se->readReg(op0->field[0]);
        }

        if (op0->type == Operand::Mem) {
            v0 = se->readMem(this->get_memory_address(), op0->bit);
        }
        res = buildop1(opcstr, v0);

        if (op0->type == Operand::Reg) {
            se->writeReg(op0->field[0], res);
        }

        if (op0->type == Operand::Mem) {
            //memory[it->memory_address] = res;
            se->writeMem(this->get_memory_address(), op0->bit, res);
        }
        return true;

    }

    bool Dyn_X86_INS_DEC::symbolic_execution(SEEngine *se)
    {
        std::shared_ptr<Operand> op0 = this->oprd[0];
        auto opcode_id = this->instruction_id;
        auto opcstr = "bv" + x86::insn_id2string(opcode_id);
        std::shared_ptr<BitVector> v0, res;

        if (op0->type == Operand::Reg) {
            v0 = se->readReg(op0->field[0]);
        }

        if (op0->type == Operand::Mem) {
            v0 = se->readMem(this->get_memory_address(), op0->bit);
        }
        res = buildop1(opcstr, v0);

        if (op0->type == Operand::Reg)
        {
            se->writeReg(op0->field[0], res);
        }

        if (op0->type == Operand::Mem)
        {
            se->writeMem(this->get_memory_address(), op0->bit, res);
        }
        return true;

    }


    bool Dyn_X86_INS_MOVSX::symbolic_execution(SEEngine *se)
    {
        std::shared_ptr<Operand> op0 = this->oprd[0];
        std::shared_ptr<Operand> op1 = this->oprd[1];
        std::shared_ptr<BitVector> v0, v1, res;

        if (op1->type == Operand::Reg) {
            auto reg = Registers::convert2RegID(op1->field[0]);
            v1 = se->readReg(reg);

            v1 = se->SignExt(v1, op1->bit, op0->bit);
            se->writeReg(op0->field[0], v1);
            return true;
        }
        if (op1->type == Operand::Mem) {
            v1 = se->readMem(this->get_memory_address(), op1->bit);
            v1 = se->SignExt(v1, op1->bit, op0->bit);
            se->writeReg(op0->field[0], v1);
            return true;
        }
        return false;
    }

    bool Dyn_X86_INS_MOVZX::symbolic_execution(SEEngine *se)
    {
        std::shared_ptr<Operand> op0 = this->oprd[0];
        std::shared_ptr<Operand> op1 = this->oprd[1];
        std::shared_ptr<BitVector> v0, v1, res;

        if (op1->type == Operand::Reg) {
            auto reg = Registers::convert2RegID(op1->field[0]);
            v1 = se->readReg(reg);
            v1 = se->ZeroExt(v1, op0->bit);
            se->writeReg(op0->field[0], v1);
            return true;

        }
        if (op1->type == Operand::Mem) {
            //TODO
            v1 = se->readMem(this->get_memory_address(), op1->bit);
            v1 = se->ZeroExt(v1, op0->bit);
            se->writeReg(op0->field[0], v1);
            return true;
        }

        ERROR("MOVZX");
        return false;
    }

    bool Dyn_X86_INS_CMOVB::symbolic_execution(SEEngine *se)
    {
        std::shared_ptr<Operand> op0 = this->oprd[0];
        std::shared_ptr<Operand> op1 = this->oprd[1];
        std::shared_ptr<BitVector> v0, v1, res;
        auto opcode_id = this->instruction_id;

        if(!(this->vcpu.eflags_state))
            ERROR("CMOVB doesn't have eflags information");

        auto CF = this->vcpu.CF();
        if(!CF)
            return true;

        if (op0->type == Operand::Reg)
        {
            if (op1->type == Operand::ImmValue) { // mov reg, 0x1111
                uint32_t temp_concrete = stoul(op1->field[0], 0, 16);
                v1 = std::make_shared<BitVector>(CONCRETE, temp_concrete, se->isImmSym(temp_concrete));
                se->writeReg(op0->field[0], v1);
                return true;
            }
            if (op1->type == Operand::Reg) { // mov reg, reg
                v1 = se->readReg(op1->field[0]);
                se->writeReg(op0->field[0], v1);
                return true;
            }
            if (op1->type == Operand::Mem) { // mov reg, dword ptr [ebp+0x1]
                v1 = se->readMem(this->get_memory_address(), op1->bit);
                se->writeReg(op0->field[0], v1);
                return true;
            }

            ERROR("op1 is not ImmValue, Reg or Mem");
            return false;
        }
        if (op0->type == Operand::Mem) {
            if (op1->type == Operand::ImmValue) { // mov dword ptr [ebp+0x1], 0x1111
                uint32_t temp_concrete = stoul(op1->field[0], 0, 16);
                se->writeMem(this->get_memory_address(), op0->bit, std::make_shared<BitVector>(CONCRETE, temp_concrete, se->isImmSym(temp_concrete)));
                return true;
            } else if (op1->type == Operand::Reg) { // mov dword ptr [ebp+0x1], reg
                v1 = se->readReg(op1->field[0]);
                se->writeMem(this->get_memory_address(), op1->bit, v1);
                return true;
            }
        }
        ERROR("Error: The first operand in MOV is not Reg or Mem!");
        return false;

    }

    bool Dyn_X86_INS_MOV::symbolic_execution(SEEngine *se)
    {
        std::shared_ptr<Operand> op0 = this->oprd[0];
        std::shared_ptr<Operand> op1 = this->oprd[1];
        std::shared_ptr<BitVector> v0, v1, res;
        auto opcode_id = this->instruction_id;

        if (op0->type == Operand::Reg)
        {
            if (op1->type == Operand::ImmValue) { // mov reg, 0x1111
                uint32_t temp_concrete = stoul(op1->field[0], 0, 16);
                v1 = std::make_shared<BitVector>(CONCRETE, temp_concrete, se->isImmSym(temp_concrete));
                se->writeReg(op0->field[0], v1);
                return true;
            }
            if (op1->type == Operand::Reg) { // mov reg, reg
                v1 = se->readReg(op1->field[0]);
                se->writeReg(op0->field[0], v1);
                return true;
            }
            if (op1->type == Operand::Mem) { // mov reg, dword ptr [ebp+0x1]
                /* 1. Get mem address
                2. check whether the mem address has been accessed
                3. if not, create a new value
                4. else load the value in that memory
                */
                v1 = se->readMem(this->get_memory_address(), op1->bit);
                se->writeReg(op0->field[0], v1);
                return true;
            }

            ERROR("op1 is not ImmValue, Reg or Mem");
            return false;
        }
        if (op0->type == Operand::Mem) {
            if (op1->type == Operand::ImmValue) { // mov dword ptr [ebp+0x1], 0x1111
                uint32_t temp_concrete = stoul(op1->field[0], 0, 16);
                se->writeMem(this->get_memory_address(), op0->bit, std::make_shared<BitVector>(CONCRETE, temp_concrete, se->isImmSym(temp_concrete)));
                return true;
            } else if (op1->type == Operand::Reg) { // mov dword ptr [ebp+0x1], reg
                //memory[it->memory_address] = ctx[getRegName(op1->field[0])];
                v1 = se->readReg(op1->field[0]);
                se->writeMem(this->get_memory_address(), op1->bit, v1);
                return true;
            }
        }
        ERROR("Error: The first operand in MOV is not Reg or Mem!");
        return false;
    }

    bool Dyn_X86_INS_LEA::symbolic_execution(SEEngine *se)
    {
        std::shared_ptr<Operand> op0 = this->oprd[0];
        std::shared_ptr<Operand> op1 = this->oprd[1];
        std::shared_ptr<BitVector> v0, v1, res;
        /* lea reg, ptr [edx+eax*1]
           interpret lea instruction based on different address type
           1. op0 must be reg
           2. op1 must be addr
        */
        if (op0->type != Operand::Reg || op1->type != Operand::Mem) {
            ERROR("lea format error!");
            return false;
        }
        switch (op1->tag) {
            case 5: {
                std::shared_ptr<BitVector> f0, f1, f2; // corresponding field[0-2] in operand
                f0 = se->readReg(op1->field[0]);
                f1 = se->readReg(op1->field[1]);
                if (op1->field[2] == "1") {
                    res = buildop2("bvadd", f0, f1);
                    //ctx[getRegName(op0->field[0])] = res;
                    se->writeReg(op0->field[0], res);
                    return true;
                }
                uint32_t temp_concrete = stoul(op1->field[2], 0, 16);
                f2 = std::make_shared<BitVector>(CONCRETE, temp_concrete, se->isImmSym(temp_concrete));
                res = buildop2("bvimul", f1, f2);
                res = buildop2("bvadd", f0, res);
                se->writeReg(op0->field[0], res);
                return true;
            }
            case 7: {
                std::shared_ptr<BitVector> f0, f1, f2, f3; // addr7: eax+ebx*2+0xfffff1
                //f0 = ctx[getRegName(op1->field[0])];       //eax
                //f1 = ctx[getRegName(op1->field[1])];       //ebx

                f0 = se->readReg(op1->field[0]);
                f1 = se->readReg(op1->field[1]);

                uint32_t temp_concrete1 = stoul(op1->field[2], 0, 16);
                f2 = std::make_shared<BitVector>(CONCRETE, temp_concrete1, se->isImmSym(temp_concrete1));   //2
                std::string sign = op1->field[3];          //+
                uint32_t temp_concrete2 = stoul(op1->field[4], 0, 16);
                f3 = std::make_shared<BitVector>(CONCRETE, temp_concrete2, se->isImmSym(temp_concrete2));   //0xfffff1
                assert((sign == "+") || (sign == "-"));
                if (op1->field[2] == "1") {
                    res = buildop2("bvadd", f0, f1);
                } else {
                    res = buildop2("bvimul", f1, f2);
                    res = buildop2("bvadd", f0, res);
                }
                if (sign == "+")
                    res = buildop2("bvadd", res, f3);
                else
                    res = buildop2("bvsub", res, f3);
                //ctx[getRegName(op0->field[0])] = res;

                se->writeReg(op0->field[0], res);

                return true;
            }
            case 4: {
                std::shared_ptr<BitVector> f0, f1; // addr4: eax+0xfffff1
                //f0 = ctx[getRegName(op1->field[0])];       //eax
                f0 = se->readReg(op1->field[0]);
                uint32_t temp_concrete = stoul(op1->field[2], 0, 16);
                f1 = std::make_shared<BitVector>(CONCRETE, temp_concrete, se->isImmSym(temp_concrete));   //0xfffff1
                std::string sign = op1->field[1];          //+
                if (sign == "+")
                    res = buildop2("bvadd", f0, f1);
                else
                    res = buildop2("bvsub", f0, f1);
                //ctx[getRegName(op0->field[0])] = res;

                se->writeReg(op0->field[0], res);
                return true;
            }
            case 6: {
                std::shared_ptr<BitVector> f0, f1, f2; // addr6: eax*2+0xfffff1
                //f0 = ctx[getRegName(op1->field[0])];
                f0 = se->readReg(op1->field[0]);
                uint32_t temp_concrete1 = stoul(op1->field[1]);
                uint32_t temp_concrete2 = stoul(op1->field[3]);

                f1 = std::make_shared<BitVector>(CONCRETE, temp_concrete1, se->isImmSym(temp_concrete1));
                f2 = std::make_shared<BitVector>(CONCRETE, temp_concrete2, se->isImmSym(temp_concrete2));
                std::string sign = op1->field[2];
                if (op1->field[1] == "1") {
                    res = f0;
                } else {
                    res = buildop2("bvimul", f0, f1);
                }
                if (sign == "+")
                    res = buildop2("bvadd", res, f2);
                else
                    res = buildop2("bvsub", res, f2);
                se->writeReg(op0->field[0], res);
                return true;
            }
            case 3: {
                std::shared_ptr<BitVector> f0, f1;          // addr3: eax*2
                //f0 = ctx[getRegName(op1->field[0])];
                f0 = se->readReg(op1->field[0]);

                uint32_t temp_concrete = stoul(op1->field[1]);
                f1 = std::make_shared<BitVector>(CONCRETE, temp_concrete, se->isImmSym(temp_concrete));
                res = buildop2("bvimul", f0, f1);
                //ctx[getRegName(op0->field[0])] = res;
                se->writeReg(op0->field[0], res);
                return true;
            }

            case 2: {
                v1 = se->readReg(op1->field[0]);
                se->writeReg(op0->field[0], v1);
                return true;
            }

            case 1: {
                std::shared_ptr<BitVector> f0;
                uint32_t temp_concrete = stoul(op1->field[0], nullptr, 16);
                f0 = std::make_shared<BitVector>(CONCRETE, temp_concrete, se->isImmSym(temp_concrete));
                se->writeReg(op0->field[0], f0);
                return true;
            }
            default:
                ERROR("Other tags in addr is not ready for lea!");
                return false;
        }

    }


    bool Dyn_X86_INS_XCHG::symbolic_execution(SEEngine *se)
    {
        std::shared_ptr<Operand> op0 = this->oprd[0];
        std::shared_ptr<Operand> op1 = this->oprd[1];
        std::shared_ptr<BitVector> v0, v1, res;
        auto opcode_id = this->instruction_id;

        if (op1->type == Operand::Reg)
        {
            v1 = se->readReg(op1->field[0]);
            if (op0->type == Operand::Reg)
            {
                v0 = se->readReg(op0->field[0]);
                v1 = se->readReg(op1->field[0]);

                se->writeReg(op1->field[0], v0);
                se->writeReg(op0->field[0], v1);

                return true;
            }
            if (op0->type == Operand::Mem)
            {
                v0 = se->readMem(this->get_memory_address(), op0->bit);
                v1 = se->readReg(op1->field[0]);

                //ctx[getRegName(op1->field[0])] = v0; // xchg mem, reg
                //memory[it->memory_address] = v1;
                se->writeReg(op1->field[0], v0);
                se->writeMem(this->get_memory_address(), op0->bit, v1);

                return true;
            }
            ERROR("xchg error: 1");
            return false;
        }
        if (op1->type == Operand::Mem) {
            v1 = se->readMem(this->get_memory_address(), op1->bit);
            if (op0->type == Operand::Reg) {
                v0 = se->readReg(op0->field[0]);
                se->writeReg(op0->field[0], v1); // xchg reg, mem
                se->writeMem(this->get_memory_address(), op1->bit, v0);
            }
            ERROR("xchg error 3");
            return false;

        }
        ERROR("xchg error: 2");
        return false;
    }

    bool Dyn_X86_INS_SBB::symbolic_execution(SEEngine *se)
    {
        std::shared_ptr<Operand> op0 = this->oprd[0];
        std::shared_ptr<Operand> op1 = this->oprd[1];

        if (op1->type == Operand::Reg) {

        }
        if (op1->type == Operand::Mem) {

        }
        return true;
    }


    bool Dyn_X86_INS_IMUL::symbolic_execution(SEEngine *se)
    {
        std::shared_ptr<Operand> op0 = this->oprd[0];
        std::shared_ptr<Operand> op1 = this->oprd[1];
        std::shared_ptr<Operand> op2 = this->oprd[2];
        std::shared_ptr<BitVector> v1, v2, v3, res;
        auto opcode_id = this->instruction_id;
        auto opcstr = "bv" + x86::insn_id2string(opcode_id);
        if(this->get_operand_number() == 1)
        {
            //TODO
            return true;
        }

        if(this->get_operand_number() == 2)
        {
            //TODO
            return true;
        }


        if (op0->type == Operand::Reg &&
            op1->type == Operand::Reg &&
            op2->type == Operand::ImmValue) { // imul reg, reg, imm
            v1 = se->readReg(op1->field[0]);
            uint32_t temp_concrete = stoul(op2->field[0], 0, 16);
            v2 = std::make_shared<BitVector>(CONCRETE, temp_concrete, se->isImmSym(temp_concrete));
            res = buildop2(opcstr, v1, v2);
            se->writeReg(op0->field[0], res);
            return true;
        }

        ERROR("three operands instructions other than imul are not handled!");
        return false;

    }


    bool Dyn_X86_INS_SHLD::symbolic_execution(SEEngine *se)
    {
        std::shared_ptr<Operand> op0 = this->oprd[0];
        std::shared_ptr<Operand> op1 = this->oprd[1];
        std::shared_ptr<Operand> op2 = this->oprd[2];
        std::shared_ptr<BitVector> v1, v2, v3, res;
        auto opcode_id = this->instruction_id;
        auto opcstr = "bv" + x86::insn_id2string(opcode_id);

        if (op0->type == Operand::Reg &&
            op1->type == Operand::Reg &&
            op2->type == Operand::ImmValue) {                                           // shld shrd reg, reg, imm
            v1 = se->readReg(op0->field[0]);
            v2 = se->readReg(op1->field[0]);
            uint32_t temp_concrete = stoul(op2->field[0], 0, 16);
            v3 = std::make_shared<BitVector>(CONCRETE, temp_concrete, se->isImmSym(temp_concrete));
            res = buildop3(opcstr, v1, v2, v3);
            se->writeReg(op0->field[0], res);
            return true;
        }

        ERROR("shld is not handled!");
        return false;

    }

    bool Dyn_X86_INS_SHRD::symbolic_execution(SEEngine *se)
    {
        std::shared_ptr<Operand> op0 = this->oprd[0];
        std::shared_ptr<Operand> op1 = this->oprd[1];
        std::shared_ptr<Operand> op2 = this->oprd[2];
        std::shared_ptr<BitVector> v1, v2, v3, res;
        auto opcode_id = this->instruction_id;
        auto opcstr = "bv" + x86::insn_id2string(opcode_id);

        if (op0->type == Operand::Reg &&
            op1->type == Operand::Reg &&
            op2->type == Operand::ImmValue) {                                           // shld shrd reg, reg, imm
            v1 = se->readReg(op0->field[0]);
            v2 = se->readReg(op1->field[0]);
            uint32_t temp_concrete = stoul(op2->field[0], 0, 16);
            v3 = std::make_shared<BitVector>(CONCRETE, temp_concrete, se->isImmSym(temp_concrete));
            res = buildop3(opcstr, v1, v2, v3);
            se->writeReg(op0->field[0], res);
            return true;
        }

        ERROR("shrd is not handled!");
        return false;
    }

    bool Dyn_X86_INS_ADD::symbolic_execution(SEEngine *se)
    {
        return inst_dyn_details::two_operand(se, this);
    }

    bool Dyn_X86_INS_SUB::symbolic_execution(SEEngine *se)
    {
        return inst_dyn_details::two_operand(se, this);
    }

    bool Dyn_X86_INS_AND::symbolic_execution(SEEngine *se)
    {
        return inst_dyn_details::two_operand(se, this);
    }

    bool Dyn_X86_INS_ADC::symbolic_execution(SEEngine *se)
    {
        if(!(this->vcpu.eflags_state))
            return inst_dyn_details::two_operand(se, this);

        bool CF = this->vcpu.CF();

        std::shared_ptr<Operand> op0 = this->oprd[0];
        std::shared_ptr<Operand> op1 = this->oprd[1];
        std::shared_ptr<BitVector> v1, v0, res;
        auto opcode_id = this->instruction_id;
        auto opcstr = x86::insn_id2string(opcode_id);

        std::shared_ptr<BitVector> v_one =  std::make_shared<BitVector>(CONCRETE, 1, se->isImmSym(1));

        if (op1->type == Operand::ImmValue) {
            uint32_t temp_concrete = stoul(op1->field[0], 0, 16);
            v1 = std::make_shared<BitVector>(CONCRETE, temp_concrete, se->isImmSym(temp_concrete));
        } else if (op1->type == Operand::Reg) {
            v1 = se->readReg(op1->field[0]);
        } else if (op1->type == Operand::Mem) {
            v1 = se->readMem(this->get_memory_address(), op1->bit);
        } else {
            ERROR("other instructions: op1 is not ImmValue, Reg, or Mem!");
            return false;
        }

        if (op0->type == Operand::Reg) { // dest op is reg
            v0 = se->readReg(op0->field[0]);
            res = buildop2("bvadd", v0, v1);
            if(CF)
            {
                res = buildop2("bvadd", res, v_one);
            }
            se->writeReg(op0->field[0], res);
            return true;
        } else if (op0->type == Operand::Mem) { // dest op is mem
            v0 = se->readMem(this->get_memory_address(), op0->bit);
            res = buildop2("bvadd", v0, v1);
            if(CF)
            {
                res = buildop2("bvadd", res, v_one);
            }
            se->writeMem(this->get_memory_address(), op0->bit, res);
            return true;
        } else {
            ERROR("other instructions: op2 is not ImmValue, Reg, or Mem!");
            return false;
        }


    }

    bool Dyn_X86_INS_ROR::symbolic_execution(SEEngine *se)
    {
        return inst_dyn_details::two_operand(se, this);
    }

    bool Dyn_X86_INS_ROL::symbolic_execution(SEEngine *se)
    {
        return inst_dyn_details::two_operand(se, this);
    }

    bool Dyn_X86_INS_OR::symbolic_execution(SEEngine *se)
    {
        return inst_dyn_details::two_operand(se, this);
    }

    bool Dyn_X86_INS_XOR::symbolic_execution(SEEngine *se)
    {
        return inst_dyn_details::two_operand(se, this);
    }

    bool Dyn_X86_INS_SHL::symbolic_execution(SEEngine *se)
    {
        return inst_dyn_details::two_operand(se, this);
    }

    bool Dyn_X86_INS_SHR::symbolic_execution(SEEngine *se)
    {
        return inst_dyn_details::two_operand(se, this);
    }

    bool Dyn_X86_INS_SAR::symbolic_execution(SEEngine *se)
    {
        return inst_dyn_details::two_operand(se, this);
    }


    bool Dyn_X86_INS_CALL::symbolic_execution(SEEngine *se)
    {
        return true;
    }

    bool Dyn_X86_INS_RET::symbolic_execution(SEEngine *se)
    {
        return true;
    }

    bool Dyn_X86_INS_LEAVE::symbolic_execution(SEEngine *se)
    {

        // EBP = ESP
        auto v_ebp = se->readReg("ebp");
        se->writeReg("esp", v_ebp);

        // POP EBP
        auto v0 = se->readMem(this->get_memory_address(), REGISTER_SIZE);
        se->writeReg("ebp", v0);

        return true;
    }

    bool Dyn_X86_INS_ENTER::symbolic_execution(SEEngine *se)
    {
        return true;
    }

    bool Dyn_X86_INS_DIV::symbolic_execution(SEEngine *se)
    {
        std::shared_ptr<Operand> op0 = this->oprd[0];
        auto operand_size = op0->bit;
        assert(operand_size == 8 || operand_size == 16 || operand_size == 32);
        std::shared_ptr<BitVector> dividend, divisor, quotient, remainder;
        if(operand_size == 8)
        {
            dividend = se->readReg("ax");
        }
        if(operand_size == 16)
        {
            auto temp1 = se->readReg("ax");
            auto temp2 = se->readReg("dx");
            dividend = se->Concat(temp2, temp1);
        }
        if(operand_size == 32)
        {
            auto temp1 = se->readReg("eax");
            auto temp2 = se->readReg("edx");
            dividend = se->Concat(temp2, temp1);
        }


        if(op0->type == Operand::Mem)
        {
            divisor = se->readMem(get_memory_address(), op0->bit);
        }

        if(op0->type == Operand::Reg)
        {
            divisor = se->readReg(op0->field[0]);
        }

        quotient = buildop2("bvquo", dividend, divisor);
        remainder = buildop2("bvrem", dividend, divisor);

        if(operand_size == 8)
        {
            se->writeReg("al", quotient);
            se->writeReg("ah", remainder);
        }

        if(operand_size == 16)
        {
            se->writeReg("ax", quotient);
            se->writeReg("dx", remainder);
        }

        if(operand_size == 32)
        {
            se->writeReg("eax", quotient);
            se->writeReg("edx", remainder);
        }

        return true;

    }



}