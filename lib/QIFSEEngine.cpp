/*************************************************************************
	> File Name: QIFSEEngine.cpp
	> Author: Qinkun Bao
	> Mail: qinkunbao@gmail.com
	> Created Time: Mon Apr 22 21:56:51 2019
 ************************************************************************/

#include <cassert>
#include <algorithm>
#include <limits.h>
#include <sstream>
#include <cmath>
#include <chrono>
#include "ins_types.h"
#include "QIFSEEngine.h"
#include "VarMap.h"
#include "error.h"
#include "Register.h"
#include "MonteCarlo.h"

#define ERROR(MESSAGE) tana::default_error_handler(__FILE__, __LINE__, MESSAGE)


namespace tana {
    QIFSEEngine::QIFSEEngine(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi,
                             uint32_t esp, uint32_t ebp) : SEEngine(false), CF(nullptr), OF(nullptr), SF(nullptr),
                                                           ZF(nullptr), AF(nullptr), PF(nullptr), eip(0), mem_data(0) {

        ctx["eax"] = std::make_shared<BitVector>(ValueType::CONCRETE, eax);
        ctx["ebx"] = std::make_shared<BitVector>(ValueType::CONCRETE, ebx);
        ctx["ecx"] = std::make_shared<BitVector>(ValueType::CONCRETE, ecx);
        ctx["edx"] = std::make_shared<BitVector>(ValueType::CONCRETE, edx);
        ctx["esi"] = std::make_shared<BitVector>(ValueType::CONCRETE, esi);
        ctx["edi"] = std::make_shared<BitVector>(ValueType::CONCRETE, edi);
        ctx["esp"] = std::make_shared<BitVector>(ValueType::CONCRETE, esp);
        ctx["ebp"] = std::make_shared<BitVector>(ValueType::CONCRETE, ebp);

        eflags = true;

    }

    void QIFSEEngine::init(std::vector<std::unique_ptr<Inst_Base>>::iterator it1,
                           std::vector<std::unique_ptr<Inst_Base>>::iterator it2,
                           tana_type::T_ADDRESS address, tana_type::T_SIZE m_size,
                           std::vector<uint8_t> key_value) {
        this->start = it1;
        this->end = it2;

        std::shared_ptr<BitVector> v0;
        std::stringstream ss;

        for (auto offset = 0; offset < m_size; offset = offset + 1) {
            ss << "Key" << offset;
            v0 = std::make_shared<BitVector>(ValueType::SYMBOL, ss.str(), T_BYTE_SIZE);
            std::stringstream mem_addr;
            mem_addr << std::hex << address + offset << std::dec;
            std::string memoryAddr = mem_addr.str();
            this->writeMem(memoryAddr, v0->size(), v0);
            ss.str("");
            //this->printMemory();
            key_value_map.insert(std::pair<int, uint32_t>(v0->id, key_value[offset]));
        }
        //this->printMemory();

    }

    std::shared_ptr<BitVector>
    QIFSEEngine::Extract(std::shared_ptr<BitVector> v, int low, int high) {
        assert(high > low);
        //std::cout << "Before Extract Debug: " << *v <<  " low: "<<  low <<  " high: "<< high << std::endl;
        std::shared_ptr<BitVector> res = nullptr;


        if (!v->isSymbol()) {
            // v is a concrete value
            uint32_t result = eval(v);
            result = BitVector::extract(result, high, low);
            res = std::make_shared<BitVector>(ValueType::CONCRETE, result);
            res->low_bit = 1;
            res->high_bit = high - low + 1;

            //std::cout << "Before Extract Debug: "<< *v <<" low: "<< low << " high: " << high << std::endl
            //          <<" After Extract Debug: " << *res <<  " low: "<<  res->low_bit <<" high: "<< res->high_bit
            //          << std::endl;
            return res;
        }

        std::unique_ptr<Operation> oper = std::make_unique<Operation>(BVOper::bvextract, v);
        auto &ref_opr = v->opr;
        bool optimized_flag = false;


        if (low == 1 && high == 16) {
            auto v_part1 = QIFSEEngine::Extract(v, 1, 8);
            auto v_part2 = QIFSEEngine::Extract(v, 9, 16);
            res = QIFSEEngine::Concat(v_part2, v_part1);
            optimized_flag = true;

        }

        if (low == 9 && high == 24) {
            auto v_part1 = QIFSEEngine::Extract(v, 9, 16);
            auto v_part2 = QIFSEEngine::Extract(v, 17, 24);
            res = QIFSEEngine::Concat(v_part2, v_part1);
            optimized_flag = true;

        }

        if (low == 17 && high == 32) {
            auto v_part1 = QIFSEEngine::Extract(v, 17, 24);
            auto v_part2 = QIFSEEngine::Extract(v, 25, 32);
            res = QIFSEEngine::Concat(v_part2, v_part1);
            optimized_flag = true;

        }

        if (low == 1 && high == 24) {
            auto v_part1 = QIFSEEngine::Extract(v, 1, 8);
            auto v_part2 = QIFSEEngine::Extract(v, 9, 16);
            auto v_part3 = QIFSEEngine::Extract(v, 17, 24);
            res = QIFSEEngine::Concat(v_part3, v_part2, v_part1);
            optimized_flag = true;
        }

        if (low == 9 && high == 32) {
            auto v_part1 = QIFSEEngine::Extract(v, 9, 16);
            auto v_part2 = QIFSEEngine::Extract(v, 17, 24);
            auto v_part3 = QIFSEEngine::Extract(v, 25, 32);
            res = QIFSEEngine::Concat(v_part3, v_part2, v_part1);
            optimized_flag = true;
        }


        if (v->isSymbol()) {
            if (ref_opr == nullptr) {
                v->low_bit = low;
                v->high_bit = high;
                res = v;
            }

            uint32_t v_min = 0, v_max = 0;
            auto &v0 = ref_opr->val[0];
            auto &v1 = ref_opr->val[1];
            auto &v2 = ref_opr->val[2];
            uint32_t v0_size = 0, v1_size = 0, v2_size = 0;


            if (ref_opr->opty == BVOper::bvconcat) {
                if (ref_opr->val[2] != nullptr) {
                    v_min = v2->low_bit;
                    v_max = v2->size();
                    v2_size = v2->size();

                    if ((v_min == low) && (v_max == high)) {
                        res = v2;
                        optimized_flag = true;
                    } else if ((v_min <= low) && (v_max >= high)) {
                        res = QIFSEEngine::Extract(v2, low, high);
                        optimized_flag = true;
                    }
                }

                if (ref_opr->val[1] != nullptr) {
                    v_min = v_max + 1;
                    v_max = v_min + v1->size() - 1;
                    v1_size = v1->size();

                    if ((v_min == low) && (v_max == high)) {
                        res = v1;
                        optimized_flag = true;
                    } else if ((v_min <= low) && (v_max >= high)) {
                        res = QIFSEEngine::Extract(v1, low - v2_size, high - v2_size);
                        optimized_flag = true;
                    }
                }

                if (v->opr->val[0] != nullptr) {
                    v_min = v_max + 1;
                    v_max = v_min + v0->size() - 1;
                    if ((v_min == low) && (v_max == high)) {
                        res = v0;
                        optimized_flag = true;
                    } else if ((v_min <= low) && (v_max >= high)) {
                        res = QIFSEEngine::Extract(v0, low - v2_size - v1_size, high - v2_size - v1_size);
                        optimized_flag = true;
                    }
                }
            }

            if (!optimized_flag) {
                res = std::make_shared<BitVector>(ValueType::SYMBOL, std::move(oper));
                res->high_bit = high;
                res->low_bit = low;
            }
        }


        //std::cout << "Before Extract Debug: "<< *v <<" low: "<< low << " high: " << high << std::endl
        //          <<" After Extract Debug: " << *res <<  " low: "<<  res->low_bit <<" high: "<< res->high_bit
        //          << std::endl;
        return res;

    }

    std::vector<std::shared_ptr<BitVector>> QIFSEEngine::getAllOutput() {
        std::vector<std::shared_ptr<BitVector>> outputs;
        auto v = std::make_shared<BitVector>(ValueType::CONCRETE, 0);
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
            std::shared_ptr<BitVector> res = QIFSEEngine::Extract(origin, 1, 16);
            return res;
        }

        if (type == QLOW) {
            auto index = Registers::getRegIndex(reg);
            std::string strName = Registers::convertRegID2RegName(index);
            std::shared_ptr<BitVector> origin = ctx[strName];
            std::shared_ptr<BitVector> res = QIFSEEngine::Extract(origin, 1, 8);
            return res;
        }

        if (type == QHIGH) {
            auto index = Registers::getRegIndex(reg);
            std::string strName = Registers::convertRegID2RegName(index);
            std::shared_ptr<BitVector> origin = ctx[strName];
            std::shared_ptr<BitVector> res = QIFSEEngine::Extract(origin, 9, 16);
            return res;
        }

        return nullptr;

    }

    bool QIFSEEngine::writeReg(const x86::x86_reg reg, std::shared_ptr<tana::BitVector> v) {
        RegType type = Registers::getRegType(reg);
        uint32_t reg_index = Registers::getRegIndex(reg);
        std::string index_name = Registers::convertRegID2RegName(reg_index);
        if (type == FULL) {
            assert(v->size() == REGISTER_SIZE);
            ctx[index_name] = v;
            return true;
        }
        if (type == HALF) {
            auto origin = ctx[index_name];
            auto reg_part = QIFSEEngine::Extract(origin, 17, 32);
            assert(v->size() == (REGISTER_SIZE / 2));
            auto v_reg = QIFSEEngine::Concat(reg_part, v);
            assert(v_reg->size() == REGISTER_SIZE);
            ctx[index_name] = v_reg;
            return true;
        }
        if (type == QLOW) {
            auto origin = ctx[index_name];
            auto reg_part = QIFSEEngine::Extract(origin, 9, 32);
            assert(v->size() == (REGISTER_SIZE / 4));
            auto v_reg = QIFSEEngine::Concat(reg_part, v);
            assert(v_reg->size() == REGISTER_SIZE);
            ctx[index_name] = v_reg;
            return true;
        }

        if (type == QHIGH) {
            auto origin = ctx[index_name];
            auto reg_part1 = QIFSEEngine::Extract(origin, 1, 8);
            auto reg_part2 = QIFSEEngine::Extract(origin, 17, 32);
            assert(v->size() == (REGISTER_SIZE / 4));
            auto v_reg = QIFSEEngine::Concat(reg_part2, v, reg_part1);
            assert(v_reg->size() == REGISTER_SIZE);
            ctx[index_name] = v_reg;
            return true;
        }
        ERROR("Unknown reg type");
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
        uint32_t offset = memory_address % 4;
        //Debug
        if (memory_find(memory_address - offset)) {
            std::shared_ptr<BitVector> v_test = memory.at(memory_address - offset);
            //debug_map(key_value_map);
            //std::cout << *v_test << std::endl;
            uint32_t calculate = 0, con = 0;
            if (v_test->symbol_num() == 0) {
                calculate = QIFSEEngine::eval(v_test);
            } else {
                calculate = QIFSEEngine::eval(v_test, key_value_map);
            }
            con = mem_data;
            auto con_t = BitVector::extract(con, size, 1);
            auto calculate_t = BitVector::extract(calculate, size, 1);

            if (con_t != calculate_t) {

                std::cout << std::endl << "Mem :" << *v_test << " == " << std::hex << con << std::dec << std::endl;
                std::cout << std::endl << memory_address_str << std::endl;
                ERROR("Memory Error");
                memory[memory_address - offset] = std::make_shared<BitVector>(ValueType::CONCRETE, mem_data);
            }

        }
        //Debug


        if (size == T_BYTE_SIZE * T_DWORD) {
            //assert(memory_address % 4 == 0);
            if (memory_find(memory_address)) {
                v0 = memory[memory_address];
            } else {
                std::stringstream ss;
                ss << "Environment data: " << memory_address_str << std::dec;
                v0 = std::make_shared<BitVector>(ValueType::CONCRETE, mem_data);
                memory[memory_address] = v0;
                assert(v0 != nullptr);
            }
            return v0;
        }

        if (size == T_BYTE_SIZE * T_WORD) {
            if (memory_find(memory_address - offset)) {
                v0 = memory[memory_address - offset];
            } else {
                std::stringstream ss;
                ss << "Environment data: " << memory_address_str << std::dec;
                v0 = std::make_shared<BitVector>(ValueType::CONCRETE, mem_data);
                memory[memory_address - offset] = v0;
                assert(v0 != nullptr);
            }
            if (offset == 0) {
                v1 = QIFSEEngine::Extract(v0, 1, 16);
            } else {
                v1 = QIFSEEngine::Extract(v0, 17, 32);
            }

            return v1;
        }

        if (size == T_BYTE_SIZE * T_BYTE) {
            if (memory_find(memory_address - offset)) {
                v0 = memory[memory_address - offset];
            } else {
                std::stringstream ss;
                ss << "Environment data: " << memory_address_str << std::dec;
                v0 = std::make_shared<BitVector>(ValueType::CONCRETE, mem_data);
                memory[memory_address - offset] = v0;
                assert(v0 != nullptr);
            }
            if (offset == 0) {
                v1 = QIFSEEngine::Extract(v0, 1, 8);
            } else if (offset == 1) {
                v1 = SEEngine::Extract(v0, 9, 16);
            } else if (offset == 2) {
                v1 = QIFSEEngine::Extract(v0, 17, 24);

            } else {
                v1 = QIFSEEngine::Extract(v0, 25, 32);

            }

            return v1;
        }

        ERROR("Invalid data size");

        return v0;

    }


    bool QIFSEEngine::writeMem(std::string memory_address_str, tana::tana_type::T_SIZE addr_size,
                               std::shared_ptr<tana::BitVector> v) {
        assert(v->size() == addr_size);
        uint32_t memory_address = std::stoul(memory_address_str, nullptr, 16);
        std::shared_ptr<BitVector> v_mem_origin, v_mem;
        uint32_t offset = memory_address % 4;



        if (addr_size == T_BYTE_SIZE * T_DWORD) {
            //assert(memory_address % 4 == 0);
            memory[memory_address] = v;
            assert(v != nullptr);
            return true;
        }

        if (addr_size == T_BYTE_SIZE * T_WORD) {
            if (memory_find(memory_address - offset)) {
                v_mem_origin = memory[memory_address - offset];
            } else {
                std::stringstream ss;
                ss << "Mem:" << std::hex << memory_address << std::dec;
                v_mem_origin = std::make_shared<BitVector>(ValueType::CONCRETE, mem_data);
                assert(v_mem_origin != nullptr);
                memory[memory_address - offset] = v_mem_origin;
            }

            if (offset == 0) {
                std::shared_ptr<BitVector> v1 = SEEngine::Extract(v_mem_origin, 17, 32);
                if (!v->isSymbol()) {
                    v = QIFSEEngine::Extract(v, 1, 16);
                }
                v_mem = QIFSEEngine::Concat(v1, v);
            } else {
                std::shared_ptr<BitVector> v1 = SEEngine::Extract(v_mem_origin, 1, 16);
                if (!v->isSymbol()) {
                    v = QIFSEEngine::Extract(v, 1, 16);
                }
                v_mem = QIFSEEngine::Concat(v, v1);
            }
            memory[memory_address - offset] = v_mem;
            assert(v_mem != nullptr);
            return true;
        }

        if (addr_size == T_BYTE_SIZE * T_BYTE) {
            if (memory_find(memory_address - offset)) {
                v_mem_origin = memory[memory_address - offset];
            } else {
                std::stringstream ss;
                ss << "Mem:" << std::hex << memory_address << std::dec;
                v_mem_origin = std::make_shared<BitVector>(ValueType::CONCRETE, mem_data);
                assert(v_mem_origin != nullptr);
                memory[memory_address - offset] = v_mem_origin;
            }

            if (offset == 0) {
                std::shared_ptr<BitVector> v1 = QIFSEEngine::Extract(v_mem_origin, 9, 32);
                if (!v->isSymbol()) {
                    v = QIFSEEngine::Extract(v, 1, 8);
                }
                v_mem = QIFSEEngine::Concat(v1, v);
            } else if (offset == 1) {
                std::cout << *v_mem_origin << std::endl;
                std::shared_ptr<BitVector> v1 = QIFSEEngine::Extract(v_mem_origin, 1, 8);
                std::shared_ptr<BitVector> v2 = QIFSEEngine::Extract(v_mem_origin, 17, 32);
                if (!v->isSymbol()) {
                    v = QIFSEEngine::Extract(v, 1, 8);
                }
                v_mem = QIFSEEngine::Concat(v2, v, v1);

            } else if (offset == 2) {
                std::shared_ptr<BitVector> v1 = QIFSEEngine::Extract(v_mem_origin, 1, 16);
                std::shared_ptr<BitVector> v2 = QIFSEEngine::Extract(v_mem_origin, 25, 32);
                if (!v->isSymbol()) {
                    v = QIFSEEngine::Extract(v, 1, 8);
                }
                v_mem = QIFSEEngine::Concat(v2, v, v1);
            } else {
                std::shared_ptr<BitVector> v1 = QIFSEEngine::Extract(v_mem_origin, 1, 24);
                if (!v->isSymbol()) {
                    v = QIFSEEngine::Extract(v, 1, 8);
                }
                v_mem = QIFSEEngine::Concat(v, v1);
            }

            assert(v_mem->size() == REGISTER_SIZE);
            memory[memory_address - offset] = v_mem;
            //std::cout << std::endl << "Debug: " << *v_mem <<std::endl;
            assert(v_mem != nullptr);
            return true;
        }
        return false;
    }

    int
    QIFSEEngine::run() {
        for (auto inst = start; std::next(inst) != end;) {
            auto it = inst->get();
            current_eip = it;
            ++inst;
            next_eip = inst->get();

            eip = it->addrn;
            mem_data = it->read_mem_data();

            checkMemoryAccess(it);

            bool status = it->symbolic_execution(this);
            std::vector<std::shared_ptr<BitVector>> sym_res;

            // Get symbolic register value after the SE
            sym_res.push_back(ctx["eax"]);
            sym_res.push_back(ctx["ebx"]);
            sym_res.push_back(ctx["ecx"]);
            sym_res.push_back(ctx["edx"]);
            sym_res.push_back(ctx["esi"]);
            sym_res.push_back(ctx["edi"]);
            sym_res.push_back(ctx["esp"]);
            sym_res.push_back(ctx["ebp"]);
            std::vector<uint32_t> con_res;

            // Get the  concrete register value after the SE
            if (inst != end) {
                for (uint32_t i = 0; i < 8; ++i) {
                    con_res.push_back(inst->get()->vcpu.gpr[i]);
                }

                //std::cout << it->id << ": ";
                for (uint32_t j = 0; j < 8; ++j) {
                    //std::cout << *sym_res[j] << " = " << std::hex << con_res[j] << std::dec <<" || ";
                    uint32_t res;
                    if ((sym_res[j])->symbol_num() == 0) {
                        res = eval(sym_res[j]);
                    } else {
                        res = eval(sym_res[j], key_value_map);
                    }
                    if ((res == con_res[j]) && ((sym_res[j])->symbol_num() == 0)) {
                        auto reg_v = std::make_shared<BitVector>(ValueType::CONCRETE, con_res[j]);
                        writeReg(Registers::convertRegID2RegName(j), reg_v);
                    }

                    if (res != con_res[j]) {
                        std::cout << "\n"
                                  << "Error: " << std::hex << it->addrn
                                  << "\n";

                        std::cout << "Register: " << Registers::convertRegID2RegName(j)
                                  << "\n";

                        std::cout << "Symbolic: " << res
                                  << "\n";

                        std::cout << "Concrete: " << con_res[j] << std::dec
                                  << "\n";

                        std::cout << "Previous: " << *it
                                  << "\n";

                        std::cout << "Present: " << *(inst->get())
                                  << std::endl;

                        auto reg_v = std::make_shared<BitVector>(ValueType::CONCRETE, con_res[j]);
                        writeReg(Registers::convertRegID2RegName(j), reg_v);
                        ERROR("ERROR");
                    }

                }
            }
            if (!status) {
                ERROR("No recognized instruction");
                return false;
            }

        }
        return true;
    }

    void QIFSEEngine::updateFlags(std::string flag_name, std::shared_ptr<BitVector> cons) {
        if (flag_name == "CF") {
            this->CF = cons;
            return;
        }

        if (flag_name == "OF") {
            this->OF = cons;
            return;
        }

        if (flag_name == "SF") {
            this->SF = cons;
            return;
        }

        if (flag_name == "ZF") {
            this->ZF = cons;
            return;
        }

        if (flag_name == "AF") {
            this->AF = cons;
            return;
        }

        if (flag_name == "PF") {
            this->PF = cons;
            return;
        }

        ERROR("Not recognized flag_name");
    }

    void QIFSEEngine::clearFlags(std::string flag_name) {
        std::shared_ptr<BitVector> con = std::make_shared<BitVector>(ValueType::CONCRETE, 0);
        this->updateFlags(flag_name, con);
    }

    std::shared_ptr<tana::BitVector> QIFSEEngine::getFlags(std::string flag_name) {
        if (flag_name == "CF") {
            return CF;
        }

        if (flag_name == "OF") {
            return OF;
        }

        if (flag_name == "SF") {
            return SF;
        }

        if (flag_name == "ZF") {
            return ZF;
        }

        if (flag_name == "AF") {
            return AF;
        }

        if (flag_name == "PF") {
            return PF;
        }

        ERROR("Not recognized flag_name");
        return nullptr;
    }

    void QIFSEEngine::updateCFConstrains(std::shared_ptr<Constrain> cons) {
        auto res = std::make_tuple(this->eip, cons, LeakageType::CFLeakage);
        constrains.push_back(res);
    }

    void QIFSEEngine::updateDAConstrains(std::shared_ptr<Constrain> cons) {
        auto res = std::make_tuple(this->eip, cons, LeakageType::DALeakage);
        constrains.push_back(res);
    }


    void QIFSEEngine::printConstrains() {
        std::cout << "\n";
        for (const auto &element : constrains) {
            auto addr = std::get<0>(element);
            auto &con = std::get<1>(element);
            LeakageType type = std::get<2>(element);
            bool Valid = con->validate();
            std::string valid_str = Valid ? " True " : " False ";
            std::string type_str = type == LeakageType::CFLeakage ? "CFleakage" : "DALeakage";

            if (!Valid) {
                std::cout << "Addr: " << std::hex << addr << std::dec << valid_str;
                std::cout << " Constrain: " << *con;
                std::cout << " Type: " << type_str << std::endl;
            }
        }
    }

    // Reduce constrains that don't have symbols
    void QIFSEEngine::reduceConstrains() {
        auto con = constrains.begin();
        while (con != constrains.end()) {
            std::tuple<uint32_t, std::shared_ptr<tana::Constrain>, LeakageType> con_tuple = *con;
            std::shared_ptr<Constrain> constrain = std::get<1>(con_tuple);
            if (constrain->getNumSymbols() == 0) {
                if(!constrain->validate())
                {
                    ERROR("Invalid constrain");
                }
                con = constrains.erase(con);

            } else
                ++con;

        }

    }

    void QIFSEEngine::printMemory() {
        for (auto const &x : memory) {
            std::cout << std::hex << x.first << std::dec  // string (key)
                      << ':'
                      << *(x.second) // string's value
                      << std::endl;
        }

    }

    float QIFSEEngine::getEntropy(std::vector<uint8_t> key_value,  \
                                  uint64_t MonteCarloTimes) {
        using clock = std::chrono::system_clock;
        using ms = std::chrono::milliseconds;
        const auto before = clock::now();

        FastMonteCarlo res(MonteCarloTimes, constrains, key_value);
        res.verifyConstrain();
        res.calculateConstrains();
        res.run();
        res.run_addr_group();
        res.print_group_result();
        float MonteCarloResult = res.getResult();

        const auto duration = std::chrono::duration_cast<ms>(clock::now() - before);

        std::cout << "It took " << duration.count() / 1000.0 << " ms"
                  << " to finish the monte carlo sampling" << std::endl;
        return abs(-log(MonteCarloResult) / log(2));
    }

    void QIFSEEngine::checkMemoryAccess(tana::Inst_Base *inst) {
        int oprd_num = 0, memory_num = 0, memory_index = 0;
        if (inst->oprd[0] != nullptr) ++oprd_num;
        if (inst->oprd[1] != nullptr) ++oprd_num;
        if (inst->oprd[2] != nullptr) ++oprd_num;

        if (oprd_num == 0)
            return;

        if (oprd_num == 1) {
            if (inst->oprd[0]->type == Operand::Mem) {
                memory_index = 0;
                ++memory_num;
            }
        }

        if (oprd_num == 2) {
            if (inst->oprd[0]->type == Operand::Mem) {
                memory_index = 0;
                ++memory_num;
            }
            if (inst->oprd[1]->type == Operand::Mem) {
                memory_index = 1;
                ++memory_num;
            }

        }

        if (oprd_num == 3) {
            if (inst->oprd[0]->type == Operand::Mem) {
                memory_index = 0;
                ++memory_num;
            }
            if (inst->oprd[1]->type == Operand::Mem) {
                memory_index = 1;
                ++memory_num;
            }
            if (inst->oprd[2]->type == Operand::Mem) {
                memory_index = 2;
                ++memory_num;
            }
        }

        if (memory_num == 0)
            return;

        checkOperand(inst->oprd[memory_index], inst);

    }

    void QIFSEEngine::checkOperand(const std::shared_ptr<tana::Operand> &opr, Inst_Base *inst) {
        assert(opr->type == Operand::Mem);

        //std::cout << "Inst: " << *inst << std::endl;
        //std::cout << "MemOperand: " << *opr << std::endl;
        switch (opr->tag) {
            case 1: {
                // Immediate Value
                return;
            }
            case 2: {
                // 32 bit register value
                auto reg = opr->field[0];
                auto regV = this->readReg(reg);
                auto regV_num = regV->symbol_num();

                if (regV_num == 0) {
                    return;
                }
                this->getMemoryAccessConstrain(regV, inst->get_memory_address());

                return;
            }
            case 3: {
                // eax*2
                auto reg = opr->field[0];
                auto regV = this->readReg(reg);
                auto regV_num = regV->symbol_num();

                if (regV_num == 0) {
                    return;
                }

                uint32_t temp_concrete = stoul(opr->field[1], nullptr, 16);
                auto res = buildop2(BVOper::bvimul, regV, temp_concrete);
                this->getMemoryAccessConstrain(res, inst->get_memory_address());

                return;
            }
            case 4: {
                // eax+0xffffff
                auto reg = opr->field[0];
                auto regV = this->readReg(reg);
                auto regV_num = regV->symbol_num();

                if (regV_num == 0) {
                    return;
                }

                uint32_t temp_concrete = stoul(opr->field[2], nullptr, 16);
                auto symbol = opr->field[1];
                auto bvopr = symbol == "+" ? BVOper::bvadd : BVOper::bvsub;
                auto res = buildop2(bvopr, regV, temp_concrete);
                this->getMemoryAccessConstrain(res, inst->get_memory_address());

                return;
            }
            case 5: {
                // eax+ebx*2
                auto reg1 = opr->field[0];
                auto reg2 = opr->field[1];
                auto regV1 = this->readReg(reg1);
                auto regV2 = this->readReg(reg2);
                auto regV1_num = regV1->symbol_num();
                auto regV2_num = regV2->symbol_num();

                if ((regV1_num + regV2_num) == 0) {
                    return;
                }

                uint32_t temp_concrete = stoul(opr->field[2], nullptr, 16);
                auto res1 = buildop2(BVOper::bvimul, regV2, temp_concrete);
                auto res = buildop2(BVOper::bvadd, regV1, res1);
                this->getMemoryAccessConstrain(res, inst->get_memory_address());

                return;
            }
            case 6: {
                // eax*2+0xffffff
                auto reg = opr->field[0];
                auto regV = this->readReg(reg);
                auto regV_num = regV->symbol_num();

                if (regV_num == 0) {
                    return;
                }

                uint32_t temp_concrete2 = stoul(opr->field[3], nullptr, 16);
                uint32_t temp_concrete1 = stoul(opr->field[1], nullptr, 16);
                auto symbol = opr->field[2];

                auto res1 = buildop2(BVOper::bvimul, regV, temp_concrete1);
                auto bvopr = symbol == "+" ? BVOper::bvadd : BVOper::bvsub;
                auto res = buildop2(bvopr, regV, temp_concrete2);
                this->getMemoryAccessConstrain(res, inst->get_memory_address());

                return;
            }
            case 7: {
                // eax+ebx*2+0xffffff
                auto reg1 = opr->field[0];
                auto reg2 = opr->field[1];
                auto regV1 = this->readReg(reg1);
                auto regV2 = this->readReg(reg2);
                auto regV1_num = regV1->symbol_num();
                auto regV2_num = regV2->symbol_num();

                if ((regV1_num + regV2_num) == 0) {
                    return;
                }

                uint32_t mul_value = stoul(opr->field[2], nullptr, 16);
                uint32_t imm_value = stoul(opr->field[4], nullptr, 16);
                auto symbol = opr->field[3];

                auto res1 = buildop2(BVOper::bvimul, regV2, mul_value);
                auto res2 = buildop2(BVOper::bvadd, regV1, res1);
                auto res = buildop2(BVOper::bvadd, res2, imm_value);
                this->getMemoryAccessConstrain(res, inst->get_memory_address());
                return;
            }
            default:
                ERROR("Invalid memory address mode");
        }

        return;

    }


    std::shared_ptr<tana::Constrain> QIFSEEngine::getMemoryAccessConstrain(
            std::shared_ptr<tana::BitVector> mem_address_symbol, std::string mem_address_str) {
        uint32_t L = 6;  // Here we use the model from the CacheD paper

        uint32_t mem_address_concrete = std::stoul(mem_address_str, nullptr, 16);
        uint32_t mem_address_concrete_L = mem_address_concrete >> L;

        std::shared_ptr<BitVector> mem_address_symbol_L = buildop2(BVOper::bvshr, mem_address_symbol, L);

        auto cons = std::make_shared<Constrain>(mem_address_symbol_L, BVOper::equal, mem_address_concrete_L);
        this->updateDAConstrains(cons);
        return cons;
    }


}