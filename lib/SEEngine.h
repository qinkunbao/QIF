#pragma once

#include <list>
#include <map>
#include <memory>
#include "ins_types.h"
#include "Register.h"


namespace tana {
    class Value;
    class Operation;
    class SEEngine;

    enum ValueTy {
        SYMBOL, CONCRETE
    };
    enum ASTOperator {
        bvadd = 0,
        bvsub,
        bvmul,
        bvudiv,
        bvor,
        bvand,
        bvxor,
        bvurem,
        bvshl,
        bvlshr,
        bvnot,
        bvneg,
        bvconcat,
        bvextract,
        bvsighext,
        bvzeroext,

    };



    // A symbolic or concrete value in a formula
    class Value {
    public:
        ValueTy valty;             // value type: SYMBOL or CONCRETE
        std::unique_ptr<Operation> opr = nullptr;
        std::string conval;             // concrete value
        int id;                 // a unique id for each value
        int low_bit = 0;
        int high_bit = REGISTER_SIZE;
        static int idseed;

        Value(ValueTy vty);

        Value(ValueTy vty, std::string con); // constructor for concrete value
        Value(ValueTy vty, uint32_t con, SEEngine *);    // constructor for concrete value
        Value(ValueTy vty, std::unique_ptr<Operation> oper);

        Value(ValueTy vty, uint32_t con);

        bool isSymbol();

        bool operator==(const Value &v1);

        Value &operator=(const Value &v1);
    };

// An operation taking several values to calculate a result value
    class Operation {
    public:
        std::string opty;
        std::shared_ptr<Value> val[3] = {nullptr, nullptr, nullptr};

        Operation(std::string opt, std::shared_ptr<Value> v1);

        Operation(std::string opt, std::shared_ptr<Value> v1, std::shared_ptr<Value> v2);

        Operation(std::string opt, std::shared_ptr<Value> v1, std::shared_ptr<Value> v2, std::shared_ptr<Value> v3);
    };


    class SEEngine {
    private:
        bool imm2sym = false;
        std::map<std::string, std::shared_ptr<Value>> ctx;
        std::vector<Inst_Dyn>::iterator start;
        std::vector<Inst_Dyn>::iterator end;
        std::map<t_type::T_ADDRESS, std::shared_ptr<Value> > memory;

        static inline uint32_t arithmeticRightShift(uint32_t op1, uint32_t op2);

        static inline uint32_t rol32(uint32_t op1, uint32_t op2);

        static inline uint32_t ror32(uint32_t op1, uint32_t op2);

        static inline uint32_t shld32(uint32_t op0, uint32_t op1, uint32_t op2);

        static inline uint32_t shrd32(uint32_t op0, uint32_t op1, uint32_t op2);

        void DO_X86_INS_PUSH(const Inst_Dyn &it);

        void DO_X86_INS_POP(const Inst_Dyn &it);

        void DO_X86_INS_NEG(const Inst_Dyn &it);

        void DO_X86_INS_NOT(const Inst_Dyn &it);

        void DO_X86_INS_INC(const Inst_Dyn &it);

        void DO_X86_INS_DEC(const Inst_Dyn &it);

        void DO_X86_INS_MOVSX(const Inst_Dyn &it);

        void DO_X86_INS_MOVZX(const Inst_Dyn &it);

        void DO_X86_INS_CMOVB(const Inst_Dyn &it);

        void DO_X86_INS_MOV(const Inst_Dyn &it);

        void DO_X86_INS_LEA(const Inst_Dyn &it);

        void DO_X86_INS_XCHG(const Inst_Dyn &it);

        void DO_X86_INS_SBB(const Inst_Dyn &it);

        void DO_X86_INS_IMUL(const Inst_Dyn &it);

        void DO_X86_INS_SHLD(const Inst_Dyn &it);

        void DO_X86_INS_SHRD(const Inst_Dyn &it);

        void DO_X86_INS_ADC(const Inst_Dyn &it);

        static inline bool isRegSame(Inst_Dyn &instruction1, Inst_Dyn &instruction2);

        bool memory_find(uint32_t addr) {
            auto ii = memory.find(addr);
            if (ii == memory.end())
                return false;
            else
                return true;
        }

        std::shared_ptr<Value> getRegister(x86::x86_reg reg);

        std::shared_ptr<Value> getMemory(t_type::T_ADDRESS, t_type::T_SIZE);

        std::shared_ptr<Value> SignExt(std::shared_ptr<Value> v, t_type::T_SIZE, bool sign);

        std::shared_ptr<Value> ZeroExt(std::shared_ptr<Value> v, t_type::T_SIZE);

        std::shared_ptr<Value> Extract(std::shared_ptr<Value> v, int low, int high);

        std::shared_ptr<Value> Concat(std::shared_ptr<Value> v1, std::shared_ptr<Value> v2);

    public:
        SEEngine(bool type);

        static void getFormulaLength(std::shared_ptr<Value> v, uint32_t &len);

        void init(std::shared_ptr<Value> v1, std::shared_ptr<Value> v2,
                  std::shared_ptr<Value> v3, std::shared_ptr<Value> v4,
                  std::shared_ptr<Value> v5, std::shared_ptr<Value> v6,
                  std::shared_ptr<Value> v7, std::shared_ptr<Value> v8,
                  std::vector<Inst_Dyn>::iterator it1,
                  std::vector<Inst_Dyn>::iterator it2);

        void init(std::vector<Inst_Dyn>::iterator it1,
                  std::vector<Inst_Dyn>::iterator it2);

        void initAllRegSymol(std::vector<Inst_Dyn>::iterator it1,
                             std::vector<Inst_Dyn>::iterator it2);

        int symexec();

        bool isImmSym(uint32_t num);

        uint32_t conexec(std::shared_ptr<Value> f, std::map<std::shared_ptr<Value>, uint32_t> *input);

        std::string getRegName(std::string s);

        std::vector<std::shared_ptr<Value> > getAllOutput();

        std::vector<std::shared_ptr<Value> > reduceValues(std::vector<std::shared_ptr<Value>> values);

        uint32_t eval(std::shared_ptr<Value> v, std::map<std::shared_ptr<Value>, uint32_t> *inmap);

        uint32_t eval(std::shared_ptr<Value> v);

    };


}