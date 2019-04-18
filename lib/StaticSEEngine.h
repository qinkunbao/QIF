#pragma once

#include <list>
#include <map>
#include <memory>
#include <string>
#include "ins_types.h"
#include "Register.h"
#include "Engine.h"
#include "BitVector.h"
#include "Blocks.h"

namespace tana {
    class BitVector;
    class Operation;
    class DynSEEngine;


    class StaticSEEngine: public SEEngine{
    private:

        std::map<std::string, std::shared_ptr<BitVector> > memory;
        bool memory_find(std::string);
        std::map<std::string, std::shared_ptr<BitVector>> ctx;


    public:

        using SEEngine::SEEngine;

        void initAllRegSymol(std::vector<std::unique_ptr<Inst_Base>>::iterator it1,
                                     std::vector<std::unique_ptr<Inst_Base>>::iterator it2) override ;

        StaticSEEngine();

        void reset();

        void initFromBlock(Block &b);
        std::vector<std::shared_ptr<BitVector>> getAllOutput() override ;

        std::shared_ptr<BitVector> readReg(x86::x86_reg reg) override ;

        std::shared_ptr<BitVector> readReg(std::string reg) override ;

        bool writeReg(x86::x86_reg reg, std::shared_ptr<BitVector> v) override ;

        bool writeReg(std::string reg, std::shared_ptr<BitVector> v) override ;

        std::shared_ptr<BitVector> readMem(std::string memory_address, tana_type::T_SIZE size) override ;

        bool writeMem(std::string memory_address, tana_type::T_SIZE size, std::shared_ptr<BitVector> v) override ;

    };


}