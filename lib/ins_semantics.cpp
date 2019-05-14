#include <sstream>
#include "ins_semantics.h"
#include "ins_types.h"
#include "error.h"
#include "BitVector.h"
#include "Engine.h"
#include "Register.h"
#include "Constrains.h"

#define ERROR(MESSAGE) tana::default_error_handler(__FILE__, __LINE__, MESSAGE)
#define WARN(MESSAGE) tana::default_warn_handler(__FILE__, __LINE__, MESSAGE)


namespace tana {


    void updateZF(SEEngine *se, std::shared_ptr<BitVector> b) {
        auto res = buildop2(BVOper::equal, b, 0);
        se->updateFlags("ZF", res);

    }

    void updateSF(SEEngine *se, std::shared_ptr<BitVector> b, uint32_t op_size) {
        uint32_t max_num_size = 0;
        if (op_size == 32) {
            max_num_size = 0x7fffffff;

        } else if (op_size == 16) {
            max_num_size = 0x7fff;

        } else if (op_size == 8) {
            max_num_size = 0x7f;
        } else {
            ERROR("Invalid operand size");
        }
        auto SF = buildop2(BVOper::greater, b, max_num_size);
        se->updateFlags("SF", SF);


    }

    void updateOFadd(SEEngine *se, std::shared_ptr<BitVector> A, std::shared_ptr<BitVector> B, uint32_t op_size) {
        // https://www.doc.ic.ac.uk/~eedwards/compsys/arithmetic/
        // Add Overflow occurs if
        // Situation 1: (+A) + (+B) = -C
        // Situation 2: (-A) + (-B) = +C
        uint32_t max_num_size = 0;
        if (op_size == 32) {
            max_num_size = 0x7fffffff;

        } else if (op_size == 16) {
            max_num_size = 0x7fff;

        } else if (op_size == 8) {
            max_num_size = 0x7f;
        } else {
            ERROR("Invalid operand size");
        }
        // Situation 1
        auto con1 = buildop2(BVOper::less, A, max_num_size);
        auto con2 = buildop2(BVOper::less, B, max_num_size);
        auto C1 = buildop2(BVOper::bvadd, A, B);
        auto C1_cons = buildop2(BVOper::greater, C1, max_num_size);
        auto S1 = buildop3(BVOper::bvand, con1, con2, C1_cons);

        // Situation 2
        auto con3 = buildop2(BVOper::greater, A, max_num_size);
        auto con4 = buildop2(BVOper::greater, B, max_num_size);
        auto sum2 = buildop2(BVOper::bvadd, A, B);
        auto sum2_cons = buildop2(BVOper::less, C1, max_num_size);
        auto S2 = buildop3(BVOper::bvand, con3, con4, sum2_cons);

        // S1 AND S2
        auto OF = buildop2(BVOper::bvor, S1, S2);
        se->updateFlags("OF", OF);

    }

    void updateOFsub(SEEngine *se, std::shared_ptr<BitVector> A, std::shared_ptr<BitVector> B, uint32_t op_size) {

        // Sub Overflow occurs if
        // Situation1 :(+A) − (−B) = −C
        // Situation2: (−A) − (+B) = +C
        uint32_t max_num_size = 0;
        if (op_size == 32) {
            max_num_size = 0x7fffffff;

        } else if (op_size == 16) {
            max_num_size = 0x7fff;

        } else if (op_size == 8) {
            max_num_size = 0x7f;
        } else {
            ERROR("Invalid operand size");
        }
        // Situation 1
        auto con1 = buildop2(BVOper::less, A, max_num_size);
        auto con2 = buildop2(BVOper::greater, B, max_num_size);
        auto C1 = buildop2(BVOper::bvsub, A, B);
        auto C1_cons = buildop2(BVOper::greater, C1, max_num_size);
        auto S1 = buildop3(BVOper::bvand, con1, con2, C1_cons);

        // Situation 2
        auto con3 = buildop2(BVOper::greater, A, max_num_size);
        auto con4 = buildop2(BVOper::less, B, max_num_size);
        auto sum2 = buildop2(BVOper::bvsub, A, B);
        auto sum2_cons = buildop2(BVOper::less, C1, max_num_size);
        auto S2 = buildop3(BVOper::bvand, con3, con4, sum2_cons);

        // S1 AND S2
        auto OF = buildop2(BVOper::bvor, S1, S2);
        se->updateFlags("OF", OF);


    }

    void updateCFadd(SEEngine *se, std::shared_ptr<BitVector> b1, \
                     uint32_t op_size) {
        uint32_t max_num_size;
        if (op_size == 32) {
            max_num_size = 0xffffffff;

        } else if (op_size == 16) {
            max_num_size = 0xffff;

        } else if (op_size == 8) {
            max_num_size = 0xff;
        } else {
            ERROR("Invalid operand size");
        }
        auto res = buildop2(BVOper::greater, b1, max_num_size);
        se->updateFlags("CF", res);
    }

    void updateCFsub(SEEngine *se, std::shared_ptr<BitVector> b1, std::shared_ptr<BitVector> b2) {
        auto res = buildop2(BVOper::less, b1, b2);
        se->updateFlags("CF", res);
    }


    std::unique_ptr<Inst_Base> Inst_Dyn_Factory::makeInst(tana::x86::x86_insn id, bool isStatic) {

        if (id == x86::x86_insn::X86_INS_NOP)
            return std::make_unique<Dyn_X86_INS_NOP>(isStatic);

        if (id == x86::x86_insn::X86_INS_PUSH)
            return std::make_unique<Dyn_X86_INS_PUSH>(isStatic);

        if (id == x86::x86_insn::X86_INS_POP)
            return std::make_unique<Dyn_X86_INS_POP>(isStatic);

        if (id == x86::x86_insn::X86_INS_NEG)
            return std::make_unique<Dyn_X86_INS_NEG>(isStatic);

        if (id == x86::x86_insn::X86_INS_NOT)
            return std::make_unique<Dyn_X86_INS_NOT>(isStatic);

        if (id == x86::x86_insn::X86_INS_INC)
            return std::make_unique<Dyn_X86_INS_INC>(isStatic);

        if (id == x86::x86_insn::X86_INS_DEC)
            return std::make_unique<Dyn_X86_INS_DEC>(isStatic);

        if (id == x86::x86_insn::X86_INS_MOVZX)
            return std::make_unique<Dyn_X86_INS_MOVZX>(isStatic);

        if (id == x86::x86_insn::X86_INS_MOVSX)
            return std::make_unique<Dyn_X86_INS_MOVSX>(isStatic);

        if (id == x86::x86_insn::X86_INS_CMOVB)
            return std::make_unique<Dyn_X86_INS_CMOVB>(isStatic);

        if (id == x86::x86_insn::X86_INS_MOV)
            return std::make_unique<Dyn_X86_INS_MOV>(isStatic);

        if (id == x86::x86_insn::X86_INS_LEA)
            return std::make_unique<Dyn_X86_INS_LEA>(isStatic);

        if (id == x86::x86_insn::X86_INS_XCHG)
            return std::make_unique<Dyn_X86_INS_XCHG>(isStatic);

        if (id == x86::x86_insn::X86_INS_SBB)
            return std::make_unique<Dyn_X86_INS_SBB>(isStatic);

        if (id == x86::x86_insn::X86_INS_IMUL)
            return std::make_unique<Dyn_X86_INS_IMUL>(isStatic);

        if (id == x86::x86_insn::X86_INS_SHLD)
            return std::make_unique<Dyn_X86_INS_SHLD>(isStatic);

        if (id == x86::x86_insn::X86_INS_SHRD)
            return std::make_unique<Dyn_X86_INS_SHRD>(isStatic);

        if (id == x86::x86_insn::X86_INS_ADD)
            return std::make_unique<Dyn_X86_INS_ADD>(isStatic);

        if (id == x86::x86_insn::X86_INS_SUB)
            return std::make_unique<Dyn_X86_INS_SUB>(isStatic);

        if (id == x86::x86_insn::X86_INS_AND)
            return std::make_unique<Dyn_X86_INS_AND>(isStatic);

        if (id == x86::x86_insn::X86_INS_ADC)
            return std::make_unique<Dyn_X86_INS_ADC>(isStatic);

        if (id == x86::x86_insn::X86_INS_ROR)
            return std::make_unique<Dyn_X86_INS_ROR>(isStatic);

        if (id == x86::x86_insn::X86_INS_ROL)
            return std::make_unique<Dyn_X86_INS_ROL>(isStatic);

        if (id == x86::x86_insn::X86_INS_OR)
            return std::make_unique<Dyn_X86_INS_OR>(isStatic);

        if (id == x86::x86_insn::X86_INS_XOR)
            return std::make_unique<Dyn_X86_INS_XOR>(isStatic);

        if (id == x86::x86_insn::X86_INS_SHL)
            return std::make_unique<Dyn_X86_INS_SHL>(isStatic);

        if (id == x86::x86_insn::X86_INS_SHR)
            return std::make_unique<Dyn_X86_INS_SHR>(isStatic);

        if (id == x86::x86_insn::X86_INS_SAR)
            return std::make_unique<Dyn_X86_INS_SAR>(isStatic);

        if (id == x86::x86_insn::X86_INS_CALL)
            return std::make_unique<Dyn_X86_INS_CALL>(isStatic);

        if (id == x86::x86_insn::X86_INS_RET)
            return std::make_unique<Dyn_X86_INS_RET>(isStatic);

        if (id == x86::x86_insn::X86_INS_LEAVE)
            return std::make_unique<Dyn_X86_INS_LEAVE>(isStatic);

        if (id == x86::x86_insn::X86_INS_ENTER)
            return std::make_unique<Dyn_X86_INS_ENTER>(isStatic);

        if (id == x86::x86_insn::X86_INS_DIV)
            return std::make_unique<Dyn_X86_INS_DIV>(isStatic);

        if (id == x86::x86_insn::X86_INS_TEST)
            return std::make_unique<Dyn_X86_INS_TEST>(isStatic);

        if (id == x86::x86_insn::X86_INS_CMP)
            return std::make_unique<Dyn_X86_INS_CMP>(isStatic);

        if (id == x86::x86_insn::X86_INS_JMP)
            return std::make_unique<Dyn_X86_INS_JMP>(isStatic);

        if (id == x86::x86_insn::X86_INS_JA)
            return std::make_unique<Dyn_X86_INS_JA>(isStatic);

        if (id == x86::x86_insn::X86_INS_JAE)
            return std::make_unique<Dyn_X86_INS_JAE>(isStatic);

        if (id == x86::x86_insn::X86_INS_JB)
            return std::make_unique<Dyn_X86_INS_JB>(isStatic);

        if (id == x86::x86_insn::X86_INS_JBE)
            return std::make_unique<Dyn_X86_INS_JB>(isStatic);

        if (id == x86::x86_insn::X86_INS_JCXZ)
            return std::make_unique<Dyn_X86_INS_JC>(isStatic);

        if (id == x86::x86_insn::X86_INS_JE)
            return std::make_unique<Dyn_X86_INS_JE>(isStatic);


        if (id == x86::x86_insn::X86_INS_JG)
            return std::make_unique<Dyn_X86_INS_JG>(isStatic);

        if (id == x86::x86_insn::X86_INS_JGE)
            return std::make_unique<Dyn_X86_INS_JGE>(isStatic);


        if (id == x86::x86_insn::X86_INS_JL)
            return std::make_unique<Dyn_X86_INS_JL>(isStatic);

        if (id == x86::x86_insn::X86_INS_JLE)
            return std::make_unique<Dyn_X86_INS_JLE>(isStatic);

        /*
        if(id == x86::x86_insn::X86_INS_JNA)
            return std::make_unique<Dyn_X86_INS_JNA>(isStatic);



        if(id == x86::x86_insn::X86_INS_JNAE)
            return std::make_unique<Dyn_X86_INS_JNAE>(isStatic);


        */
        if(id == x86::x86_insn::X86_INS_JNB)
            return std::make_unique<Dyn_X86_INS_JNB>(isStatic);



        if (id == x86::x86_insn::X86_INS_JNBE)
            return std::make_unique<Dyn_X86_INS_JNBE>(isStatic);

        /*
        if(id == x86::x86_insn::X86_INS_JNC)
            return std::make_unique<Dyn_X86_INS_JNC>(isStatic);

         */

        if (id == x86::x86_insn::X86_INS_JNE)
            return std::make_unique<Dyn_X86_INS_JNE>(isStatic);

        /*

        if(id == x86::x86_insn::X86_INS_JNG)
            return std::make_unique<Dyn_X86_INS_JNG>(isStatic);


        if(id == x86::x86_insn::X86_INS_JNL)
            return std::make_unique<Dyn_X86_INS_JNL>(isStatic);

         */

        if (id == x86::x86_insn::X86_INS_JNO)
            return std::make_unique<Dyn_X86_INS_JNO>(isStatic);


        if (id == x86::x86_insn::X86_INS_JNS)
            return std::make_unique<Dyn_X86_INS_JNS>(isStatic);

        if (id == x86::x86_insn::X86_INS_JNZ)
            return std::make_unique<Dyn_X86_INS_JNZ>(isStatic);


        if (id == x86::x86_insn::X86_INS_JO)
            return std::make_unique<Dyn_X86_INS_JO>(isStatic);

        if (id == x86::x86_insn::X86_INS_JS)
            return std::make_unique<Dyn_X86_INS_JS>(isStatic);


        if (id == x86::x86_insn::X86_INS_JZ)
            return std::make_unique<Dyn_X86_INS_JZ>(isStatic);

        if (id == x86::x86_insn::X86_INS_STOSD)
            return std::make_unique<Dyn_X86_INS_REP_STOSD>(isStatic);

        if (id == x86::x86_insn::X86_INS_CMOVZ)
            return std::make_unique<Dyn_X86_INS_CMOVZ>(isStatic);

        if (id == x86::x86_insn::X86_INS_SETZ)
            return std::make_unique<Dyn_X86_INS_SETZ>(isStatic);

        if (id == x86::x86_insn::X86_INS_SETNZ)
            return std::make_unique<Dyn_X86_INS_SETNZ>(isStatic);


        WARN("unrecognized instructions");
        std::cout << x86::insn_id2string(id) << std::endl;
        return std::make_unique<Inst_Base>(isStatic);
    }

    std::shared_ptr<BitVector> inst_dyn_details::two_operand(SEEngine *se, Inst_Base *inst, BVOper bvoper) {
        std::shared_ptr<Operand> op0 = inst->oprd[0];
        std::shared_ptr<Operand> op1 = inst->oprd[1];
        std::shared_ptr<BitVector> v1, v0, res;
        auto opcode_id = inst->instruction_id;
        auto opcstr = "bv" + x86::insn_id2string(opcode_id);

        if (op1->type == Operand::ImmValue) {
            uint32_t temp_concrete = stoul(op1->field[0], nullptr, 16);
            v1 = std::make_shared<BitVector>(ValueType::CONCRETE, temp_concrete, se->isImmSym(temp_concrete));
        } else if (op1->type == Operand::Reg) {
            v1 = se->readReg(op1->field[0]);
        } else if (op1->type == Operand::Mem) {
            v1 = se->readMem(inst->get_memory_address(), op1->bit);
        } else {
            ERROR("other instructions: op1 is not ImmValue, Reg, or Mem!");
        }

        if (op0->type == Operand::Reg) { // dest op is reg
            v0 = se->readReg(op0->field[0]);
            res = buildop2(bvoper, v0, v1);
            se->writeReg(op0->field[0], res);
            return res;
        } else if (op0->type == Operand::Mem) { // dest op is mem
            v0 = se->readMem(inst->get_memory_address(), op0->bit);
            res = buildop2(bvoper, v0, v1);
            se->writeMem(inst->get_memory_address(), op0->bit, res);
            return res;
        } else {
            ERROR("other instructions: op2 is not ImmValue, Reg, or Mem!");
        }

        return nullptr;
    }

    bool Dyn_X86_INS_NOP::symbolic_execution(tana::SEEngine *se) {
        return true;
    }


    bool Dyn_X86_INS_PUSH::symbolic_execution(SEEngine *se) {
        std::shared_ptr<Operand> op0 = this->oprd[0];
        std::shared_ptr<BitVector> v0;
        std::shared_ptr<BitVector> esp = se->readReg("esp");
        uint32_t dec = op0->bit / T_BYTE_SIZE;
        esp = buildop2(BVOper::bvsub, esp, dec);
        se->writeReg("esp", esp);
        if (op0->type == Operand::ImmValue) {
            uint32_t temp_concrete = stoul(op0->field[0], nullptr, 16);
            v0 = std::make_shared<BitVector>(ValueType::CONCRETE, temp_concrete, se->isImmSym(temp_concrete));
            se->writeMem(this->get_memory_address(), op0->bit, v0);
            return true;
        }

        if (op0->type == Operand::Reg) {
            auto regV = se->readReg(op0->field[0]);
            se->writeMem(this->get_memory_address(), op0->bit, regV);
            return true;
        }
        if (op0->type == Operand::Mem) {
            // The memaddr in the trace is the read address
            // We need to compute the write address
            auto reg = Registers::convert2RegID("esp");
            uint32_t esp_index = Registers::getRegIndex(reg);
            uint32_t esp_value = this->vcpu.gpr[esp_index];
            v0 = se->readMem(this->get_memory_address(), op0->bit);
            auto mem_esp_v = esp_value - 4;
            std::stringstream sstream;
            sstream << std::hex << mem_esp_v << std::dec;
            se->writeMem(sstream.str(), op0->bit, v0);
            return true;
        }

        ERROR("push error: the operand is not Imm, Reg or Mem!");
        return false;
    }

    bool Dyn_X86_INS_POP::symbolic_execution(SEEngine *se) {
        std::shared_ptr<Operand> op0 = this->oprd[0];

        std::shared_ptr<BitVector> esp = se->readReg("esp");
        uint32_t add_size = op0->bit / T_BYTE_SIZE;
        esp = buildop2(BVOper::bvadd, esp, add_size);

        se->writeReg("esp", esp);
        if (op0->type == Operand::Reg) {
            assert(Registers::getRegType(op0->field[0]) == FULL);
            auto v0 = se->readMem(this->get_memory_address(), op0->bit);
            se->writeReg(op0->field[0], v0);
            return true;
        }

        ERROR("pop error: the operand is not Reg!");
        return false;

    }

    bool Dyn_X86_INS_NEG::symbolic_execution(SEEngine *se) {
        std::shared_ptr<Operand> op0 = this->oprd[0];
        auto opcode_id = this->instruction_id;
        auto opcstr = "bv" + x86::insn_id2string(opcode_id);
        std::shared_ptr<BitVector> v0, res;
        bool status = false;
        uint32_t size = 0;


        if (op0->type == Operand::Reg) {
            size = Registers::getRegSize(op0->field[0]);
            v0 = se->readReg(op0->field[0]);
            res = buildop1(BVOper::bvneg, v0);
            se->writeReg(op0->field[0], res);
            status = true;
        }

        if (op0->type == Operand::Mem) {
            size = op0->bit;
            v0 = se->readMem(this->get_memory_address(), op0->bit);
            res = buildop1(BVOper::bvneg, v0);
            se->writeMem(this->get_memory_address(), op0->bit, res);
            status = true;
        }

        if (se->eflags) {
            auto zero = std::make_shared<BitVector>(ValueType::CONCRETE, 0);
            // Update CF
            updateCFsub(se, zero, v0);

            // Update SF
            updateSF(se, res, size);

            // Update ZF
            updateZF(se, res);

            // Update OF
            updateOFsub(se, zero, v0, op0->bit);
        }

        if (status)
            return true;

        ERROR("neg error: the operand is not Reg!");
        return false;
    }

    bool Dyn_X86_INS_NOT::symbolic_execution(SEEngine *se) {
        std::shared_ptr<Operand> op0 = this->oprd[0];

        std::shared_ptr<BitVector> v0, res;
        bool status = false;


        if (op0->type == Operand::Reg) {
            assert(Registers::getRegType(op0->field[0]) == FULL);
            v0 = se->readReg(op0->field[0]);
            res = buildop1(BVOper::bvnot, v0);
            se->writeReg(op0->field[0], res);
            status = true;
        }

        if (status)
            return true;

        ERROR("neg error: the operand is not Reg!");
        return false;
    }

    bool Dyn_X86_INS_INC::symbolic_execution(SEEngine *se) {
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

        auto bit_vec_one = std::make_shared<BitVector>(ValueType::CONCRETE, 1);
        res = buildop2(BVOper::bvadd, v0, bit_vec_one);

        if (op0->type == Operand::Reg) {
            se->writeReg(op0->field[0], res);
        }

        if (op0->type == Operand::Mem) {
            //memory[it->memory_address] = res;
            se->writeMem(this->get_memory_address(), op0->bit, res);
        }

        if (se->eflags) {
            // Update CF
            // CF is not affected

            // Update SF
            updateSF(se, res, op0->bit);

            // Update ZF
            updateZF(se, res);

            // Update OF
            auto one = std::make_shared<BitVector>(ValueType::CONCRETE, 1);
            updateOFadd(se, one, v0, op0->bit);

        }

        return true;

    }

    bool Dyn_X86_INS_DEC::symbolic_execution(SEEngine *se) {
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

        auto bit_vec_one = std::make_shared<BitVector>(ValueType::CONCRETE, 1);
        res = buildop2(BVOper::bvsub, v0, bit_vec_one);

        if (op0->type == Operand::Reg) {
            se->writeReg(op0->field[0], res);
        }

        if (op0->type == Operand::Mem) {
            se->writeMem(this->get_memory_address(), op0->bit, res);
        }

        if (se->eflags) {
            // Update CF
            // CF is not affected

            // Update SF
            updateSF(se, res, op0->bit);

            // Update ZF
            updateZF(se, res);

            // Update OF
            auto one = std::make_shared<BitVector>(ValueType::CONCRETE, 1);
            updateOFadd(se, v0, one, op0->bit);

        }

        return true;

    }


    bool Dyn_X86_INS_MOVSX::symbolic_execution(SEEngine *se) {
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

    bool Dyn_X86_INS_MOVZX::symbolic_execution(SEEngine *se) {
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
            v1 = se->readMem(this->get_memory_address(), op1->bit);
            v1 = se->ZeroExt(v1, op0->bit);
            se->writeReg(op0->field[0], v1);
            return true;
        }

        ERROR("MOVZX");
        return false;
    }

    bool Dyn_X86_INS_CMOVB::symbolic_execution(SEEngine *se) {
        std::shared_ptr<Operand> op0 = this->oprd[0];
        std::shared_ptr<Operand> op1 = this->oprd[1];
        std::shared_ptr<BitVector> v0, v1, res;
        auto opcode_id = this->instruction_id;
        bool CF = false;

        if (!(this->vcpu.eflags_state))
            WARN("CMOVB doesn't have eflags information");

        CF = this->vcpu.CF();
        if (!CF)
            return true;

        if (op0->type == Operand::Reg) {
            if (op1->type == Operand::ImmValue) { // mov reg, 0x1111
                uint32_t temp_concrete = stoul(op1->field[0], 0, 16);
                v1 = std::make_shared<BitVector>(ValueType::CONCRETE, temp_concrete, se->isImmSym(temp_concrete));
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
                se->writeMem(this->get_memory_address(), op0->bit,
                             std::make_shared<BitVector>(ValueType::CONCRETE, temp_concrete,
                                                         se->isImmSym(temp_concrete)));
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

    bool Dyn_X86_INS_MOV::symbolic_execution(SEEngine *se) {
        std::shared_ptr<Operand> op0 = this->oprd[0];
        std::shared_ptr<Operand> op1 = this->oprd[1];
        std::shared_ptr<BitVector> v0, v1, res;

        if (op0->type == Operand::Reg) {
            if (op1->type == Operand::ImmValue) { // mov reg, 0x1111
                uint32_t temp_concrete = stoul(op1->field[0], 0, 16);
                v1 = std::make_shared<BitVector>(ValueType::CONCRETE, temp_concrete, se->isImmSym(temp_concrete));
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
                se->writeMem(this->get_memory_address(), op0->bit,
                             std::make_shared<BitVector>(ValueType::CONCRETE, temp_concrete,
                                                         se->isImmSym(temp_concrete)));
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

    bool Dyn_X86_INS_LEA::symbolic_execution(SEEngine *se) {
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
                    res = buildop2(BVOper::bvadd, f0, f1);
                    //ctx[getRegName(op0->field[0])] = res;
                    se->writeReg(op0->field[0], res);
                    return true;
                }
                uint32_t temp_concrete = stoul(op1->field[2], 0, 16);
                f2 = std::make_shared<BitVector>(ValueType::CONCRETE, temp_concrete, se->isImmSym(temp_concrete));
                res = buildop2(BVOper::bvimul, f1, f2);
                res = buildop2(BVOper::bvadd, f0, res);
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
                f2 = std::make_shared<BitVector>(ValueType::CONCRETE, temp_concrete1,
                                                 se->isImmSym(temp_concrete1));   //2
                std::string sign = op1->field[3];          //+
                uint32_t temp_concrete2 = stoul(op1->field[4], 0, 16);
                f3 = std::make_shared<BitVector>(ValueType::CONCRETE, temp_concrete2,
                                                 se->isImmSym(temp_concrete2));   //0xfffff1
                assert((sign == "+") || (sign == "-"));
                if (op1->field[2] == "1") {
                    res = buildop2(BVOper::bvadd, f0, f1);
                } else {
                    res = buildop2(BVOper::bvimul, f1, f2);
                    res = buildop2(BVOper::bvadd, f0, res);
                }
                if (sign == "+")
                    res = buildop2(BVOper::bvadd, res, f3);
                else
                    res = buildop2(BVOper::bvsub, res, f3);
                //ctx[getRegName(op0->field[0])] = res;

                se->writeReg(op0->field[0], res);

                return true;
            }
            case 4: {
                std::shared_ptr<BitVector> f0, f1; // addr4: eax+0xfffff1
                //f0 = ctx[getRegName(op1->field[0])];       //eax
                f0 = se->readReg(op1->field[0]);
                uint32_t temp_concrete = stoul(op1->field[2], 0, 16);
                f1 = std::make_shared<BitVector>(ValueType::CONCRETE, temp_concrete,
                                                 se->isImmSym(temp_concrete));   //0xfffff1
                std::string sign = op1->field[1];          //+
                if (sign == "+")
                    res = buildop2(BVOper::bvadd, f0, f1);
                else
                    res = buildop2(BVOper::bvsub, f0, f1);
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

                f1 = std::make_shared<BitVector>(ValueType::CONCRETE, temp_concrete1, se->isImmSym(temp_concrete1));
                f2 = std::make_shared<BitVector>(ValueType::CONCRETE, temp_concrete2, se->isImmSym(temp_concrete2));
                std::string sign = op1->field[2];
                if (op1->field[1] == "1") {
                    res = f0;
                } else {
                    res = buildop2(BVOper::bvimul, f0, f1);
                }
                if (sign == "+")
                    res = buildop2(BVOper::bvadd, res, f2);
                else
                    res = buildop2(BVOper::bvsub, res, f2);
                se->writeReg(op0->field[0], res);
                return true;
            }
            case 3: {
                std::shared_ptr<BitVector> f0, f1;          // addr3: eax*2
                //f0 = ctx[getRegName(op1->field[0])];
                f0 = se->readReg(op1->field[0]);

                uint32_t temp_concrete = stoul(op1->field[1]);
                f1 = std::make_shared<BitVector>(ValueType::CONCRETE, temp_concrete, se->isImmSym(temp_concrete));
                res = buildop2(BVOper::bvimul, f0, f1);
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
                f0 = std::make_shared<BitVector>(ValueType::CONCRETE, temp_concrete, se->isImmSym(temp_concrete));
                se->writeReg(op0->field[0], f0);
                return true;
            }
            default:
                ERROR("Other tags in addr is not ready for lea!");
                return false;
        }

    }


    bool Dyn_X86_INS_XCHG::symbolic_execution(SEEngine *se) {
        std::shared_ptr<Operand> op0 = this->oprd[0];
        std::shared_ptr<Operand> op1 = this->oprd[1];
        std::shared_ptr<BitVector> v0, v1, res;
        auto opcode_id = this->instruction_id;

        if (op1->type == Operand::Reg) {
            v1 = se->readReg(op1->field[0]);
            if (op0->type == Operand::Reg) {
                v0 = se->readReg(op0->field[0]);
                v1 = se->readReg(op1->field[0]);

                se->writeReg(op1->field[0], v0);
                se->writeReg(op0->field[0], v1);

                return true;
            }
            if (op0->type == Operand::Mem) {
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

    bool Dyn_X86_INS_SBB::symbolic_execution(SEEngine *se) {
        std::shared_ptr<Operand> op0 = this->oprd[0];
        std::shared_ptr<Operand> op1 = this->oprd[1];

        std::shared_ptr<BitVector> v0, v1, res;
        bool flags = false;
        if (!this->vcpu.eflags_state) {
            WARN("SBB doesn't have eflags information");
            return false;
        }
        auto CF = this->vcpu.CF();

        if (op1->type == Operand::Reg) {
            v1 = se->readReg(op1->field[0]);
        }

        if (op1->type == Operand::Mem) {
            v1 = se->readMem(this->get_memory_address(), op1->bit);
        }

        if (op1->type == Operand::ImmValue) {
            v1 = std::make_shared<BitVector>(ValueType::CONCRETE, op1->field[0]);
        }


        if (op0->type == Operand::Reg) {
            v0 = se->readReg(op0->field[0]);
            flags = true;
        }

        if (op0->type == Operand::Mem) {
            v0 = se->readMem(this->get_memory_address(), op1->bit);
            flags = true;
        }

        if (CF) {
            auto one_bit = std::make_shared<BitVector>(ValueType::CONCRETE, 1);
            res = buildop2(BVOper::bvsub, v0, v1);
            res = buildop2(BVOper::bvsub, res, one_bit);
        } else {
            res = buildop2(BVOper::bvsub, v0, v1);
        }

        if (op0->type == Operand::Reg) {
            se->writeReg(op0->field[0], res);
        }

        if (op0->type == Operand::Mem) {
            se->writeMem(this->get_memory_address(), op0->bit, res);
        }

        if (se->eflags) {
            // Update CF
            updateCFsub(se, v0, v1);

            // Update SF
            updateSF(se, res, op0->bit);

            // Update ZF
            updateZF(se, res);

            // Update OF
            updateOFsub(se, v0, v1, op0->bit);
        }

        if (flags)
            return true;

        return false;
    }


    bool Dyn_X86_INS_IMUL::symbolic_execution(SEEngine *se) {
        std::shared_ptr<Operand> op0 = this->oprd[0];
        std::shared_ptr<Operand> op1 = this->oprd[1];
        std::shared_ptr<Operand> op2 = this->oprd[2];
        std::shared_ptr<BitVector> v1, v2, v3, res;
        auto opcode_id = this->instruction_id;
        auto opcstr = "bv" + x86::insn_id2string(opcode_id);
        if (this->get_operand_number() == 1) {
            //TODO
            return true;
        }

        if (this->get_operand_number() == 2) {
            //TODO
            return true;
        }


        if (op0->type == Operand::Reg &&
            op1->type == Operand::Reg &&
            op2->type == Operand::ImmValue) { // imul reg, reg, imm
            v1 = se->readReg(op1->field[0]);
            uint32_t temp_concrete = stoul(op2->field[0], 0, 16);
            v2 = std::make_shared<BitVector>(ValueType::CONCRETE, temp_concrete, se->isImmSym(temp_concrete));
            res = buildop2(BVOper::bvimul, v1, v2);
            se->writeReg(op0->field[0], res);
            return true;
        }

        ERROR("three operands instructions other than imul are not handled!");
        return false;

    }


    bool Dyn_X86_INS_SHLD::symbolic_execution(SEEngine *se) {
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
            v3 = std::make_shared<BitVector>(ValueType::CONCRETE, temp_concrete, se->isImmSym(temp_concrete));
            res = buildop3(BVOper::bvshld, v1, v2, v3);
            se->writeReg(op0->field[0], res);
            return true;
        }

        ERROR("shld is not handled!");
        return false;

    }

    bool Dyn_X86_INS_SHRD::symbolic_execution(SEEngine *se) {
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
            v3 = std::make_shared<BitVector>(ValueType::CONCRETE, temp_concrete, se->isImmSym(temp_concrete));
            res = buildop3(BVOper::bvshrd, v1, v2, v3);
            se->writeReg(op0->field[0], res);
            return true;
        }

        ERROR("shrd is not handled!");
        return false;
    }

    bool Dyn_X86_INS_ADD::symbolic_execution(SEEngine *se) {

        std::shared_ptr<Operand> op0 = this->oprd[0];
        std::shared_ptr<Operand> op1 = this->oprd[1];
        std::shared_ptr<BitVector> v1, v0, res;
        auto opcode_id = this->instruction_id;

        if (op1->type == Operand::ImmValue) {
            uint32_t temp_concrete = stoul(op1->field[0], nullptr, 16);
            v1 = std::make_shared<BitVector>(ValueType::CONCRETE, temp_concrete, se->isImmSym(temp_concrete));
        } else if (op1->type == Operand::Reg) {
            v1 = se->readReg(op1->field[0]);
        } else if (op1->type == Operand::Mem) {
            v1 = se->readMem(this->get_memory_address(), op1->bit);
        } else {
            ERROR("other instructions: op1 is not ImmValue, Reg, or Mem!");
        }

        if (op0->type == Operand::Reg) { // dest op is reg
            v0 = se->readReg(op0->field[0]);
            res = buildop2(BVOper::bvadd, v0, v1);
            se->writeReg(op0->field[0], res);
        } else if (op0->type == Operand::Mem) { // dest op is mem
            v0 = se->readMem(this->get_memory_address(), op0->bit);
            res = buildop2(BVOper::bvadd, v0, v1);
            se->writeMem(this->get_memory_address(), op0->bit, res);
        } else {
            ERROR("other instructions: op2 is not ImmValue, Reg, or Mem!");
        }

        if (se->eflags) {
            // Update CF
            updateCFadd(se, res, op0->bit);

            // Update SF
            updateSF(se, res, op0->bit);

            // Update ZF
            updateZF(se, res);

            // Update OF
            updateOFadd(se, v0, v1, op0->bit);

        }

        return true;
    }

    bool Dyn_X86_INS_SUB::symbolic_execution(SEEngine *se) {
        std::shared_ptr<Operand> op0 = this->oprd[0];
        std::shared_ptr<Operand> op1 = this->oprd[1];
        std::shared_ptr<BitVector> v1, v0, res;
        auto opcode_id = this->instruction_id;
        auto opcstr = "bv" + x86::insn_id2string(opcode_id);

        if (op1->type == Operand::ImmValue) {
            uint32_t temp_concrete = stoul(op1->field[0], nullptr, 16);
            v1 = std::make_shared<BitVector>(ValueType::CONCRETE, temp_concrete, se->isImmSym(temp_concrete));
        } else if (op1->type == Operand::Reg) {
            v1 = se->readReg(op1->field[0]);
        } else if (op1->type == Operand::Mem) {
            v1 = se->readMem(this->get_memory_address(), op1->bit);
        } else {
            ERROR("other instructions: op1 is not ImmValue, Reg, or Mem!");
        }

        if (op0->type == Operand::Reg) { // dest op is reg
            v0 = se->readReg(op0->field[0]);
            res = buildop2(BVOper::bvsub, v0, v1);
            se->writeReg(op0->field[0], res);
        } else if (op0->type == Operand::Mem) { // dest op is mem
            v0 = se->readMem(this->get_memory_address(), op0->bit);
            res = buildop2(BVOper::bvsub, v0, v1);
            se->writeMem(this->get_memory_address(), op0->bit, res);
        } else {
            ERROR("other instructions: op2 is not ImmValue, Reg, or Mem!");
        }

        if (se->eflags) {
            // Update CF
            updateCFsub(se, v0, v1);

            // Update SF
            updateSF(se, res, op0->bit);

            // Update ZF
            updateZF(se, res);

            // Update OF
            updateOFsub(se, v0, v1, op0->bit);

        }

        return true;
    }

    bool Dyn_X86_INS_AND::symbolic_execution(SEEngine *se) {
        std::shared_ptr<Operand> op0 = this->oprd[0];
        std::shared_ptr<Operand> op1 = this->oprd[1];
        std::shared_ptr<BitVector> v1, v0, res;
        auto opcode_id = this->instruction_id;
        auto opcstr = "bv" + x86::insn_id2string(opcode_id);

        if (op1->type == Operand::ImmValue) {
            uint32_t temp_concrete = stoul(op1->field[0], nullptr, 16);
            v1 = std::make_shared<BitVector>(ValueType::CONCRETE, temp_concrete, se->isImmSym(temp_concrete));
        } else if (op1->type == Operand::Reg) {
            v1 = se->readReg(op1->field[0]);
        } else if (op1->type == Operand::Mem) {
            v1 = se->readMem(this->get_memory_address(), op1->bit);
        } else {
            ERROR("other instructions: op1 is not ImmValue, Reg, or Mem!");
        }

        if (op0->type == Operand::Reg) { // dest op is reg
            v0 = se->readReg(op0->field[0]);
            res = buildop2(BVOper::bvand, v0, v1);
            se->writeReg(op0->field[0], res);
        } else if (op0->type == Operand::Mem) { // dest op is mem
            v0 = se->readMem(this->get_memory_address(), op0->bit);
            res = buildop2(BVOper::bvand, v0, v1);
            se->writeMem(this->get_memory_address(), op0->bit, res);
        } else {
            ERROR("other instructions: op2 is not ImmValue, Reg, or Mem!");
        }

        if (se->eflags) {
            // Update CF
            se->clearFlags("CF");

            // Update SF
            updateSF(se, res, op0->bit);

            // Update ZF
            updateZF(se, res);

            // Update OF
            se->clearFlags("OF");
        }

        return true;

    }

    bool Dyn_X86_INS_ADC::symbolic_execution(SEEngine *se) {

        bool CF = false, status = false;
        if (this->vcpu.eflags_state)
            CF = this->vcpu.CF();

        std::shared_ptr<Operand> op0 = this->oprd[0];
        std::shared_ptr<Operand> op1 = this->oprd[1];
        std::shared_ptr<BitVector> v1, v0, res;
        auto opcode_id = this->instruction_id;
        auto opcstr = x86::insn_id2string(opcode_id);

        std::shared_ptr<BitVector> v_one = std::make_shared<BitVector>(ValueType::CONCRETE, 1, se->isImmSym(1));

        if (op1->type == Operand::ImmValue) {
            uint32_t temp_concrete = stoul(op1->field[0], 0, 16);
            v1 = std::make_shared<BitVector>(ValueType::CONCRETE, temp_concrete, se->isImmSym(temp_concrete));
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
            res = buildop2(BVOper::bvadd, v0, v1);
            if (CF) {
                res = buildop2(BVOper::bvadd, res, v_one);
            }
            se->writeReg(op0->field[0], res);
            status = true;
        } else if (op0->type == Operand::Mem) { // dest op is mem
            v0 = se->readMem(this->get_memory_address(), op0->bit);
            res = buildop2(BVOper::bvadd, v0, v1);
            if (CF) {
                res = buildop2(BVOper::bvadd, res, v_one);
            }
            se->writeMem(this->get_memory_address(), op0->bit, res);
            status = true;
        } else {
            ERROR("other instructions: op2 is not ImmValue, Reg, or Mem!");
            return false;
        }

        if (se->eflags) {
            // Update CF
            updateCFadd(se, res, op0->bit);

            // Update SF
            updateSF(se, res, op0->bit);

            // Update ZF
            updateZF(se, res);

            // Update OF
            updateOFadd(se, v0, v1, op0->bit);
        }


        return true;

    }

    bool Dyn_X86_INS_ROR::symbolic_execution(SEEngine *se) {
        auto res = inst_dyn_details::two_operand(se, this, BVOper::bvror);
        return true;
    }

    bool Dyn_X86_INS_ROL::symbolic_execution(SEEngine *se) {
        auto res = inst_dyn_details::two_operand(se, this, BVOper::bvrol);
        return true;
    }

    bool Dyn_X86_INS_SHL::symbolic_execution(SEEngine *se) {
        auto res = inst_dyn_details::two_operand(se, this, BVOper::bvshl);
        return true;
    }

    bool Dyn_X86_INS_SHR::symbolic_execution(SEEngine *se) {
        auto res = inst_dyn_details::two_operand(se, this, BVOper::bvshr);
        return true;
    }

    bool Dyn_X86_INS_SAR::symbolic_execution(SEEngine *se) {
        auto res = inst_dyn_details::two_operand(se, this, BVOper::bvsar);
        return true;
    }

    bool Dyn_X86_INS_OR::symbolic_execution(SEEngine *se) {
        std::shared_ptr<Operand> op0 = this->oprd[0];
        std::shared_ptr<Operand> op1 = this->oprd[1];
        std::shared_ptr<BitVector> v1, v0, res;
        auto opcode_id = this->instruction_id;

        if (op1->type == Operand::ImmValue) {
            uint32_t temp_concrete = stoul(op1->field[0], nullptr, 16);
            v1 = std::make_shared<BitVector>(ValueType::CONCRETE, temp_concrete, se->isImmSym(temp_concrete));
        } else if (op1->type == Operand::Reg) {
            v1 = se->readReg(op1->field[0]);
        } else if (op1->type == Operand::Mem) {
            v1 = se->readMem(this->get_memory_address(), op1->bit);
        } else {
            ERROR("other instructions: op1 is not ImmValue, Reg, or Mem!");
        }

        if (op0->type == Operand::Reg) { // dest op is reg
            v0 = se->readReg(op0->field[0]);
            res = buildop2(BVOper::bvor, v0, v1);
            se->writeReg(op0->field[0], res);
        } else if (op0->type == Operand::Mem) { // dest op is mem
            v0 = se->readMem(this->get_memory_address(), op0->bit);
            res = buildop2(BVOper::bvor, v0, v1);
            se->writeMem(this->get_memory_address(), op0->bit, res);
        } else {
            ERROR("other instructions: op2 is not ImmValue, Reg, or Mem!");
        }

        if (se->eflags) {
            // Update CF
            se->clearFlags("CF");

            // Update SF
            updateSF(se, res, op0->bit);

            // Update ZF
            updateZF(se, res);

            // Update OF
            se->clearFlags("OF");
        }

        return true;
    }

    bool Dyn_X86_INS_XOR::symbolic_execution(SEEngine *se) {
        std::shared_ptr<Operand> op0 = this->oprd[0];
        std::shared_ptr<Operand> op1 = this->oprd[1];
        std::shared_ptr<BitVector> v1, v0, res;
        auto opcode_id = this->instruction_id;
        auto opcstr = "bv" + x86::insn_id2string(opcode_id);

        if (op1->type == Operand::ImmValue) {
            uint32_t temp_concrete = stoul(op1->field[0], nullptr, 16);
            v1 = std::make_shared<BitVector>(ValueType::CONCRETE, temp_concrete, se->isImmSym(temp_concrete));
        } else if (op1->type == Operand::Reg) {
            v1 = se->readReg(op1->field[0]);
        } else if (op1->type == Operand::Mem) {
            v1 = se->readMem(this->get_memory_address(), op1->bit);
        } else {
            ERROR("other instructions: op1 is not ImmValue, Reg, or Mem!");
        }

        if (op0->type == Operand::Reg) { // dest op is reg
            v0 = se->readReg(op0->field[0]);
            res = buildop2(BVOper::bvxor, v0, v1);
            se->writeReg(op0->field[0], res);
        } else if (op0->type == Operand::Mem) { // dest op is mem
            v0 = se->readMem(this->get_memory_address(), op0->bit);
            res = buildop2(BVOper::bvxor, v0, v1);
            se->writeMem(this->get_memory_address(), op0->bit, res);
        } else {
            ERROR("other instructions: op2 is not ImmValue, Reg, or Mem!");
        }

        if (se->eflags) {
            // Update CF
            se->clearFlags("CF");

            // Update SF
            updateSF(se, res, op0->bit);

            // Update ZF
            updateZF(se, res);

            // Update OF
            se->clearFlags("OF");
        }

        return true;
    }

    bool Dyn_X86_INS_CALL::symbolic_execution(SEEngine *se) {
        std::shared_ptr<BitVector> esp = se->readReg("esp");
        uint32_t sub_size = 4;
        esp = buildop2(BVOper::bvsub, esp, sub_size);
        se->writeReg("esp", esp);
        uint32_t eip = this->addrn + 5; // X86 call 0xf77268ed
        auto v_eip = std::make_shared<BitVector>(ValueType::CONCRETE, eip);

        se->writeMem(this->get_memory_address(), REGISTER_SIZE, v_eip);
        return true;
    }

    bool Dyn_X86_INS_RET::symbolic_execution(SEEngine *se) {
        std::shared_ptr<BitVector> esp = se->readReg("esp");
        uint32_t add_size = 4;
        if(oprd[0] != nullptr)
        {
            assert(oprd[0]->type == Operand::ImmValue);
            add_size = add_size + std::stoul(oprd[0]->field[0], nullptr, 16);
        }
        esp = buildop2(BVOper::bvadd, esp, add_size);
        se->writeReg("esp", esp);
        return true;
    }

    bool Dyn_X86_INS_LEAVE::symbolic_execution(SEEngine *se) {

        // EBP = ESP
        auto v_ebp = se->readReg("ebp");
        se->writeReg("esp", v_ebp);

        // POP EBP
        auto v0 = se->readMem(this->get_memory_address(), REGISTER_SIZE);
        se->writeReg("ebp", v0);

        return true;
    }

    bool Dyn_X86_INS_ENTER::symbolic_execution(SEEngine *se) {
        return true;
    }

    bool Dyn_X86_INS_DIV::symbolic_execution(SEEngine *se) {
        std::shared_ptr<Operand> op0 = this->oprd[0];
        auto operand_size = op0->bit;
        assert(operand_size == 8 || operand_size == 16 || operand_size == 32);
        std::shared_ptr<BitVector> dividend, divisor, quotient, remainder;
        if (operand_size == 8) {
            dividend = se->readReg("ax");
        }
        if (operand_size == 16) {
            auto temp1 = se->readReg("ax");
            auto temp2 = se->readReg("dx");
            dividend = se->Concat(temp2, temp1);
        }
        if (operand_size == 32) {
            auto temp1 = se->readReg("eax");
            auto temp2 = se->readReg("edx");
            dividend = se->Concat(temp2, temp1);

        }


        if (op0->type == Operand::Mem) {
            divisor = se->readMem(get_memory_address(), op0->bit);
        }

        if (op0->type == Operand::Reg) {
            divisor = se->readReg(op0->field[0]);
        }

        quotient = buildop2(BVOper::bvquo, dividend, divisor);
        remainder = buildop2(BVOper::bvrem, dividend, divisor);

        if (operand_size == 8) {
            se->writeReg("al", quotient);
            se->writeReg("ah", remainder);
        }

        if (operand_size == 16) {
            se->writeReg("ax", quotient);
            se->writeReg("dx", remainder);
        }

        if (operand_size == 32) {
            se->writeReg("eax", quotient);
            se->writeReg("edx", remainder);
        }

        return true;

    }

    // The And instrucntion without storing the result
    bool Dyn_X86_INS_TEST::symbolic_execution(tana::SEEngine *se) {
        std::shared_ptr<Operand> op0 = this->oprd[0];
        std::shared_ptr<Operand> op1 = this->oprd[1];
        std::shared_ptr<BitVector> v1, v0, res;
        auto opcstr = "bvand";

        if (op1->type == Operand::ImmValue) {
            uint32_t temp_concrete = stoul(op1->field[0], nullptr, 16);
            v1 = std::make_shared<BitVector>(ValueType::CONCRETE, temp_concrete, se->isImmSym(temp_concrete));
        } else if (op1->type == Operand::Reg) {
            v1 = se->readReg(op1->field[0]);
        } else if (op1->type == Operand::Mem) {
            v1 = se->readMem(this->get_memory_address(), op1->bit);
        } else {
            ERROR("other instructions: op1 is not ImmValue, Reg, or Mem!");
        }

        if (op0->type == Operand::Reg) { // dest op is reg
            v0 = se->readReg(op0->field[0]);
            res = buildop2(BVOper::bvand, v0, v1);
        } else if (op0->type == Operand::Mem) { // dest op is mem
            v0 = se->readMem(this->get_memory_address(), op0->bit);
            res = buildop2(BVOper::bvand, v0, v1);
        } else {
            ERROR("other instructions: op2 is not ImmValue, Reg, or Mem!");
        }

        if (se->eflags) {
            // Update CF
            se->clearFlags("CF");

            // Update SF
            updateSF(se, res, op0->bit);

            // Update ZF
            updateZF(se, res);

            // Update OF
            se->clearFlags("OF");
        }

        return true;
    }

    bool Dyn_X86_INS_CMP::symbolic_execution(tana::SEEngine *se) {
        std::shared_ptr<Operand> op0 = this->oprd[0];
        std::shared_ptr<Operand> op1 = this->oprd[1];
        std::shared_ptr<BitVector> v1, v0, res;
        std::string opcstr = "bvsub";

        if (op1->type == Operand::ImmValue) {
            uint32_t temp_concrete = stoul(op1->field[0], nullptr, 16);
            v1 = std::make_shared<BitVector>(ValueType::CONCRETE, temp_concrete, se->isImmSym(temp_concrete));
        } else if (op1->type == Operand::Reg) {
            v1 = se->readReg(op1->field[0]);
        } else if (op1->type == Operand::Mem) {
            v1 = se->readMem(this->get_memory_address(), op1->bit);
        } else {
            ERROR("other instructions: op1 is not ImmValue, Reg, or Mem!");
        }

        if (op0->type == Operand::Reg) { // dest op is reg
            v0 = se->readReg(op0->field[0]);
            res = buildop2(BVOper::bvsub, v0, v1);
        } else if (op0->type == Operand::Mem) { // dest op is mem
            v0 = se->readMem(this->get_memory_address(), op0->bit);
            res = buildop2(BVOper::bvsub, v0, v1);
        } else {
            ERROR("other instructions: op2 is not ImmValue, Reg, or Mem!");
        }

        if (se->eflags) {
            // Update CF
            updateCFsub(se, v0, v1);

            // Update SF
            updateSF(se, res, op0->bit);

            // Update ZF
            updateZF(se, res);

            // Update OF
            updateOFsub(se, v0, v1, op0->bit);

        }

        return true;
    }

    bool Dyn_X86_INS_JMP::symbolic_execution(tana::SEEngine *se) {
        return true;
    }

    //JA CF = 0 and ZF = 0
    bool Dyn_X86_INS_JA::symbolic_execution(tana::SEEngine *se) {
        if (!se->eflags)
            return false;
        std::shared_ptr<BitVector> CF = se->getFlags("CF");
        std::shared_ptr<BitVector> ZF = se->getFlags("ZF");

        if (this->vcpu.CF() == 0 && this->vcpu.ZF() == 0) {
            auto CF_O = buildop2(BVOper::equal, CF, 0);
            auto ZF_O = buildop2(BVOper::equal, ZF, 0);
            auto constrains = std::make_shared<Constrain>(CF_O, BVOper::bvand, ZF_O);
            se->updateConstrains(constrains);
        } else {
            auto CF_O = buildop2(BVOper::equal, CF, 1);
            auto ZF_O = buildop2(BVOper::equal, ZF, 1);
            auto constrains = std::make_shared<Constrain>(CF_O, BVOper::bvor, ZF_O);
            se->updateConstrains(constrains);
        }
        return true;
    }

    //JAE CF = 0
    bool Dyn_X86_INS_JAE::symbolic_execution(tana::SEEngine *se) {
        if (!se->eflags)
            return false;
        std::shared_ptr<BitVector> CF = se->getFlags("CF");
        std::shared_ptr<Constrain> constrains;
        if (vcpu.CF() == 0) {
            constrains = std::make_shared<Constrain>(CF, BVOper::equal, 0);
        } else {
            constrains = std::make_shared<Constrain>(CF, BVOper::equal, 1);
        }
        se->updateConstrains(constrains);
        return true;
    }

    //JB CF = 1
    bool Dyn_X86_INS_JB::symbolic_execution(tana::SEEngine *se) {
        if (!se->eflags)
            return false;
        std::shared_ptr<BitVector> CF = se->getFlags("CF");
        std::shared_ptr<Constrain> constrains;

        if (vcpu.CF()) {
            constrains = std::make_shared<Constrain>(CF, BVOper::equal, 1);
        } else {
            constrains = std::make_shared<Constrain>(CF, BVOper::equal, 0);
        }
        se->updateConstrains(constrains);
        return true;
    }

    //JBE CF = 1 or ZF = 1
    bool Dyn_X86_INS_JBE::symbolic_execution(tana::SEEngine *se) {
        if (!se->eflags)
            return false;
        std::shared_ptr<BitVector> CF = se->getFlags("CF");
        std::shared_ptr<BitVector> ZF = se->getFlags("ZF");

        if (vcpu.CF() == 1 || vcpu.ZF() == 1) {
            auto CF_1 = buildop2(BVOper::equal, CF, 1);
            auto ZF_1 = buildop2(BVOper::equal, ZF, 1);
            auto constrains = std::make_shared<Constrain>(CF_1, BVOper::bvor, ZF_1);
            se->updateConstrains(constrains);
        } else {
            auto CF_0 = buildop2(BVOper::equal, CF, 0);
            auto ZF_0 = buildop2(BVOper::equal, ZF, 0);
            auto constrains = std::make_shared<Constrain>(CF_0, BVOper::bvand, ZF_0);
            se->updateConstrains(constrains);
        }
        return true;
    }

    //JC CF = 1
    bool Dyn_X86_INS_JC::symbolic_execution(tana::SEEngine *se) {
        if (!se->eflags)
            return false;
        std::shared_ptr<BitVector> CF = se->getFlags("CF");
        std::shared_ptr<Constrain> constrains;
        if (vcpu.CF() == 1) {
            constrains = std::make_shared<Constrain>(CF, BVOper::equal, 1);
        } else {
            constrains = std::make_shared<Constrain>(CF, BVOper::equal, 0);
        }
        se->updateConstrains(constrains);
        return true;
    }

    //JE ZF = 1
    bool Dyn_X86_INS_JE::symbolic_execution(tana::SEEngine *se) {
        if (!se->eflags)
            return false;
        std::shared_ptr<BitVector> ZF = se->getFlags("ZF");
        std::shared_ptr<Constrain> constrains;
        if (vcpu.ZF() == 1) {
            constrains = std::make_shared<Constrain>(ZF, BVOper::equal, 1);
        } else {
            constrains = std::make_shared<Constrain>(ZF, BVOper::equal, 0);
        }
        se->updateConstrains(constrains);
        return true;
    }

    //JG ZF = 0 and SF = OF
    bool Dyn_X86_INS_JG::symbolic_execution(tana::SEEngine *se) {
        if (!se->eflags)
            return false;
        std::shared_ptr<BitVector> ZF = se->getFlags("ZF");
        std::shared_ptr<BitVector> SF = se->getFlags("SF");
        std::shared_ptr<BitVector> OF = se->getFlags("OF");
        std::shared_ptr<Constrain> constrains;

        if ((vcpu.ZF() == 0) && (vcpu.SF() == vcpu.OF())) {
            auto ZF_O = buildop2(BVOper::equal, ZF, 0);
            auto SF_OF = buildop2(BVOper::equal, SF, OF);
            constrains = std::make_shared<Constrain>(SF_OF, BVOper::bvand, ZF_O);
        } else {
            auto ZF_1 = buildop2(BVOper::equal, ZF, 1);
            auto SFnOF = buildop2(BVOper::noequal, SF, OF);
            constrains = std::make_shared<Constrain>(SFnOF, BVOper::bvor, ZF_1);
        }
        se->updateConstrains(constrains);
        return true;
    }

    //JGE SF = OF
    bool Dyn_X86_INS_JGE::symbolic_execution(tana::SEEngine *se) {
        if (!se->eflags)
            return false;

        std::shared_ptr<BitVector> SF = se->getFlags("SF");
        std::shared_ptr<BitVector> OF = se->getFlags("OF");
        std::shared_ptr<Constrain> constrains;

        if (vcpu.SF() == vcpu.OF()) {
            constrains = std::make_shared<Constrain>(SF, BVOper::equal, OF);
        } else {
            constrains = std::make_shared<Constrain>(SF, BVOper::noequal, OF);
        }
        se->updateConstrains(constrains);
        return true;
    }

    //JL SF != OF
    bool Dyn_X86_INS_JL::symbolic_execution(tana::SEEngine *se) {
        if (!se->eflags)
            return false;

        std::shared_ptr<BitVector> SF = se->getFlags("SF");
        std::shared_ptr<BitVector> OF = se->getFlags("OF");
        std::shared_ptr<Constrain> constrains;

        if (vcpu.SF() != vcpu.OF()) {
            constrains = std::make_shared<Constrain>(SF, BVOper::noequal, OF);
        } else {
            constrains = std::make_shared<Constrain>(SF, BVOper::equal, OF);
        }
        se->updateConstrains(constrains);
        return true;
    }

    //JLE ZF or SF != OF
    bool Dyn_X86_INS_JLE::symbolic_execution(tana::SEEngine *se) {
        if (!se->eflags)
            return false;
        std::shared_ptr<BitVector> ZF = se->getFlags("ZF");
        std::shared_ptr<BitVector> SF = se->getFlags("SF");
        std::shared_ptr<BitVector> OF = se->getFlags("OF");
        std::shared_ptr<Constrain> constrains;

        if (vcpu.ZF() || (vcpu.SF() != vcpu.OF())) {
            auto ZF_1 = buildop2(BVOper::equal, ZF, 1);
            auto SF_OF = buildop2(BVOper::noequal, SF, OF);
            constrains = std::make_shared<Constrain>(SF_OF, BVOper::bvor, ZF_1);
        } else {
            auto ZF_0 = buildop2(BVOper::equal, ZF, 0);
            auto SF_OF = buildop2(BVOper::equal, SF, OF);
            constrains = std::make_shared<Constrain>(SF_OF, BVOper::bvand, ZF_0);
        }
        se->updateConstrains(constrains);
        return true;
    }

    //JNA CF or ZF
    bool Dyn_X86_INS_JNA::symbolic_execution(tana::SEEngine *se) {
        if (!se->eflags)
            return false;
        std::shared_ptr<BitVector> ZF = se->getFlags("ZF");
        std::shared_ptr<BitVector> CF = se->getFlags("CF");
        std::shared_ptr<Constrain> constrains;

        if (vcpu.CF() || vcpu.ZF()) {
            auto CF_1 = buildop2(BVOper::equal, CF, 1);
            auto ZF_1 = buildop2(BVOper::equal, ZF, 1);
            constrains = std::make_shared<Constrain>(CF_1, BVOper::bvor, ZF_1);
        } else {
            auto CF_0 = buildop2(BVOper::equal, CF, 0);
            auto ZF_0 = buildop2(BVOper::equal, ZF, 0);
            constrains = std::make_shared<Constrain>(CF_0, BVOper::bvand, ZF_0);
        }
        se->updateConstrains(constrains);
        return true;

    }

    //JNAE CF
    bool Dyn_X86_INS_JNAE::symbolic_execution(tana::SEEngine *se) {
        if (!se->eflags)
            return false;
        std::shared_ptr<BitVector> CF = se->getFlags("CF");
        std::shared_ptr<Constrain> constrains;

        if (vcpu.CF()) {
            constrains = std::make_shared<Constrain>(CF, BVOper::equal, 1);
        } else {
            constrains = std::make_shared<Constrain>(CF, BVOper::equal, 0);
        }
        se->updateConstrains(constrains);
        return true;
    }

    //JNB CF = 0
    bool Dyn_X86_INS_JNB::symbolic_execution(tana::SEEngine *se) {
        if (!se->eflags)
            return false;
        std::shared_ptr<BitVector> CF = se->getFlags("CF");
        std::shared_ptr<Constrain> constrains;

        if (vcpu.CF() == 0) {
            constrains = std::make_shared<Constrain>(CF, BVOper::equal, 0);
        } else {
            constrains = std::make_shared<Constrain>(CF, BVOper::equal, 1);
        }
        se->updateConstrains(constrains);
        return true;
    }

    //JNBE CF = 0 and ZF = 0
    bool Dyn_X86_INS_JNBE::symbolic_execution(tana::SEEngine *se) {
        if (!se->eflags)
            return false;
        std::shared_ptr<BitVector> CF = se->getFlags("CF");
        std::shared_ptr<BitVector> ZF = se->getFlags("ZF");
        std::shared_ptr<Constrain> constrains;

        if ((vcpu.CF() == 0) && (vcpu.ZF() == 0)) {
            auto CF_0 = buildop2(BVOper::equal, CF, 0);
            auto ZF_0 = buildop2(BVOper::equal, ZF, 0);
            constrains = std::make_shared<Constrain>(CF_0, BVOper::bvand, ZF_0);
        } else {
            auto CF_1 = buildop2(BVOper::equal, CF, 1);
            auto ZF_1 = buildop2(BVOper::equal, ZF, 1);
            constrains = std::make_shared<Constrain>(CF_1, BVOper::bvor, ZF_1);
        }
        se->updateConstrains(constrains);
        return true;
    }

    //JNC CF = 0
    bool Dyn_X86_INS_JNC::symbolic_execution(tana::SEEngine *se) {
        if (!se->eflags)
            return false;
        std::shared_ptr<BitVector> CF = se->getFlags("CF");
        std::shared_ptr<Constrain> constrains;

        if (vcpu.CF() == 0) {
            constrains = std::make_shared<Constrain>(CF, BVOper::equal, 0);
        } else {
            constrains = std::make_shared<Constrain>(CF, BVOper::equal, 1);
        }
        se->updateConstrains(constrains);
        return true;
    }

    //JNE ZF = 0
    bool Dyn_X86_INS_JNE::symbolic_execution(tana::SEEngine *se) {
        if (!se->eflags)
            return false;
        std::shared_ptr<BitVector> ZF = se->getFlags("ZF");
        std::shared_ptr<Constrain> constrains;
        if (vcpu.ZF() == 0) {
            constrains = std::make_shared<Constrain>(ZF, BVOper::equal, 0);
        } else {
            constrains = std::make_shared<Constrain>(ZF, BVOper::equal, 1);
        }
        se->updateConstrains(constrains);
        return true;
    }

    //JNG ZF or SF != OF
    bool Dyn_X86_INS_JNG::symbolic_execution(tana::SEEngine *se) {
        if (!se->eflags)
            return false;
        std::shared_ptr<BitVector> ZF = se->getFlags("ZF");
        std::shared_ptr<BitVector> SF = se->getFlags("SF");
        std::shared_ptr<BitVector> OF = se->getFlags("OF");
        std::shared_ptr<Constrain> constrains;

        if (vcpu.ZF() || (vcpu.ZF() != vcpu.OF())) {
            std::shared_ptr<BitVector> ZF_1 = buildop2(BVOper::equal, ZF, 1);
            std::shared_ptr<BitVector> SF_OF = buildop2(BVOper::noequal, SF, OF);
            constrains = std::make_shared<Constrain>(ZF_1, BVOper::bvor, SF_OF);
        } else {
            std::shared_ptr<BitVector> ZF_0 = buildop2(BVOper::equal, ZF, 0);
            std::shared_ptr<BitVector> SF_OF = buildop2(BVOper::equal, SF, OF);
            constrains = std::make_shared<Constrain>(ZF_0, BVOper::bvand, SF_OF);
        }
        se->updateConstrains(constrains);
        return true;
    }

    //JNGE SF != OF
    bool Dyn_X86_INS_JNGE::symbolic_execution(tana::SEEngine *se) {
        if (!se->eflags)
            return false;
        std::shared_ptr<BitVector> SF = se->getFlags("SF");
        std::shared_ptr<BitVector> OF = se->getFlags("OF");
        std::shared_ptr<Constrain> constrains;

        if (vcpu.SF() != vcpu.OF()) {
            constrains = std::make_shared<Constrain>(SF, BVOper::noequal, OF);
        } else {
            constrains = std::make_shared<Constrain>(SF, BVOper::equal, OF);

        }
        se->updateConstrains(constrains);
        return true;
    }

    //JNL SF = OF
    bool Dyn_X86_INS_JNL::symbolic_execution(tana::SEEngine *se) {
        if (!se->eflags)
            return false;
        std::shared_ptr<BitVector> SF = se->getFlags("SF");
        std::shared_ptr<BitVector> OF = se->getFlags("OF");
        std::shared_ptr<Constrain> constrains;

        if (vcpu.SF() == vcpu.OF()) {
            constrains = std::make_shared<Constrain>(SF, BVOper::equal, OF);
        } else {
            constrains = std::make_shared<Constrain>(SF, BVOper::noequal, OF);
        }
        se->updateConstrains(constrains);
        return true;
    }

    //JNLE ZF = 0 and SF = OF
    bool Dyn_X86_INS_JNLE::symbolic_execution(tana::SEEngine *se) {
        if (!se->eflags)
            return false;
        std::shared_ptr<BitVector> ZF = se->getFlags("ZF");
        std::shared_ptr<BitVector> SF = se->getFlags("SF");
        std::shared_ptr<BitVector> OF = se->getFlags("OF");
        std::shared_ptr<Constrain> constrains;

        if ((vcpu.ZF() == 0) && (vcpu.SF() == vcpu.OF())) {
            std::shared_ptr<BitVector> ZF_0 = buildop2(BVOper::equal, ZF, 0);
            std::shared_ptr<BitVector> SF_OF = buildop2(BVOper::equal, SF, OF);
            constrains = std::make_shared<Constrain>(ZF_0, BVOper::bvand, SF_OF);
        } else {
            std::shared_ptr<BitVector> ZF_1 = buildop2(BVOper::equal, ZF, 1);
            std::shared_ptr<BitVector> SFnOF = buildop2(BVOper::noequal, SF, OF);
            constrains = std::make_shared<Constrain>(ZF_1, BVOper::bvor, SFnOF);
        }
        se->updateConstrains(constrains);
        return true;
    }

    //JNO OF = 0
    bool Dyn_X86_INS_JNO::symbolic_execution(tana::SEEngine *se) {
        if (!se->eflags)
            return false;
        std::shared_ptr<BitVector> OF = se->getFlags("OF");
        std::shared_ptr<Constrain> constrains;

        constrains = std::make_shared<Constrain>(OF, BVOper::equal, vcpu.OF());
        se->updateConstrains(constrains);
        return true;
    }

    //JNS SF = 0
    bool Dyn_X86_INS_JNS::symbolic_execution(tana::SEEngine *se) {
        if (!se->eflags)
            return false;
        std::shared_ptr<BitVector> SF = se->getFlags("SF");
        std::shared_ptr<Constrain> constrains;

        constrains = std::make_shared<Constrain>(SF, BVOper::equal, vcpu.SF());
        se->updateConstrains(constrains);
        return true;
    }


    //JNZ ZF = 0
    bool Dyn_X86_INS_JNZ::symbolic_execution(tana::SEEngine *se) {
        if (!se->eflags)
            return false;
        std::shared_ptr<BitVector> ZF = se->getFlags("ZF");
        std::shared_ptr<Constrain> constrains;

        constrains = std::make_shared<Constrain>(ZF, BVOper::equal, vcpu.ZF());
        se->updateConstrains(constrains);
        return true;
    }

    //JO OF
    bool Dyn_X86_INS_JO::symbolic_execution(tana::SEEngine *se) {
        if (!se->eflags)
            return false;
        std::shared_ptr<BitVector> OF = se->getFlags("OF");
        std::shared_ptr<Constrain> constrains;

        constrains = std::make_shared<Constrain>(OF, BVOper::equal, vcpu.OF());
        se->updateConstrains(constrains);
        return true;
    }

    //JS SF
    bool Dyn_X86_INS_JS::symbolic_execution(tana::SEEngine *se) {
        if (!se->eflags)
            return false;
        std::shared_ptr<BitVector> SF = se->getFlags("SF");
        std::shared_ptr<Constrain> constrains;

        constrains = std::make_shared<Constrain>(SF, BVOper::equal, vcpu.SF());
        se->updateConstrains(constrains);
        return true;
    }

    //JZ ZF
    bool Dyn_X86_INS_JZ::symbolic_execution(tana::SEEngine *se) {
        if (!se->eflags)
            return false;
        std::shared_ptr<BitVector> SF = se->getFlags("ZF");
        std::shared_ptr<Constrain> constrains;

        constrains = std::make_shared<Constrain>(SF, BVOper::equal, vcpu.ZF());
        se->updateConstrains(constrains);
        return true;
    }

    bool Dyn_X86_INS_REP_STOSD::symbolic_execution(tana::SEEngine *se)
    {

        // ecx = ecx - 1
        auto v_ecx = se->readReg("ecx");
        v_ecx = buildop2(BVOper::bvsub, v_ecx, 1);
        se->writeReg("ecx", v_ecx);

        assert(oprd[0]->type == Operand::Mem);
        // Update register
        auto v_reg = se->readReg(oprd[0]->field[0]);
        v_reg = buildop2(BVOper::bvadd, v_reg, 4);
        se->writeReg(oprd[0]->field[0], v_reg);

        // Store the contents of eax into the memory
        auto v_eax = se->readReg("eax");
        se->writeMem(this->get_memory_address(), oprd[0]->bit, v_eax);

        return true;
    }

    bool Dyn_X86_INS_CMOVZ::symbolic_execution(tana::SEEngine *se)
    {
        return true;
    }

    bool Dyn_X86_INS_SETZ::symbolic_execution(tana::SEEngine *se)
    {
        return true;
    }

    bool Dyn_X86_INS_SETNZ::symbolic_execution(tana::SEEngine *se)
    {
        return true;
    }


}