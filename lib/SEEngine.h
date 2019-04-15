#pragma once

#include <list>
#include <map>
#include <memory>
#include "ins_types.h"
#include "Register.h"
#include "Engine.h"
#include "BitVector.h"

namespace tana {
    class BitVector;
    class Operation;
    class SEEngine;


    class SEEngine: private Engine {
    private:
        bool imm2sym = false;
        std::map<std::string, std::shared_ptr<BitVector>> ctx;
        std::vector<std::unique_ptr<Inst_Dyn>>::iterator start;
        std::vector<std::unique_ptr<Inst_Dyn>>::iterator end;
        std::map<tana_type::T_ADDRESS, std::shared_ptr<BitVector> > memory;
        bool memory_find(uint32_t addr);


    public:
        explicit SEEngine(bool type);

        std::shared_ptr<BitVector> ZeroExt(std::shared_ptr<BitVector> v, tana_type::T_SIZE);

        std::shared_ptr<BitVector> SignExt(std::shared_ptr<BitVector> v, tana_type::T_SIZE orgin_size,
                                           tana_type::T_SIZE new_size);

        std::shared_ptr<BitVector> Extract(std::shared_ptr<BitVector> v, int low, int high);
        std::shared_ptr<BitVector> Concat(std::shared_ptr<BitVector> v1, std::shared_ptr<BitVector> v2);
        std::shared_ptr<BitVector> Concat(std::shared_ptr<BitVector> v1, std::shared_ptr<BitVector> v2,
                                          std::shared_ptr<BitVector> v3);

        static void getFormulaLength(const std::shared_ptr<BitVector> &v, uint32_t &len);

        void init(std::shared_ptr<BitVector> v1, std::shared_ptr<BitVector> v2,
                  std::shared_ptr<BitVector> v3, std::shared_ptr<BitVector> v4,
                  std::shared_ptr<BitVector> v5, std::shared_ptr<BitVector> v6,
                  std::shared_ptr<BitVector> v7, std::shared_ptr<BitVector> v8,
                  std::vector<std::unique_ptr<Inst_Dyn>>::iterator it1,
                  std::vector<std::unique_ptr<Inst_Dyn>>::iterator it2);

        void init(std::vector<std::unique_ptr<Inst_Dyn>>::iterator it1,
                  std::vector<std::unique_ptr<Inst_Dyn>>::iterator it2);

        void initAllRegSymol(std::vector<std::unique_ptr<Inst_Dyn>>::iterator it1,
                             std::vector<std::unique_ptr<Inst_Dyn>>::iterator it2);

        int run();

        bool isImmSym(uint32_t num);

        uint32_t conexec(std::shared_ptr<BitVector> f, std::map<std::shared_ptr<BitVector>, uint32_t> *input);

        std::vector<std::shared_ptr<BitVector> > getAllOutput();

        std::vector<std::shared_ptr<BitVector> > reduceValues(std::vector<std::shared_ptr<BitVector>> values);

        uint32_t eval(const std::shared_ptr<BitVector> &v, std::map<std::shared_ptr<BitVector>, uint32_t> *inmap);

        uint32_t eval(const std::shared_ptr<BitVector> &v);

        std::shared_ptr<BitVector> readReg(x86::x86_reg reg);

        std::shared_ptr<BitVector> readReg(std::string reg);

        bool writeReg(x86::x86_reg reg, std::shared_ptr<BitVector> v);

        bool writeReg(std::string reg, std::shared_ptr<BitVector> v);

        std::shared_ptr<BitVector> readMem(tana_type::T_ADDRESS memory_address, tana_type::T_SIZE size);

        bool writeMem(tana_type::T_ADDRESS memory_address, tana_type::T_SIZE size, std::shared_ptr<BitVector> v);

        std::shared_ptr<BitVector> formula_simplfy(std::shared_ptr<BitVector> v);

    };


}