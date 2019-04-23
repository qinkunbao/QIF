#pragma once
#include <vector>
#include <string>
#include <set>
#include <array>
#include <memory>
#include <cassert>
#include <iostream>

#include "x86.h"

namespace tana {

    class SEEngine;
    const static uint32_t T_BYTE = 1;
    const static uint32_t T_WORD = 2;
    const static uint32_t T_DWORD = 4;
    const static uint32_t T_QWORD = 8;
    const static uint32_t T_OWORD = 16;


    const static uint32_t GPR_NUM = 8;
    const static uint32_t REGISTER_SIZE = 32;
    const static uint32_t T_BYTE_SIZE = 8;

    const static uint32_t BATCH_SIZE = 2000;
	const static uint32_t FORMULA_MAX_LENGTH = 0xffff;
	const static uint32_t FORMULA_MIN_LENGTH = 6;

	const static uint32_t MIN_NUM_INPUT = 2;

	const static uint32_t MAX_LOOP_HISTORY = 2000;
	const static uint32_t MIN_LOOP_LENGTH = 20;

	const static uint32_t LOOP_PRINT_FREQUENCY = 1000;

	const static uint32_t MAX_IMM_NUMBER = 0xff;

	const static uint32_t MAX_INT = 0x7fffffff;
	const static uint32_t MAX_UNSIGNED_INT = 0xffffffff;

    namespace tana_type {

        typedef uint32_t T_ADDRESS;
        typedef uint32_t T_SIZE;
        typedef uint32_t R_SIZE;
        typedef uint32_t RegPart;
        typedef uint32_t index;
    }



    class vcpu_ctx{
        /*
         * General Purpose Registers
         * 0 EAX
         * 1 EBX
         * 2 ECX
         * 3 EDX
         * 4 ESI
         * 5 EDX
         * 6 ESP
         * 7 EBP
         */
        uint32_t eflags;
    public:
        std::array<uint32_t, GPR_NUM> gpr;

        void set_eflags(uint32_t data)
        {
            eflags = data;
        }
        vcpu_ctx();

        bool CF();
        bool PF();
        bool AF();
        bool ZF();
        bool SF();
        bool TF();
        bool DF();
        bool OF();

        bool eflags_state;
    };

    class Operand {
    public:
        enum OprType {
            ImmValue, Reg, Mem, UINIT
        };
        OprType type = UINIT;
        uint32_t tag = 0;
        uint32_t bit = 0;
        std::string field[5];
        bool issegaddr = false;
        std::string segreg;
    };

    class Inst_Base {
    public:
        bool is_static = false;
        tana_type::index id;  //instruction ID
        tana_type::T_ADDRESS addrn; //Instruction address
        x86::x86_insn instruction_id;
        std::vector<std::string> oprs;
        std::shared_ptr<Operand> oprd[3];
        vcpu_ctx vcpu;
        tana_type::T_ADDRESS memory_address;

        std::string get_opcode_operand() const ;
        virtual void print() const;
        uint32_t get_operand_number() const;


        explicit Inst_Base(bool);
        std::string get_memory_address();

        void parseOperand();

        virtual bool symbolic_execution(SEEngine *se)
        {

            std::cout << "Index: " << id <<" Unsupported Instruction: " << this->get_opcode_operand() << std::endl;
            print();
            return true;
        };

        virtual bool taint()
        {
            std::cout << "Index: " << id <<" Unsupported Instruction: " << this->get_opcode_operand() << std::endl;
            print();
            return true;
        };

        virtual ~Inst_Base() = default;
    };

    class Routine {
    public:
        tana_type::T_ADDRESS start_addr; // start address of the function
        tana_type::T_ADDRESS end_addr;   // end address of the function
        std::string rtn_name;
        std::string module_name;
        Routine(): start_addr(0), end_addr(0), rtn_name(), module_name() {}
    };


}
