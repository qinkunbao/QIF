#pragma once

#include <list>
#include <map>
#include <memory>
#include <string>
#include <set>
#include "ins_types.h"

namespace tana {

    class BitVector;
    class Operation;

    enum class ValueType {
        SYMBOL, CONCRETE
    };

    enum class BVOper
    {
        bvadd,
        bvsub,
        bvimul,
        bvshld,
        bvshrd,
        bvxor,
        bvand,
        bvor,
        bvshl,
        bvshr,
        bvsar,
        bvneg,
        bvnot,
        bvrol,
        bvror,
        bvquo,
        bvrem,
        equal,
        noequal,
        greater,
        less,
        bvzeroext,
        bvextract,
        bvconcat,
        bvsignext
    };

    class BitVector {
    private:
        void symbol_num_internal(const std::shared_ptr<BitVector> &v, std::set<int> &input) const;
        std::string print() const;
        void printV(std::stringstream& ss, uint32_t& length) const ;
    public:
        int id;                                    // a unique id for each value
        ValueType val_type;                        // value type: SYMBOL or CONCRETE
        uint32_t concrete_value;                   // concrete value
        uint32_t low_bit= 1;
        uint32_t high_bit = REGISTER_SIZE;
        static int idseed;
        std::string info;
        std::unique_ptr<Operation> opr = nullptr;


        BitVector() = delete;
        explicit BitVector(ValueType vty, std::string symbol_info);
        explicit BitVector(ValueType vty, uint32_t concrete, bool Imm2SymState);    // constructor for concrete value
        explicit BitVector(ValueType vty, std::unique_ptr<Operation> oper);
        explicit BitVector(ValueType vty, uint32_t con);


        static uint32_t arithmeticRightShift(uint32_t op1, uint32_t op2);

        static uint32_t rol32(uint32_t op1, uint32_t op2);

        static uint32_t ror32(uint32_t op1, uint32_t op2);

        static uint32_t shld32(uint32_t op0, uint32_t op1, uint32_t op2);

        static uint32_t shrd32(uint32_t op0, uint32_t op1, uint32_t op2);

        static uint32_t extract(uint32_t op1, uint32_t high, uint32_t low);

        static uint32_t concat(uint32_t op1, uint32_t op2, uint32_t op1_size, uint32_t op2_size);

        static uint32_t zeroext(uint32_t op1);

        static uint32_t signext(uint32_t op1, uint32_t origin_size, uint32_t new_size);

        bool isSymbol();

        uint32_t size();

        bool operator==(const BitVector &v1);

        uint32_t printV(std::stringstream& ss) const ;

        uint32_t symbol_num() const;

        friend std::ostream& operator<<(std::ostream& os, const BitVector& c);

    };

// An operation taking several values to calculate a result value
    class Operation {
    public:
        BVOper opty;
        std::shared_ptr<BitVector> val[3] = {nullptr, nullptr, nullptr};

        Operation(BVOper opt, std::shared_ptr<BitVector> v1);

        Operation(BVOper opt, std::shared_ptr<BitVector> v1, std::shared_ptr<BitVector> v2);

        Operation(BVOper opt, std::shared_ptr<BitVector> v1, std::shared_ptr<BitVector> v2,
                  std::shared_ptr<BitVector> v3);
    };

    std::shared_ptr<BitVector>
    buildop1(BVOper opty, std::shared_ptr<BitVector> v1);

    std::shared_ptr<BitVector>
    buildop2(BVOper opty, std::shared_ptr<BitVector> v1, std::shared_ptr<BitVector> v2);

    std::shared_ptr<BitVector>
    buildop2(BVOper opty, std::shared_ptr<BitVector> v1, uint32_t v2);

    std::shared_ptr<BitVector>
    buildop3(BVOper opty, std::shared_ptr<BitVector> v1, std::shared_ptr<BitVector> v2,
            std::shared_ptr<BitVector> v3);




}