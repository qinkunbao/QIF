#pragma once

#include <list>
#include <map>
#include <memory>
#include <string>
#include <tuple>
#include "ins_types.h"
#include "Register.h"
#include "Engine.h"
#include "BitVector.h"
#include "Constrains.h"

namespace tana {
    class BitVector;
    class Operation;


    class QIFSEEngine: public SEEngine{
    private:

        std::map<tana_type::T_ADDRESS, std::shared_ptr<BitVector> > memory;
        bool memory_find(uint32_t addr);
        std::map<std::string, std::shared_ptr<BitVector>> ctx;
        std::shared_ptr<BitVector> CF, OP, SF, ZF, AF, PF;
        std::vector<std::tuple<uint32_t, std::shared_ptr<tana::Constrain>>> constrains;
        uint32_t eip;

    public:

        using SEEngine::SEEngine;
        void init(std::vector<std::unique_ptr<Inst_Base>>::iterator it1,
                  std::vector<std::unique_ptr<Inst_Base>>::iterator it2,
                  tana_type::T_ADDRESS, tana_type::T_SIZE m_size);

        QIFSEEngine(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx,
                    uint32_t esi, uint32_t edi, uint32_t esp, uint32_t ebp);

        std::vector<std::shared_ptr<BitVector>> getAllOutput() override ;

        std::shared_ptr<BitVector> readReg(x86::x86_reg reg) override ;

        std::shared_ptr<BitVector> readReg(std::string reg) override ;

        bool writeReg(x86::x86_reg reg, std::shared_ptr<BitVector> v) override ;

        bool writeReg(std::string reg, std::shared_ptr<BitVector> v) override ;

        std::shared_ptr<BitVector> readMem(std::string memory_address, tana_type::T_SIZE size) override ;

        bool writeMem(std::string memory_address, tana_type::T_SIZE size, std::shared_ptr<BitVector> v) override ;

        int run() override ;

        void updateFlags(std::string, std::shared_ptr<BitVector>) override ;

        void clearFlags(std::string) override ;

        std::shared_ptr<tana::BitVector> getFlags(std::string) override;

        void updateConstrains(std::shared_ptr<Constrain> cons) override ;

    };




}