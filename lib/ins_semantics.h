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

    class Dyn_X86_INS_NOP : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;
        bool symbolic_execution(SEEngine *se) final;
    };



    class Dyn_X86_INS_PUSH : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;
        bool symbolic_execution(SEEngine *se) final;
    };

    class Dyn_X86_INS_POP : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;
        bool symbolic_execution(SEEngine *se) final;
    };

    class Dyn_X86_INS_NEG : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;
        bool symbolic_execution(SEEngine *se) final;
    };

    class Dyn_X86_INS_NOT : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;
        bool symbolic_execution(SEEngine *se) final;

    };

    class Dyn_X86_INS_INC : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;
        bool symbolic_execution(SEEngine *se) final;
    };

    class Dyn_X86_INS_DEC : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;
        bool symbolic_execution(SEEngine *se) final;
    };

    class Dyn_X86_INS_MOVSX : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;
        bool symbolic_execution(SEEngine *se) final;
    };

    class Dyn_X86_INS_MOVZX : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;
        bool symbolic_execution(SEEngine *se) final;
    };

    class Dyn_X86_INS_CMOVB : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;
        bool symbolic_execution(SEEngine *se) final;
    };

    class Dyn_X86_INS_MOV : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;
        bool symbolic_execution(SEEngine *se) override;
    };

    class Dyn_X86_INS_LEA : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;
        bool symbolic_execution(SEEngine *se) final;
    };

    class Dyn_X86_INS_XCHG : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;
        bool symbolic_execution(SEEngine *se) final;
    };

    class Dyn_X86_INS_SBB : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;
        bool symbolic_execution(SEEngine *se) final;
    };

    class Dyn_X86_INS_IMUL : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;
        bool symbolic_execution(SEEngine *se) final;
    };

    class Dyn_X86_INS_SHLD : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;
        bool symbolic_execution(SEEngine *se) final;
    };

    class Dyn_X86_INS_SHRD : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;
        bool symbolic_execution(SEEngine *se) final;
    };

    class Dyn_X86_INS_ADC : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;
        bool symbolic_execution(SEEngine *se) final;
    };

    //two operands

    class Dyn_X86_INS_ADD : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;
        bool symbolic_execution(SEEngine *se) final;
    };

    class Dyn_X86_INS_SUB : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;
        bool symbolic_execution(SEEngine *se) final;
    };


    class Dyn_X86_INS_AND : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;
        bool symbolic_execution(SEEngine *se) final;
    };


    class Dyn_X86_INS_ROR : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;
        bool symbolic_execution(SEEngine *se) final;
    };

    class Dyn_X86_INS_ROL : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;
        bool symbolic_execution(SEEngine *se) final;
    };

    class Dyn_X86_INS_OR : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;
        bool symbolic_execution(SEEngine *se) final;
    };

    class Dyn_X86_INS_XOR : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;
        bool symbolic_execution(SEEngine *se) final;
    };

    class Dyn_X86_INS_SHL : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;
        bool symbolic_execution(SEEngine *se) final;
    };

    class Dyn_X86_INS_SHR : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;
        bool symbolic_execution(SEEngine *se) final;
    };

    class Dyn_X86_INS_SAR : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;
        bool symbolic_execution(SEEngine *se) final;
    };

    class Dyn_X86_INS_RET : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;
        bool symbolic_execution(SEEngine *se) final;
    };

    class Dyn_X86_INS_CALL : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;
        bool symbolic_execution(SEEngine *se) final;
    };

    class Dyn_X86_INS_LEAVE : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;
        bool symbolic_execution(SEEngine *se) final;
    };


    class Dyn_X86_INS_ENTER : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;
        bool symbolic_execution(SEEngine *se) final;
    };

    class Dyn_X86_INS_DIV : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;
        bool symbolic_execution(SEEngine *se) final;
    };

    class Dyn_X86_INS_TEST : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;
        bool symbolic_execution(SEEngine *se) final;
    };

    class Dyn_X86_INS_CMP : public Inst_Base {
    public:
        using Inst_Base::Inst_Base;
        bool symbolic_execution(SEEngine *se) final;
    };

    class Dyn_X86_INS_JMP : public  Inst_Base{
    public:
        using Inst_Base::Inst_Base;
        bool symbolic_execution(SEEngine *se) final;

    };

    class Dyn_X86_INS_JE : public Inst_Base{
    public:
        using Inst_Base::Inst_Base;
        bool symbolic_execution(SEEngine *se) final;
    };

    class Dyn_X86_INS_JB : public Inst_Base{
    public:
        using Inst_Base::Inst_Base;
        bool symbolic_execution(SEEngine *se) final;
    };

    class Dyn_X86_INS_JG : public Inst_Base{
    public:
        using Inst_Base::Inst_Base;
        bool symbolic_execution(SEEngine *se) final;
    };
}