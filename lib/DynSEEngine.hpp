/*************************************************************************
	> File Name: DynSEEngine.h
	> Author: Qinkun Bao
	> Mail: qinkunbao@gmail.com
	> Created Time: Mon Apr 22 21:56:51 2019
 ************************************************************************/

#pragma once

#include <list>
#include <map>
#include <memory>
#include <string>
#include "ins_types.hpp"
#include "Register.hpp"
#include "Engine.hpp"
#include "BitVector.hpp"

namespace tana {
    class BitVector;

    class Operation;

    class DynSEEngine;


    class DynSEEngine : public SEEngine {
    private:

        std::map<tana_type::T_ADDRESS, std::shared_ptr<BitVector> > m_memory;

        bool memory_find(uint32_t addr);

        std::map<std::string, std::shared_ptr<BitVector>> m_ctx;


    public:

        using SEEngine::SEEngine;

        void initAllRegSymol(std::vector<std::unique_ptr<Inst_Base>>::iterator it1,
                             std::vector<std::unique_ptr<Inst_Base>>::iterator it2) override;

        DynSEEngine();

        std::vector<std::shared_ptr<BitVector>> getAllOutput() override;

        std::shared_ptr<BitVector> readReg(x86::x86_reg reg) override;

        std::shared_ptr<BitVector> readReg(std::string reg) override;

        bool writeReg(x86::x86_reg reg, std::shared_ptr<BitVector> v) override;

        bool writeReg(std::string reg, std::shared_ptr<BitVector> v) override;

        std::shared_ptr<BitVector> readMem(std::string memory_address, tana_type::T_SIZE size) override;

        bool writeMem(std::string memory_address, tana_type::T_SIZE size, std::shared_ptr<BitVector> v) override;

    };


}