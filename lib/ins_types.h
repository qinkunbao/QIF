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
        //Operand() : bit(0), issegaddr(false) {}
    };

    class Inst_Base {
    public:
        t_type::index id;  //instruction ID
        t_type::T_ADDRESS addrn; //Instruction address
        x86::x86_insn instruction_id;
        std::vector<std::string> oprs;
        std::shared_ptr<Operand> oprd[3];
        Inst_Base();
        std::string get_opcode_operand() const ;

        virtual void print() const;
        uint32_t get_operand_number() const;
    };

    class Inst_Static : public  Inst_Base {
    public:
        Inst_Static();
        void parseOperand();

    };

    class Inst_Dyn : public Inst_Base {
    public:
        vcpu_ctx vcpu;
        t_type::T_ADDRESS memory_address;
        Inst_Dyn();
        void parseOperand();
		virtual bool symbolic_execution(SEEngine &se)
		{

		    std::cout << "Index: " << id <<" Unsupported Instruction: " << this->get_opcode_operand() << std::endl;
		    return true;
		};
		virtual bool taint(){ return true; };

	};

    class Routine {
    public:
        t_type::T_ADDRESS start_addr; // start address of the function
        t_type::T_ADDRESS end_addr;   // end address of the function
        std::string rtn_name;
        std::string module_name;
        Routine(): start_addr(0), end_addr(0), rtn_name(), module_name() {}
    };


    namespace inst_dyn_details {
        bool two_operand(SEEngine &se, Inst_Dyn *inst);
    }


	class Dyn_X86_INS_PUSH : public Inst_Dyn {
	public:
		bool symbolic_execution(SEEngine &se) final;
	};

	class Dyn_X86_INS_POP : public Inst_Dyn {
	public:
		bool symbolic_execution(SEEngine &se) final;
	};

	class Dyn_X86_INS_NEG : public Inst_Dyn {
	public:
		bool symbolic_execution(SEEngine &se) final;
	};

	class Dyn_X86_INS_NOT : public Inst_Dyn {
	public:
		bool symbolic_execution(SEEngine &se) final;

	};

	class Dyn_X86_INS_INC : public Inst_Dyn {
	public:
		bool symbolic_execution(SEEngine &se) final;
	};

	class Dyn_X86_INS_DEC : public Inst_Dyn {
	public:
		bool symbolic_execution(SEEngine &se) final;
	};

	class Dyn_X86_INS_MOVSX : public Inst_Dyn {
	public:
		bool symbolic_execution(SEEngine &se) final;
	};

	class Dyn_X86_INS_MOVZX : public Inst_Dyn {
	public:
		bool symbolic_execution(SEEngine &se) final;
	};

	class Dyn_X86_INS_CMOVB : public Inst_Dyn {
	public:
		bool symbolic_execution(SEEngine &se) final;
	};

	class Dyn_X86_INS_MOV : public Inst_Dyn {
	public:
		bool symbolic_execution(SEEngine &se) override;
	};

	class Dyn_X86_INS_LEA : public Inst_Dyn {
	public:
		bool symbolic_execution(SEEngine &se) final;
	};

	class Dyn_X86_INS_XCHG : public Inst_Dyn {
	public:
		bool symbolic_execution(SEEngine &se) final;
	};

	class Dyn_X86_INS_SBB : public Inst_Dyn {
	public:
		bool symbolic_execution(SEEngine &se) final;
	};

	class Dyn_X86_INS_IMUL : public Inst_Dyn {
	public:
		bool symbolic_execution(SEEngine &se) final;
	};

	class Dyn_X86_INS_SHLD : public Inst_Dyn {
	public:
		bool symbolic_execution(SEEngine &se) final;
	};

	class Dyn_X86_INS_SHRD : public Inst_Dyn {
	public:
		bool symbolic_execution(SEEngine &se) final;
	};

	//two operands

    class Dyn_X86_INS_ADD : public Inst_Dyn {
    public:
        bool symbolic_execution(SEEngine &se) final
        {
            return inst_dyn_details::two_operand(se, this);
        }

    };

    class Dyn_X86_INS_SUB : public Inst_Dyn {
    public:
        bool symbolic_execution(SEEngine &se) final
        {
            return inst_dyn_details::two_operand(se, this);
        }

    };


    class Dyn_X86_INS_AND : public Inst_Dyn {
    public:
        bool symbolic_execution(SEEngine &se) final
        {
            return inst_dyn_details::two_operand(se, this);
        }
    };

	class Dyn_X86_INS_ADC : public Inst_Dyn {
	public:
        bool symbolic_execution(SEEngine &se) final
        {
            return inst_dyn_details::two_operand(se, this);
        }
	};

    class Dyn_X86_INS_ROR : public Inst_Dyn {
    public:
        bool symbolic_execution(SEEngine &se) final
        {
            return inst_dyn_details::two_operand(se, this);
        }
    };

    class Dyn_X86_INS_ROL : public Inst_Dyn {
    public:
        bool symbolic_execution(SEEngine &se) final
        {
            return inst_dyn_details::two_operand(se, this);
        }
    };

    class Dyn_X86_INS_OR : public Inst_Dyn {
    public:
        bool symbolic_execution(SEEngine &se) final
        {
            return inst_dyn_details::two_operand(se, this);
        }
    };

    class Dyn_X86_INS_XOR : public Inst_Dyn {
    public:
        bool symbolic_execution(SEEngine &se) final
        {
            return inst_dyn_details::two_operand(se, this);
        }
    };

    class Dyn_X86_INS_SHL : public Inst_Dyn {
    public:
        bool symbolic_execution(SEEngine &se) final
        {
            return inst_dyn_details::two_operand(se, this);
        }
    };

    class Dyn_X86_INS_SHR : public Inst_Dyn {
    public:
        bool symbolic_execution(SEEngine &se) final
        {
            return inst_dyn_details::two_operand(se, this);
        }
    };

    class Dyn_X86_INS_SAR : public Inst_Dyn {
    public:
        bool symbolic_execution(SEEngine &se) final
        {
            return inst_dyn_details::two_operand(se, this);
        }
    };

    class Inst_Dyn_Factory
    {
    public:
        static std::unique_ptr<Inst_Dyn> makeInst (x86::x86_insn id);
    };


}
