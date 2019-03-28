#pragma once

#include <list>
#include <map>
#include <memory>
#include <string>
#include "ins_types.h"

namespace tana {

    class BitVector;
    class Operation;

    enum ValueTy {
        SYMBOL, CONCRETE
    };

    class BitVector {
    public:
        ValueTy valty;             // value type: SYMBOL or CONCRETE
        std::unique_ptr<Operation> opr = nullptr;
        std::string conval;             // concrete value
        int id;                 // a unique id for each value
        int low_bit = 0;
        int high_bit = REGISTER_SIZE;
        static int idseed;

        explicit BitVector(ValueTy vty);

        explicit BitVector(ValueTy vty, std::string con); // constructor for concrete value
        explicit BitVector(ValueTy vty, uint32_t concrete, bool Imm2SymState);    // constructor for concrete value
        explicit BitVector(ValueTy vty, std::unique_ptr<Operation> oper);
        explicit BitVector(ValueTy vty, uint32_t con);


        static uint32_t arithmeticRightShift(uint32_t op1, uint32_t op2);

        static uint32_t rol32(uint32_t op1, uint32_t op2);

        static uint32_t ror32(uint32_t op1, uint32_t op2);

        static uint32_t shld32(uint32_t op0, uint32_t op1, uint32_t op2);

        static uint32_t shrd32(uint32_t op0, uint32_t op1, uint32_t op2);

        static uint32_t extract(uint32_t op1, uint32_t high, uint32_t low);

        static uint32_t concat(uint32_t op1, uint32_t op2);

        bool isSymbol();

        bool operator==(const BitVector &v1);

        std::string print();

    };

// An operation taking several values to calculate a result value
    class Operation {
    public:
        std::string opty;
        std::shared_ptr<BitVector> val[3] = {nullptr, nullptr, nullptr};

        Operation(std::string opt, std::shared_ptr<BitVector> v1);

        Operation(std::string opt, std::shared_ptr<BitVector> v1, std::shared_ptr<BitVector> v2);

        Operation(std::string opt, std::shared_ptr<BitVector> v1, std::shared_ptr<BitVector> v2,
                  std::shared_ptr<BitVector> v3);
    };

    std::shared_ptr<BitVector>
    buildop1(std::string opty, std::shared_ptr<BitVector> v1);

    std::shared_ptr<BitVector>
    buildop2(std::string opty, std::shared_ptr<BitVector> v1, std::shared_ptr<BitVector> v2);

    std::shared_ptr<BitVector>
    buildop3(std::string opty, std::shared_ptr<BitVector> v1, std::shared_ptr<BitVector> v2,
            std::shared_ptr<BitVector> v3);

}