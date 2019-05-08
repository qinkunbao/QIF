#include <cassert>
#include <algorithm>
#include <limits.h>
#include <sstream>
#include "ins_types.h"
#include "QIFSEEngine.h"
#include "VarMap.h"
#include "error.h"

#define ERROR(MESSAGE) tana::default_error_handler(__FILE__, __LINE__, MESSAGE)


namespace tana {
    QIFSEEngine::QIFSEEngine(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi,
                             uint32_t esp, uint32_t ebp) : SEEngine(false), CF(nullptr), OF(nullptr), SF(nullptr),
                                                           ZF(nullptr), AF(nullptr), PF(nullptr), eip(0) {

        ctx["eax"] = std::make_shared<BitVector>(ValueType ::CONCRETE, eax);
        ctx["ebx"] = std::make_shared<BitVector>(ValueType ::CONCRETE, ebx);
        ctx["ecx"] = std::make_shared<BitVector>(ValueType ::CONCRETE, ecx);
        ctx["edx"] = std::make_shared<BitVector>(ValueType ::CONCRETE, edx);
        ctx["esi"] = std::make_shared<BitVector>(ValueType ::CONCRETE, esi);
        ctx["edi"] = std::make_shared<BitVector>(ValueType ::CONCRETE, edi);
        ctx["esp"] = std::make_shared<BitVector>(ValueType ::CONCRETE, esp);
        ctx["ebp"] = std::make_shared<BitVector>(ValueType ::CONCRETE, ebp);

        eflags = true;

    }

    void QIFSEEngine::init(std::vector<std::unique_ptr<Inst_Base>>::iterator it1,
                           std::vector<std::unique_ptr<Inst_Base>>::iterator it2,
                           tana_type::T_ADDRESS address, tana_type::T_SIZE m_size) {
        this->start = it1;
        this->end = it2;
        auto reminder = m_size % 4;
        if (reminder) {
            m_size = m_size + (4 - reminder);
        }
        assert(m_size % 4 == 0);

        std::shared_ptr<BitVector> v0;
        std::stringstream ss;

        for (auto offset = 0; offset < m_size; offset = offset + 4) {
            ss << "Key" << offset / 4;
            v0 = std::make_shared<BitVector>(ValueType ::SYMBOL, ss.str());
            memory[address + offset] = v0;
        }

    }

    std::vector<std::shared_ptr<BitVector>> QIFSEEngine::getAllOutput() {
        std::vector<std::shared_ptr<BitVector>> outputs;
        auto v = std::make_shared<BitVector>(ValueType ::CONCRETE, 0);
        outputs.push_back(v);
        return outputs;
    }

    std::shared_ptr<BitVector> QIFSEEngine::readReg(const std::string reg) {
        x86::x86_reg reg_id = x86::reg_string2id(reg);
        return readReg(reg_id);
    }

    std::shared_ptr<BitVector> QIFSEEngine::readReg(const x86::x86_reg reg) {
        RegType type = Registers::getRegType(reg);
        if (type == FULL) {
            auto index = Registers::getRegIndex(reg);
            std::string strName = Registers::convertRegID2RegName(index);
            std::shared_ptr<BitVector> res = ctx[strName];
            return res;
        }

        if (type == HALF) {
            auto index = Registers::getRegIndex(reg);
            std::string strName = Registers::convertRegID2RegName(index);
            std::shared_ptr<BitVector> origin = ctx[strName];
            std::shared_ptr<BitVector> res = SEEngine::Extract(origin, 1, 16);
            return res;
        }

        if (type == QLOW) {
            auto index = Registers::getRegIndex(reg);
            std::string strName = Registers::convertRegID2RegName(index);
            std::shared_ptr<BitVector> origin = ctx[strName];
            std::shared_ptr<BitVector> res = SEEngine::Extract(origin, 1, 8);
            return res;
        }

        if (type == QHIGH) {
            auto index = Registers::getRegIndex(reg);
            std::string strName = Registers::convertRegID2RegName(index);
            std::shared_ptr<BitVector> origin = ctx[strName];
            std::shared_ptr<BitVector> res = SEEngine::Extract(origin, 9, 16);
            return res;
        }

        return nullptr;

    }

    bool QIFSEEngine::writeReg(const x86::x86_reg reg, std::shared_ptr<tana::BitVector> v) {
        RegType type = Registers::getRegType(reg);
        uint32_t reg_index = Registers::getRegIndex(reg);
        std::string index_name = Registers::convertRegID2RegName(reg_index);
        if (type == FULL) {
            ctx[index_name] = v;
            return true;
        }
        if (type == HALF) {
            auto origin = ctx[index_name];
            auto reg_part = Extract(origin, 17, 32);
            assert(v->size() == (REGISTER_SIZE / 2));
            auto v_reg = Concat(reg_part, v);
            assert(v_reg->size() == REGISTER_SIZE);
            ctx[index_name] = v_reg;
            return true;
        }
        if (type == QLOW) {
            auto origin = ctx[index_name];
            auto reg_part = Extract(origin, 9, 32);
            assert(v->size() == (REGISTER_SIZE / 4));
            auto v_reg = Concat(reg_part, v);
            assert(v_reg->size() == REGISTER_SIZE);
            ctx[index_name] = v_reg;
            return true;
        }

        if (type == QHIGH) {
            auto origin = ctx[index_name];
            auto reg_part1 = Extract(origin, 1, 8);
            auto reg_part2 = Extract(origin, 17, 32);
            assert(v->size() == (REGISTER_SIZE / 4));
            auto v_reg = Concat(reg_part2, v, reg_part1);
            assert(v_reg->size() == REGISTER_SIZE);
            ctx[index_name] = v_reg;
            return true;
        }
        ERROR("Unkown reg type");
        return false;

    }

    bool QIFSEEngine::writeReg(const std::string reg, std::shared_ptr<tana::BitVector> v) {
        x86::x86_reg reg_id = x86::reg_string2id(reg);
        return writeReg(reg_id, v);
    }

    bool QIFSEEngine::memory_find(uint32_t addr) {
        auto ii = memory.find(addr);
        if (ii == memory.end())
            return false;
        else
            return true;
    }


    std::shared_ptr<BitVector> QIFSEEngine::readMem(std::string memory_address_str, tana_type::T_SIZE size) {
        uint32_t memory_address = std::stoul(memory_address_str, nullptr, 16);
        std::shared_ptr<BitVector> v0, v1;
        if (size == T_BYTE_SIZE * T_DWORD) {
            if (memory_find(memory_address)) {
                v0 = memory[memory_address];
            } else {
                std::stringstream ss;
                ss << "Environment data: " << memory_address_str << std::dec;
                v0 = std::make_shared<BitVector>(ValueType ::CONCRETE, mem_data);
                memory[memory_address] = v0;
            }
            return v0;
        }

        if (size == T_BYTE_SIZE * T_WORD) {
            uint32_t offset = memory_address % 2;
            if (memory_find(memory_address - offset)) {
                v0 = memory[memory_address - offset];
            } else {
                std::stringstream ss;
                ss << "Environment data: " << memory_address_str << std::dec;
                v0 = std::make_shared<BitVector>(ValueType ::CONCRETE, mem_data);
                memory[memory_address - offset] = v0;
                v1 = DynSEEngine::Extract(v0, 1, 16);
                return v1;
            }
            if(offset == 0)
            {
                v1 = DynSEEngine::Extract(v0, 1, 16);
            } else{
                v1 = DynSEEngine::Extract(v0, 17, 32);
            }

            return v1;
        }

        if (size == T_BYTE_SIZE * T_BYTE)
        {
            uint32_t offset = memory_address % 4;
            if (memory_find(memory_address - offset)) {
                v0 = memory[memory_address - offset];
            } else {
                std::stringstream ss;
                ss << "Environment data: " << memory_address_str << std::dec;
                v0 = std::make_shared<BitVector>(ValueType ::CONCRETE, mem_data);
                memory[memory_address - offset] = v0;
                v1 = DynSEEngine::Extract(v0, 1, 8);
                return v1;
            }


            if(offset == 0) {
                v1 = DynSEEngine::Extract(v0, 1, 8);
            }
            else if(offset == 1) {
                v1 = DynSEEngine::Extract(v0, 9, 16);
            } else if(offset == 2)
            {
                v1 = DynSEEngine::Extract(v0, 17, 24);

            } else{
                v1 = DynSEEngine::Extract(v0, 25, 32);

            }

            return v1;
        }

        ERROR("Invalid data size");

        return v0;

    }


    bool QIFSEEngine::writeMem(std::string memory_address_str, tana::tana_type::T_SIZE addr_size,
                               std::shared_ptr<tana::BitVector> v) {
        assert(v->size() == addr_size || !v->isSymbol());
        uint32_t memory_address = std::stoul(memory_address_str, nullptr, 16);
        std::shared_ptr<BitVector> v0, v_mem;

        if (addr_size == T_BYTE_SIZE * T_DWORD) {
            assert(memory_address % 4 == 0);
            memory[memory_address] = v;
            return true;
        }




        if (addr_size == T_BYTE_SIZE * T_WORD) {
            uint32_t offset = memory_address % 2;
            if (memory_find(memory_address - offset)) {
                v0 = memory[memory_address - offset];
            } else {
                std::stringstream ss;
                ss << "Mem:" << std::hex << memory_address << std::dec;
                v0 = std::make_shared<BitVector>(ValueType ::CONCRETE, ss.str());
                memory[memory_address - offset] = v0;
            }


            std::shared_ptr<BitVector> v1 = DynSEEngine::Extract(v0, 1, 16);
            if (!v->isSymbol()) {
                v = DynSEEngine::Extract(v, 1, 16);
            }
            v_mem = Concat(v1, v);
            memory[memory_address - offset] = v_mem;
            return true;
        }

        if (addr_size == T_BYTE_SIZE * T_BYTE) {
            uint32_t offset = memory_address % 4;
            if (memory_find(memory_address - offset)) {
                v0 = memory[memory_address - offset];
            } else {
                std::stringstream ss;
                ss << "Mem:" << std::hex << memory_address << std::dec;
                v0 = std::make_shared<BitVector>(ValueType ::CONCRETE, ss.str());
                memory[memory_address - offset] = v0;
            }

            std::shared_ptr<BitVector> v1 = DynSEEngine::Extract(v0, 1, 8);
            if (!v->isSymbol()) {
                v = DynSEEngine::Extract(v, 1, 8);
            }
            v_mem = Concat(v1, v);
            memory[memory_address - offset] = v_mem;
            return true;
        }
        return false;
    }

    int
    QIFSEEngine::run() {
        for (auto inst = start; inst != end; ++inst) {
            auto it = inst->get();
            eip = it->addrn;
            mem_data = it->read_mem_data();
            bool status = it->symbolic_execution(this);

            if (!status) {
                ERROR("No recognized instruction");
                return false;
            }

        }
        return true;
    }

    void QIFSEEngine::updateFlags(std::string flag_name, std::shared_ptr<BitVector> cons)
    {
        if(flag_name == "CF")
        {
            this->CF = cons;
            return;
        }

        if(flag_name == "OF")
        {
            this->OF = cons;
            return;
        }

        if(flag_name == "SF")
        {
            this->SF = cons;
            return;
        }

        if(flag_name == "ZF")
        {
            this->ZF = cons;
            return;
        }

        if(flag_name == "AF")
        {
            this->AF = cons;
            return;
        }

        if(flag_name == "PF")
        {
            this->PF = cons;
            return;
        }

        ERROR("Not recognized flag_name");
    }

    void QIFSEEngine::clearFlags(std::string flag_name)
    {
        std::shared_ptr<BitVector> con = std::make_shared<BitVector>(ValueType::CONCRETE, 0);
        this->updateFlags(flag_name, con);
    }

    std::shared_ptr<tana::BitVector> QIFSEEngine::getFlags(std::string flag_name)
    {
        if(flag_name == "CF")
        {
            return CF;
        }

        if(flag_name == "OF")
        {
            return OF;
        }

        if(flag_name == "SF")
        {
            return SF;
        }

        if(flag_name == "ZF")
        {
            return ZF;
        }

        if(flag_name == "AF")
        {
            return AF;
        }

        if(flag_name == "PF")
        {
            return PF;
        }

        ERROR("Not recognized flag_name");
        return nullptr;
    }

    void QIFSEEngine::updateConstrains(std::shared_ptr<Constrain> cons)
    {
        auto res = std::make_tuple(this->eip, cons);
        constrains.push_back(res);
    }

    void QIFSEEngine::outputConstrains()
    {
        for(const auto& element : constrains)
        {
            auto addr = std::get<0>(element);
            auto &con = std::get<1>(element);
            std::cout << "Addr: "  << std:: hex << addr << std::dec;
            std::cout << " Constrain: " << *con << std::endl;
        }

    }


}