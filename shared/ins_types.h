#pragma once
#include <vector>
#include <string>
#include <set>
#include <memory>

#include "x86.h"

namespace tana {

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

    namespace t_type {

        typedef uint32_t T_ADDRESS;
        typedef uint32_t T_SIZE;
        typedef uint32_t R_SIZE;
        typedef uint32_t RegPart;
        typedef uint32_t index;
    }

	namespace reg_mask {
		const static uint32_t EIGHT_L = 0x000000ff; //AL, BL, CL, DL
		const static uint32_t EIGHT_H = 0x0000ff00; //AH, BH, CH, DH
		const static uint32_t HALF = 0x0000ffff; //AX, BX, CX, DX
		const static uint32_t FULL = 0xffffffff;
	}

    typedef struct {
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
        uint32_t gpr[GPR_NUM];
    } vcpu_ctx;

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
        //Operand() : bit(0), issegaddr(false) {}
    };

    class Inst {
    public:
        t_type::index id;  //instruction ID
        t_type::T_ADDRESS addrn; //Instruction address
        x86::x86_insn instruction_id;
        std::vector<std::string> oprs;
        std::shared_ptr<Operand> oprd[3];
        vcpu_ctx vcpu;
        t_type::T_ADDRESS memory_address;
        std::string get_opcode_operand() const ;
        uint32_t get_operand_number() const;
        Inst();
    };

    class Routine {
    public:
        t_type::T_ADDRESS start_addr = 0; // start address of the function
        t_type::T_ADDRESS end_addr = 0;   // end address of the function
        std::string rtn_name;
        std::string module_name;
    };


    namespace instruction {

		enum OprID {
			t_mov = 0,
			t_add,
			t_xor,
			t_push,
			t_pop,
			t_sub,
			t_sbb,
			t_imul,
			t_or,
			t_shl,
			t_shr,
			t_neg,
			t_not,
			t_inc,
			t_lea,
			t_and,
			t_div,
			t_sar,
			t_rol,
			t_ror,
			t_dec,
			t_movsx,
		    t_movzx,
			t_cmovb,
			t_cmovz,
			t_xchg,
			t_shrd,
			t_shld,
            t_none
        };

		OprID getOprID(std::string opcstr);
		bool seNoEffect(std::string opcstr);
        bool SymbolicExecutionNoEffect(x86::x86_insn insn);
		bool isSysCall(std::string opcstr);
		bool isExchangeable(std::string opcstr);
    }
}
