/*************************************************************************
	> File Name: ins_semantics.h
	> Author: Qinkun Bao
	> Mail: qinkunbao@gmail.com
	> Created Time: Mon Apr 22 21:56:51 2019
 ************************************************************************/

#pragma once

#include "BitVector.h"
#include "ins_types.h"


namespace tana {

    class Inst_Dyn_Factory {
    public:
        static std::unique_ptr<Inst_Base> makeInst(x86::x86_insn id, bool isStatic);
    };

    namespace inst_dyn_details {
        std::shared_ptr<BitVector> two_operand(SEEngine *se, Inst_Base *inst, BVOper);
    }

    class INST_X86_INS_NOP : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };


    class INST_X86_INS_PUSH : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_POP : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_NEG : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_NOT : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;

    };

    class INST_X86_INS_INC : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_DEC : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_MOVSX : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_MOVZX : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_CMOVB : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_MOV : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) override;
    };

    class INST_X86_INS_LEA : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_XCHG : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_SBB : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_IMUL : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_SHLD : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_SHRD : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_ADC : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    //two operands

    class INST_X86_INS_ADD : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_SUB : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };


    class INST_X86_INS_AND : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };


    class INST_X86_INS_ROR : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_ROL : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_OR : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_XOR : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_SHL : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_SHR : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_SAR : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_RET : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_CALL : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_LEAVE : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };


    class INST_X86_INS_ENTER : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_DIV : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_TEST : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_CMP : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_JMP : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;

    };

    class INST_X86_INS_JA : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;

    };

    class INST_X86_INS_JAE : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;

    };

    class INST_X86_INS_JB : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_JBE : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_JC : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_JE : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_JG : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };


    class INST_X86_INS_JGE : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_JL : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_JLE : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_JNA : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_JNAE : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_JNB : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };


    class INST_X86_INS_JNBE : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_JNC : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_JNE : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_JNG : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_JNGE : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_JNL : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_JNLE : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_JNO : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_JNS : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_JNZ : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_JO : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_JS : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_JZ : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_REP_STOSD : public Inst_Base {
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_STOSB : public Inst_Base {
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_CMOVZ : public Inst_Base {
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_SETZ : public Inst_Base {
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_SETNZ : public Inst_Base {
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_BT : public Inst_Base {
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_MOVSB : public Inst_Base {
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_MUL : public Inst_Base {
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_MOVSD : public Inst_Base {
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_CLD : public Inst_Base {
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;
    };

    class INST_X86_INS_CMOVNS : public Inst_Base {
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;

    };

    class INST_X86_INS_CPUID : public Inst_Base {
        using Inst_Base::Inst_Base;

        bool symbolic_execution(SEEngine *se) final;

    };


}