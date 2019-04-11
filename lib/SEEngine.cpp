#include <cassert>
#include <algorithm>
#include <limits.h>
#include <sstream>
#include "ins_types.h"
#include "SEEngine.h"
#include "VarMap.h"
#include "error.h"

#define ERROR(MESSAGE) tana::default_error_handler(__FILE__, __LINE__, MESSAGE)


namespace tana {

// part of code is from https://github.com/s3team/CryptoHunt

    bool SEEngine::isImmSym(uint32_t num) {
        if (!imm2sym) {
            return false;
        }
        if (num < MAX_IMM_NUMBER) {
            return false;
        }
        std::vector<uint32_t> common = {0xff, 0xfff, 0xffff, 0xfffff, 0xffffff, 0xffffffff, 0xff00, 0xffff0000};
        auto find_result = std::find(common.begin(), common.end(), num);
        if (find_result != common.end()) {
            return false;
        }

        return true;

    }

    bool SEEngine::memory_find(uint32_t addr) {
        auto ii = memory.find(addr);
        if (ii == memory.end())
            return false;
        else
            return true;
    }

    SEEngine::SEEngine(bool state_type) {
        ctx = {{"eax", nullptr},
               {"ebx", nullptr},
               {"ecx", nullptr},
               {"edx", nullptr},
               {"esi", nullptr},
               {"edi", nullptr},
               {"esp", nullptr},
               {"ebp", nullptr}
        };
        imm2sym = state_type;
    }

    void SEEngine::getFormulaLength(const std::shared_ptr<BitVector> &v, uint32_t &len) {
        const std::unique_ptr<Operation> &op = v->opr;
        ++len;
        if (op == nullptr) {
            return;
        }

        if (op->val[0] != nullptr)
            getFormulaLength(op->val[0], len);
        if (op->val[1] != nullptr)
            getFormulaLength(op->val[1], len);
        if (op->val[2] != nullptr)
            getFormulaLength(op->val[2], len);
    }

    std::shared_ptr<BitVector> SEEngine::readReg(const std::string reg)
    {
        x86::x86_reg reg_id = x86::reg_string2id(reg);
        return readReg(reg_id);
    }

    std::shared_ptr<BitVector> SEEngine::readReg(const x86::x86_reg reg)
    {
        RegType type = Registers::getRegType(reg);
        if(type == FULL)
        {
            auto index = Registers::getRegIndex(reg);
            std::string strName = Registers::convertRegID2RegName(index);
            std::shared_ptr<BitVector> res = ctx[strName];
            return res;
        }

        if(type == HALF)
        {
            auto index = Registers::getRegIndex(reg);
            std::string strName = Registers::convertRegID2RegName(index);
            std::shared_ptr<BitVector> origin = ctx[strName];
            std::shared_ptr<BitVector> res = SEEngine::Extract(origin, 1, 16);
            return res;
        }

        if(type == QLOW)
        {
            auto index = Registers::getRegIndex(reg);
            std::string strName = Registers::convertRegID2RegName(index);
            std::shared_ptr<BitVector> origin = ctx[strName];
            std::shared_ptr<BitVector> res = SEEngine::Extract(origin, 1, 8);
            return res;
        }

        if(type == QHIGH)
        {
            auto index = Registers::getRegIndex(reg);
            std::string strName = Registers::convertRegID2RegName(index);
            std::shared_ptr<BitVector> origin = ctx[strName];
            std::shared_ptr<BitVector> res = SEEngine::Extract(origin, 9, 16);
            return res;
        }

        return nullptr;

    }

    bool SEEngine::writeReg(const x86::x86_reg reg, std::shared_ptr<tana::BitVector> v)
    {
        RegType type = Registers::getRegType(reg);
        uint32_t reg_index = Registers::getRegIndex(reg);
        std::string index_name = Registers::convertRegID2RegName(reg_index);
        if(type == FULL)
        {
            ctx[index_name] = v;
            return true;
        }
        if(type == HALF)
        {
            auto origin = ctx[index_name];
            auto reg_part = Extract(origin, 17, 32);
            assert(v->size() == (REGISTER_SIZE/2));
            auto v_reg = Concat(reg_part, v);
            assert(v_reg->size() == REGISTER_SIZE);
            ctx[index_name] = v_reg;
            return true;
        }
        if(type == QLOW)
        {
            auto origin = ctx[index_name];
            auto reg_part = Extract(origin, 9, 32);
            assert(v->size() == (REGISTER_SIZE/4));
            auto v_reg = Concat(reg_part, v);
            assert(v_reg->size() == REGISTER_SIZE);
            ctx[index_name] = v_reg;
            return true;
        }

        if(type == QHIGH)
        {
            auto origin = ctx[index_name];
            auto reg_part1 = Extract(origin, 1, 8);
            auto reg_part2 = Extract(origin, 17, 32);
            assert(v->size() == (REGISTER_SIZE/4));
            auto v_reg = Concat(reg_part2, v, reg_part1);
            assert(v_reg->size() == REGISTER_SIZE);
            ctx[index_name] = v_reg;
            return true;
        }
        ERROR("Unkown reg type");
        return false;

    }

    bool SEEngine::writeReg(const std::string reg, std::shared_ptr<tana::BitVector> v)
    {
        x86::x86_reg reg_id = x86::reg_string2id(reg);
        return writeReg(reg_id, v);
    }


    std::shared_ptr<BitVector> SEEngine::readMem(t_type::T_ADDRESS memory_address, t_type::T_SIZE size)
    {
        std::shared_ptr<BitVector> v0;
        if (memory_find(memory_address)) {
            v0 = memory[memory_address];
        } else {
            std::stringstream ss;
            ss << "Mem:" << std::hex << memory_address << std::dec;
            v0 = std::make_shared<BitVector>(SYMBOL, ss.str());
            memory[memory_address] = v0;
        }
        if (size == T_BYTE_SIZE * T_DWORD)
        {
            return v0;
        }

        if (size == T_BYTE_SIZE * T_WORD)
        {
            std::shared_ptr<BitVector> v1 = SEEngine::Extract(v0, 1, 16);
            return v1;
        }

        std::shared_ptr<BitVector> v1 = SEEngine::Extract(v0, 1, 8);
        return v1;
    }


    bool SEEngine::writeMem(tana::t_type::T_ADDRESS memory_address, tana::t_type::T_SIZE addr_size,
                            std::shared_ptr<tana::BitVector> v)
    {
        assert(v->size() == addr_size || !v->isSymbol());
        std::shared_ptr<BitVector> v0,  v_mem;

        if(addr_size == T_BYTE_SIZE * T_DWORD)
        {
            memory[memory_address] = v;
            return true;
        }
        // Create a symbol for the mem address if not found
        if (memory_find(memory_address)) {
            v0 = memory[memory_address];
        } else {
            std::stringstream ss;
            ss << "Mem:" << std::hex << memory_address << std::dec;
            v0 = std::make_shared<BitVector>(SYMBOL, ss.str());
            memory[memory_address] = v0;
        }

        if(addr_size == T_BYTE_SIZE * T_WORD)
        {
            std::shared_ptr<BitVector> v1 = SEEngine::Extract(v0, 17, 32);
            if(!v->isSymbol())
            {
                v = SEEngine::Extract(v, 1, 16);
            }
            v_mem = Concat(v1, v);
            memory[memory_address] = v_mem;
            return true;
        }

        if(addr_size == T_BYTE_SIZE * T_BYTE)
        {
            std::shared_ptr<BitVector> v1 = SEEngine::Extract(v0, 9, 32);
            if(!v->isSymbol())
            {
                v = SEEngine::Extract(v, 1, 8);
            }
            v_mem = Concat(v1, v);
            memory[memory_address] = v_mem;
            return true;
        }
        return false;
    }


    std::shared_ptr<BitVector>
    SEEngine::Concat(std::shared_ptr<BitVector> v1, std::shared_ptr<BitVector> v2) {
        std::shared_ptr<BitVector> low = nullptr, high = nullptr, res = nullptr;
        uint32_t size_res = v1->size() + v2->size();
        high = v1;
        low = v2;

        std::unique_ptr<Operation> oper = std::make_unique<Operation>("bvconcat", v1, v2);

        if (v1->isSymbol() || v2->isSymbol())
            res = std::make_shared<BitVector>(SYMBOL, std::move(oper));
        else {
            uint32_t v1_value = eval(v1);
            v1_value = BitVector::extract(v1_value, v1->high_bit, v1->low_bit);
            uint32_t v2_value = eval(v2);
            v2_value = BitVector::extract(v2_value, v2->high_bit, v2->low_bit);
            uint32_t result_value = BitVector::concat(v1_value, v2_value, v1->size(), v2->size());
            res = std::make_shared<BitVector>(CONCRETE, result_value);
        }

        res->low_bit = 1;
        res->high_bit = size_res;
        return res;
    }

    std::shared_ptr<BitVector>
    SEEngine::Concat(std::shared_ptr<BitVector> v1, std::shared_ptr<BitVector> v2, std::shared_ptr<BitVector> v3) {
        return Concat(Concat(v1, v2), v3);
    }

    std::shared_ptr<BitVector>
    SEEngine::Extract(std::shared_ptr<BitVector> v, int low, int high) {
        assert(high > low);

        std::unique_ptr<Operation> oper = std::make_unique<Operation>("bvextract", v);
        std::shared_ptr<BitVector> res = nullptr;
        if(v->isSymbol())
        {
            res = std::make_shared<BitVector>(SYMBOL, std::move(oper));
        }
        else
        {
            uint32_t result = eval(v);
            result = BitVector::extract(result, high, low);
            res = std::make_shared<BitVector>(CONCRETE, result);
        }
        res->high_bit = high;
        res->low_bit = low;
        return res;
    }

    std::shared_ptr<BitVector>
    SEEngine::ZeroExt(std::shared_ptr<tana::BitVector> v, tana::t_type::T_SIZE size_new)
    {
        assert(size_new >= v->size());
        std::unique_ptr<Operation> oper = std::make_unique<Operation>("bvzeroext", v);
        std::shared_ptr<BitVector> res = nullptr;
        if(v->isSymbol())
        {
            res = std::make_shared<BitVector>(SYMBOL, std::move(oper));
        }
        else
        {
            uint32_t result = eval(v);
            result = BitVector::extract(result, size_new, 1);
            res = std::make_shared<BitVector>(CONCRETE, result);
        }
        res->high_bit = size_new;
        res->low_bit = 1;
        return res;
    }

    std::shared_ptr<BitVector>
    SEEngine::SignExt(std::shared_ptr<tana::BitVector> v, tana::t_type::T_SIZE orgin_size,
                      tana::t_type::T_SIZE new_size){

        assert(orgin_size < new_size);
        std::unique_ptr<Operation> oper = std::make_unique<Operation>("bvsignext", v);
        std::shared_ptr<BitVector> res = nullptr;
        if(v->isSymbol())
        {
            res = std::make_shared<BitVector>(SYMBOL, std::move(oper));
        }
        else
        {
            uint32_t result = eval(v);
            result = BitVector::signext(result, orgin_size, new_size);
            res = std::make_shared<BitVector>(CONCRETE, result);
        }
        res->high_bit = new_size;
        res->low_bit = 1;
        return res;



    }

    void
    SEEngine::init(std::shared_ptr<BitVector> v1, std::shared_ptr<BitVector> v2,
                   std::shared_ptr<BitVector> v3, std::shared_ptr<BitVector> v4,
                   std::shared_ptr<BitVector> v5, std::shared_ptr<BitVector> v6,
                   std::shared_ptr<BitVector> v7, std::shared_ptr<BitVector> v8,
                   std::vector<std::unique_ptr<Inst_Dyn>>::iterator it1,
                   std::vector<std::unique_ptr<Inst_Dyn>>::iterator it2) {
        ctx["eax"] = v1;
        ctx["ebx"] = v2;
        ctx["ecx"] = v3;
        ctx["edx"] = v4;
        ctx["esi"] = v5;
        ctx["edi"] = v6;
        ctx["esp"] = v7;
        ctx["ebp"] = v8;

        this->start = it1;
        this->end = it2;
    }

    void
    SEEngine::init(std::vector<std::unique_ptr<Inst_Dyn>>::iterator it1,
                   std::vector<std::unique_ptr<Inst_Dyn>>::iterator it2) {
        this->start = it1;
        this->end = it2;
    }

    void
    SEEngine::initAllRegSymol(std::vector<std::unique_ptr<Inst_Dyn>>::iterator it1,
                              std::vector<std::unique_ptr<Inst_Dyn>>::iterator it2) {
        ctx["eax"] = std::make_shared<BitVector>(SYMBOL, "eax");
        ctx["ebx"] = std::make_shared<BitVector>(SYMBOL, "ebx");
        ctx["ecx"] = std::make_shared<BitVector>(SYMBOL, "ecx");
        ctx["edx"] = std::make_shared<BitVector>(SYMBOL, "edx");
        ctx["esi"] = std::make_shared<BitVector>(SYMBOL, "esi");
        ctx["edi"] = std::make_shared<BitVector>(SYMBOL, "edi");
        ctx["esp"] = std::make_shared<BitVector>(SYMBOL, "esp");
        ctx["ebp"] = std::make_shared<BitVector>(SYMBOL, "ebp");

        this->start = it1;
        this->end = it2;
    }


    int
    SEEngine::run(){
        for (auto inst = start; inst != end; ++inst)
        {
            auto it = inst->get();

            bool status = it->symbolic_execution(*this);

            if (!status)
            {
                ERROR("No recognized instruction");
                return false;
            }

        }
        return true;
    }


    uint32_t
    SEEngine::conexec(std::shared_ptr<BitVector> f, std::map<std::shared_ptr<BitVector>, uint32_t> *input) {
        std::set<std::shared_ptr<BitVector>> inmapkeys;
        std::set<std::shared_ptr<BitVector>> inputsym = varmap::getInputs(f);
        for (auto it = input->begin(); it != input->end(); ++it) {
            inmapkeys.insert(it->first);
        }

        if (inmapkeys != inputsym) {
            ERROR("Some inputs don't have parameters!");
            return 1;
        }

        return eval(f, input);
    }

    bool isTree(std::shared_ptr<BitVector> v) {

        std::list<std::shared_ptr<BitVector>> list_que;
        list_que.push_back(v);
        uint32_t count = 0;
        while (!list_que.empty()) {
            std::shared_ptr<BitVector> v = list_que.front();
            list_que.pop_front();
            ++count;
            const std::unique_ptr<Operation> &op = v->opr;
            if (op != nullptr) {
                if (op->val[0] != nullptr) list_que.push_back(op->val[0]);
                if (op->val[1] != nullptr) list_que.push_back(op->val[1]);
                if (op->val[2] != nullptr) list_que.push_back(op->val[2]);
            }
            if ((list_que.size() > FORMULA_MAX_LENGTH) || (count > FORMULA_MAX_LENGTH))
                return false;
        }
        return true;
    }


    std::vector<std::shared_ptr<BitVector>>
    SEEngine::getAllOutput() {
        std::vector<std::shared_ptr<BitVector>> outputs;
        std::shared_ptr<BitVector> v;

        // symbols in registers
        v = ctx["eax"];
        if ((v->opr != nullptr) && (isTree(v)))
            outputs.push_back(v);
        v = ctx["ebx"];
        if ((v->opr != nullptr) && (isTree(v)))
            outputs.push_back(v);
        v = ctx["ecx"];
        if ((v->opr != nullptr) && (isTree(v)))
            outputs.push_back(v);
        v = ctx["edx"];
        if ((v->opr != nullptr) && (isTree(v)))
            outputs.push_back(v);

        // symbols in memory
        for (auto const &x : memory) {

            v = x.second;
            if (v == nullptr) {
                continue;
            }
            if ((v->opr != nullptr) && (isTree(v)))
                outputs.push_back(v);
        }
        return outputs;
    }


    std::vector<std::shared_ptr<BitVector> > SEEngine::reduceValues(std::vector<std::shared_ptr<BitVector>> values) {
        auto values_size = values.size();
        std::vector<bool> redundancy_table(values_size, false);
        uint32_t values_size_after_reduction = 0;
        for (uint32_t i = 0; i < values_size; ++i) {
            for (uint32_t j = 0; j < values_size; ++j) {
                if (i == j) {
                    continue;
                }
                if (*values[i] == *values[j]) {
                    //std::cout << "i = " << i << "j = " << j << std::endl;
                    if (redundancy_table[i] == false) {
                        redundancy_table[j] = true;
                    }
                }
            }
        }


        for (uint32_t i = 0; i < values_size; ++i) {
            if (redundancy_table[i] == false) {
                ++values_size_after_reduction;
            }
        }

        std::vector<std::shared_ptr<BitVector> > values_reduced(values_size_after_reduction);
        uint32_t index = 0;
        for (uint32_t i = 0; i < values_size; ++i) {
            if (redundancy_table[i] == false) {
                values_reduced[index] = values[i];
                ++index;
            }
        }

        // Remove formulas that are either too long or too short
        assert(index == values_size_after_reduction);


        auto iter = values_reduced.begin();

        while (iter != values_reduced.end()) {
            uint32_t formula_size = 0;
            SEEngine::getFormulaLength(*iter, formula_size);
            if ((formula_size < FORMULA_MIN_LENGTH) || (formula_size > FORMULA_MAX_LENGTH)) {
                iter = values_reduced.erase(iter);
            } else {
                ++iter;
            }
        }

        return values_reduced;
    }


    uint32_t
    SEEngine::eval(const std::shared_ptr<BitVector> &v) { //for no input
        std::map<std::shared_ptr<BitVector>, uint32_t> varm;
        assert((varmap::getInputVector(v)).empty());
        return eval(v, &varm);
    }

    uint32_t
    SEEngine::eval(const std::shared_ptr<BitVector> &v, std::map<std::shared_ptr<BitVector>, uint32_t> *inmap) {
        const std::unique_ptr<Operation> &op = v->opr;
        if (op == nullptr) {
            if (v->val_type == CONCRETE)
                return v->concrete_value;
            else
                return (*inmap)[v];
        } else {
            uint32_t op0 = 0, op1 = 0;
            uint32_t op2 = 0;

            if (op->val[0] != nullptr) op0 = eval(op->val[0], inmap);
            if (op->val[1] != nullptr) op1 = eval(op->val[1], inmap);
            if (op->val[2] != nullptr) op2 = eval(op->val[2], inmap);

            if(op->opty == "bvzeroext"){
                return BitVector::zeroext(op0);
            } else if (op->opty == "bvextract") {
                return BitVector::extract(op0, v->high_bit, v->low_bit);
            } else if (op->opty == "bvconcat") {
                return BitVector::concat(op0,op1,op->val[0]->size(),op->val[1]->size());
            }  else if (op->opty == "bvadd") {
                return op0 + op1;
            } else if (op->opty == "bvadc") {
                return op0 + op1 + 1;
            } else if (op->opty == "bvsub") {
                return op0 - op1;
            } else if (op->opty == "bvimul") {
                return op0 * op1;
            } else if (op->opty == "bvshld") {
                return BitVector::shld32(op0, op1, op2);
            } else if (op->opty == "bvshrd") {
                return BitVector::shrd32(op0, op1, op2);
            } else if (op->opty == "bvxor") {
                return op0 ^ op1;
            } else if (op->opty == "bvand") {
                return op0 & op1;
            } else if (op->opty == "bvor") {
                return op0 | op1;
            } else if (op->opty == "bvshl") {
                return op0 << op1;
            } else if (op->opty == "bvshr") {
                return op0 >> op1;
            } else if (op->opty == "bvsar") {
                return BitVector::arithmeticRightShift(op0, op1);
            } else if (op->opty == "bvneg") {
                return ~op0 + 1;
            } else if (op->opty == "bvnot") {
                return ~op0;
            } else if (op->opty == "bvinc") {
                return op0 + 1;
            } else if (op->opty == "bvdec") {
                return op0 - 1;
            } else if (op->opty == "bvrol") {
                return BitVector::rol32(op0, op1);
            } else if (op->opty == "bvror") {
                return BitVector::ror32(op0, op1);
            } else if (op->opty == "bvquo"){
                return op0 / op1;
            } else if (op->opty == "bvrem"){
                return op0 % op1;
            } else {
                std::cout << "Instruction: [" << op->opty << "] is not interpreted!" << std::endl;
                if ((op->val[0] != nullptr) && ((op->val[1] != nullptr))) return op0 + op1;
                if (op->val[0] != nullptr) return op0;
                return 1;
            }
        }
    }

    std::shared_ptr<BitVector> SEEngine::formula_simplfy(std::shared_ptr<tana::BitVector> v)
    {
        const std::unique_ptr<Operation> &op = v->opr;
        if (op == nullptr)
        {
            return v;
        }
        uint32_t input_num = v->symbol_num();
        if(input_num == 0)
        {
            uint32_t res = eval(v);
            auto res_v = std::make_shared<BitVector>(CONCRETE, res);
            return res_v;
        }

        if (op->val[0] != nullptr) op->val[0] = formula_simplfy(op->val[0]);
        if (op->val[1] != nullptr) op->val[1] = formula_simplfy(op->val[1]);
        if (op->val[2] != nullptr) op->val[2] = formula_simplfy(op->val[2]);

        return v;

    }


    bool inst_dyn_details::two_operand(SEEngine &se, Inst_Dyn *inst)
    {
        std::shared_ptr<Operand> op0 = inst->oprd[0];
        std::shared_ptr<Operand> op1 = inst->oprd[1];
        std::shared_ptr<BitVector> v1, v0, res;
        auto opcode_id = inst->instruction_id;
        auto opcstr = "bv" + x86::insn_id2string(opcode_id);

        if (op1->type == Operand::ImmValue) {
            uint32_t temp_concrete = stoul(op1->field[0], nullptr, 16);
            v1 = std::make_shared<BitVector>(CONCRETE, temp_concrete, se.isImmSym(temp_concrete));
        } else if (op1->type == Operand::Reg) {
            v1 = se.readReg(op1->field[0]);
        } else if (op1->type == Operand::Mem) {
            v1 = se.readMem(inst->memory_address, op1->bit);
        } else {
            ERROR("other instructions: op1 is not ImmValue, Reg, or Mem!");
            return false;
        }

        if (op0->type == Operand::Reg) { // dest op is reg
            v0 = se.readReg(op0->field[0]);
            res = buildop2(opcstr, v0, v1);
            se.writeReg(op0->field[0], res);
            return true;
        } else if (op0->type == Operand::Mem) { // dest op is mem
            v0 = se.readMem(inst->memory_address, op0->bit);
            res = buildop2(opcstr, v0, v1);
            se.writeMem(inst->memory_address, op0->bit ,res);
            return true;
        } else {
            ERROR("other instructions: op2 is not ImmValue, Reg, or Mem!");
            return false;
        }
    }


    bool Dyn_X86_INS_PUSH::symbolic_execution(SEEngine &se)
    {
        std::shared_ptr<Operand> op0 = this->oprd[0];
        std::shared_ptr<BitVector> v0;

        if (op0->type == Operand::ImmValue) {
            uint32_t temp_concrete = stoul(op0->field[0], nullptr, 16);
            v0 = std::make_shared<BitVector>(CONCRETE, temp_concrete, se.isImmSym(temp_concrete));
            se.writeMem(this->memory_address, op0->bit, v0);
            return true;
        }

        if (op0->type == Operand::Reg) {
            auto regV = se.readReg(op0->field[0]);
            se.writeMem(this->memory_address, op0->bit,regV);
            return true;
        }
        if (op0->type == Operand::Mem) {
            // The memaddr in the trace is the read address
            // We need to compute the write address
            auto reg = Registers::convert2RegID("esp");
            uint32_t esp_index = Registers::getRegIndex(reg);
            uint32_t esp_value = this->vcpu.gpr[esp_index];
            v0 = se.readMem(this->memory_address, op0->bit);
            se.writeMem(esp_value - 4, op0->bit, v0);
            return true;
        }

        ERROR("push error: the operand is not Imm, Reg or Mem!");
        return false;
    }

    bool Dyn_X86_INS_POP::symbolic_execution(tana::SEEngine &se)
    {
        std::shared_ptr<Operand> op0 = this->oprd[0];

        if (op0->type == Operand::Reg) {
            assert(Registers::getRegType(op0->field[0]) == FULL);
            auto v0 = se.readMem(this->memory_address, op0->bit);
            se.writeReg(op0->field[0], v0);
            return true;
        }

        ERROR("pop error: the operand is not Reg!");
        return false;

    }

    bool Dyn_X86_INS_NEG::symbolic_execution(tana::SEEngine &se)
    {
        std::shared_ptr<Operand> op0 = this->oprd[0];
        auto opcode_id = this->instruction_id;
        auto opcstr = "bv" + x86::insn_id2string(opcode_id);


        if (op0->type == Operand::Reg) {
            assert(Registers::getRegType(op0->field[0]) == FULL);
            auto v0 = se.readReg(op0->field[0]);
            auto res = buildop1(opcstr, v0);
            se.writeReg(op0->field[0], res);
            return true;
        }
        ERROR("neg error: the operand is not Reg!");
        return false;
    }

    bool Dyn_X86_INS_NOT::symbolic_execution(tana::SEEngine &se)
    {
        std::shared_ptr<Operand> op0 = this->oprd[0];
        auto opcode_id = this->instruction_id;
        auto opcstr = "bv" + x86::insn_id2string(opcode_id);

        if (op0->type == Operand::Reg) {
            assert(Registers::getRegType(op0->field[0]) == FULL);
            auto v0 = se.readReg(op0->field[0]);
            auto res = buildop1(opcstr, v0);
            se.writeReg(op0->field[0], res);
            return true;
        }
        ERROR("neg error: the operand is not Reg!");
        return false;
    }

    bool Dyn_X86_INS_INC::symbolic_execution(tana::SEEngine &se)
    {
        std::shared_ptr<Operand> op0 = this->oprd[0];
        auto opcode_id = this->instruction_id;
        auto opcstr = "bv" + x86::insn_id2string(opcode_id);
        std::shared_ptr<BitVector> v0, res;

        if (op0->type == Operand::Reg)
        {
            v0 = se.readReg(op0->field[0]);
        }

        if (op0->type == Operand::Mem) {
            v0 = se.readMem(this->memory_address, op0->bit);
        }
        res = buildop1(opcstr, v0);

        if (op0->type == Operand::Reg) {
            se.writeReg(op0->field[0], res);
        }

        if (op0->type == Operand::Mem) {
            //memory[it->memory_address] = res;
            se.writeMem(this->memory_address, op0->bit, res);
        }
        return true;

    }

    bool Dyn_X86_INS_DEC::symbolic_execution(tana::SEEngine &se)
    {
        std::shared_ptr<Operand> op0 = this->oprd[0];
        auto opcode_id = this->instruction_id;
        auto opcstr = "bv" + x86::insn_id2string(opcode_id);
        std::shared_ptr<BitVector> v0, res;

        if (op0->type == Operand::Reg) {
            v0 = se.readReg(op0->field[0]);
        }

        if (op0->type == Operand::Mem) {
            v0 = se.readMem(this->memory_address, op0->bit);
        }
        res = buildop1(opcstr, v0);

        if (op0->type == Operand::Reg)
        {
            se.writeReg(op0->field[0], res);
        }

        if (op0->type == Operand::Mem)
        {
            se.writeMem(this->memory_address, op0->bit, res);
        }
        return true;

    }


    bool Dyn_X86_INS_MOVSX::symbolic_execution(tana::SEEngine &se)
    {
        std::shared_ptr<Operand> op0 = this->oprd[0];
        std::shared_ptr<Operand> op1 = this->oprd[1];
        std::shared_ptr<BitVector> v0, v1, res;

        if (op1->type == Operand::Reg) {
            auto reg = Registers::convert2RegID(op1->field[0]);
            v1 = se.readReg(reg);

            v1 = se.SignExt(v1, op1->bit, op0->bit);
            se.writeReg(op0->field[0], v1);
            return true;
        }
        if (op1->type == Operand::Mem) {
            v1 = se.readMem(this->memory_address, op1->bit);
            v1 = se.SignExt(v1, op1->bit, op0->bit);
            se.writeReg(op0->field[0], v1);
            return true;
        }
        return false;
    }

    bool Dyn_X86_INS_MOVZX::symbolic_execution(tana::SEEngine &se)
    {
        std::shared_ptr<Operand> op0 = this->oprd[0];
        std::shared_ptr<Operand> op1 = this->oprd[1];
        std::shared_ptr<BitVector> v0, v1, res;

        if (op1->type == Operand::Reg) {
            auto reg = Registers::convert2RegID(op1->field[0]);
            v1 = se.readReg(reg);
            v1 = se.ZeroExt(v1, op0->bit);
            se.writeReg(op0->field[0], v1);
            return true;

        }
        if (op1->type == Operand::Mem) {
            //TODO
            v1 = se.readMem(this->memory_address, op1->bit);
            v1 = se.ZeroExt(v1, op0->bit);
            se.writeReg(op0->field[0], v1);
            return true;
        }

        ERROR("MOVZX");
        return false;
    }

    bool Dyn_X86_INS_CMOVB::symbolic_execution(tana::SEEngine &se)
    {
        std::shared_ptr<Operand> op0 = this->oprd[0];
        std::shared_ptr<Operand> op1 = this->oprd[1];
        std::shared_ptr<BitVector> v0, v1, res;
        auto opcode_id = this->instruction_id;

        if(!(this->vcpu.eflags_state))
            ERROR("CMOVB doesn't have eflags information");

        auto CF = this->vcpu.CF();
        if(!CF)
            return true;

        if (op0->type == Operand::Reg)
        {
            if (op1->type == Operand::ImmValue) { // mov reg, 0x1111
                uint32_t temp_concrete = stoul(op1->field[0], 0, 16);
                v1 = std::make_shared<BitVector>(CONCRETE, temp_concrete, se.isImmSym(temp_concrete));
                se.writeReg(op0->field[0], v1);
                return true;
            }
            if (op1->type == Operand::Reg) { // mov reg, reg
                v1 = se.readReg(op1->field[0]);
                se.writeReg(op0->field[0], v1);
                return true;
            }
            if (op1->type == Operand::Mem) { // mov reg, dword ptr [ebp+0x1]
                v1 = se.readMem(this->memory_address, op1->bit);
                se.writeReg(op0->field[0], v1);
                return true;
            }

            ERROR("op1 is not ImmValue, Reg or Mem");
            return false;
        }
        if (op0->type == Operand::Mem) {
            if (op1->type == Operand::ImmValue) { // mov dword ptr [ebp+0x1], 0x1111
                uint32_t temp_concrete = stoul(op1->field[0], 0, 16);
                se.writeMem(this->memory_address, op0->bit, std::make_shared<BitVector>(CONCRETE, temp_concrete, se.isImmSym(temp_concrete)));
                return true;
            } else if (op1->type == Operand::Reg) { // mov dword ptr [ebp+0x1], reg
                v1 = se.readReg(op1->field[0]);
                se.writeMem(this->memory_address, op1->bit, v1);
                return true;
            }
        }
        ERROR("Error: The first operand in MOV is not Reg or Mem!");
        return false;

    }

    bool Dyn_X86_INS_MOV::symbolic_execution(tana::SEEngine &se)
    {
        std::shared_ptr<Operand> op0 = this->oprd[0];
        std::shared_ptr<Operand> op1 = this->oprd[1];
        std::shared_ptr<BitVector> v0, v1, res;
        auto opcode_id = this->instruction_id;

        if (op0->type == Operand::Reg)
        {
            if (op1->type == Operand::ImmValue) { // mov reg, 0x1111
                uint32_t temp_concrete = stoul(op1->field[0], 0, 16);
                v1 = std::make_shared<BitVector>(CONCRETE, temp_concrete, se.isImmSym(temp_concrete));
                se.writeReg(op0->field[0], v1);
                return true;
            }
            if (op1->type == Operand::Reg) { // mov reg, reg
                v1 = se.readReg(op1->field[0]);
                se.writeReg(op0->field[0], v1);
                return true;
            }
            if (op1->type == Operand::Mem) { // mov reg, dword ptr [ebp+0x1]
                /* 1. Get mem address
                2. check whether the mem address has been accessed
                3. if not, create a new value
                4. else load the value in that memory
                */
                v1 = se.readMem(this->memory_address, op1->bit);
                se.writeReg(op0->field[0], v1);
                return true;
            }

            ERROR("op1 is not ImmValue, Reg or Mem");
            return false;
        }
        if (op0->type == Operand::Mem) {
            if (op1->type == Operand::ImmValue) { // mov dword ptr [ebp+0x1], 0x1111
                uint32_t temp_concrete = stoul(op1->field[0], 0, 16);
                se.writeMem(this->memory_address, op0->bit, std::make_shared<BitVector>(CONCRETE, temp_concrete, se.isImmSym(temp_concrete)));
                return true;
            } else if (op1->type == Operand::Reg) { // mov dword ptr [ebp+0x1], reg
                //memory[it->memory_address] = ctx[getRegName(op1->field[0])];
                v1 = se.readReg(op1->field[0]);
                se.writeMem(this->memory_address, op1->bit, v1);
                return true;
            }
        }
        ERROR("Error: The first operand in MOV is not Reg or Mem!");
        return false;
    }

    bool Dyn_X86_INS_LEA::symbolic_execution(tana::SEEngine &se)
    {
        std::shared_ptr<Operand> op0 = this->oprd[0];
        std::shared_ptr<Operand> op1 = this->oprd[1];
        std::shared_ptr<BitVector> v0, v1, res;
        /* lea reg, ptr [edx+eax*1]
           interpret lea instruction based on different address type
           1. op0 must be reg
           2. op1 must be addr
        */
        if (op0->type != Operand::Reg || op1->type != Operand::Mem) {
            ERROR("lea format error!");
            return false;
        }
        switch (op1->tag) {
            case 5: {
                std::shared_ptr<BitVector> f0, f1, f2; // corresponding field[0-2] in operand
                f0 = se.readReg(op1->field[0]);
                f1 = se.readReg(op1->field[1]);
                if (op1->field[2] == "1") {
                    res = buildop2("bvadd", f0, f1);
                    //ctx[getRegName(op0->field[0])] = res;
                    se.writeReg(op0->field[0], res);
                    return true;
                }
                uint32_t temp_concrete = stoul(op1->field[2], 0, 16);
                f2 = std::make_shared<BitVector>(CONCRETE, temp_concrete, se.isImmSym(temp_concrete));
                res = buildop2("bvimul", f1, f2);
                res = buildop2("bvadd", f0, res);
                se.writeReg(op0->field[0], res);
                return true;
            }
            case 7: {
                std::shared_ptr<BitVector> f0, f1, f2, f3; // addr7: eax+ebx*2+0xfffff1
                //f0 = ctx[getRegName(op1->field[0])];       //eax
                //f1 = ctx[getRegName(op1->field[1])];       //ebx

                f0 = se.readReg(op1->field[0]);
                f1 = se.readReg(op1->field[1]);

                uint32_t temp_concrete1 = stoul(op1->field[2], 0, 16);
                f2 = std::make_shared<BitVector>(CONCRETE, temp_concrete1, se.isImmSym(temp_concrete1));   //2
                std::string sign = op1->field[3];          //+
                uint32_t temp_concrete2 = stoul(op1->field[4], 0, 16);
                f3 = std::make_shared<BitVector>(CONCRETE, temp_concrete2, se.isImmSym(temp_concrete2));   //0xfffff1
                assert((sign == "+") || (sign == "-"));
                if (op1->field[2] == "1") {
                    res = buildop2("bvadd", f0, f1);
                } else {
                    res = buildop2("bvimul", f1, f2);
                    res = buildop2("bvadd", f0, res);
                }
                if (sign == "+")
                    res = buildop2("bvadd", res, f3);
                else
                    res = buildop2("bvsub", res, f3);
                //ctx[getRegName(op0->field[0])] = res;

                se.writeReg(op0->field[0], res);

                return true;
            }
            case 4: {
                std::shared_ptr<BitVector> f0, f1; // addr4: eax+0xfffff1
                //f0 = ctx[getRegName(op1->field[0])];       //eax
                f0 = se.readReg(op1->field[0]);
                uint32_t temp_concrete = stoul(op1->field[2], 0, 16);
                f1 = std::make_shared<BitVector>(CONCRETE, temp_concrete, se.isImmSym(temp_concrete));   //0xfffff1
                std::string sign = op1->field[1];          //+
                if (sign == "+")
                    res = buildop2("bvadd", f0, f1);
                else
                    res = buildop2("bvsub", f0, f1);
                //ctx[getRegName(op0->field[0])] = res;

                se.writeReg(op0->field[0], res);
                return true;
            }
            case 6: {
                std::shared_ptr<BitVector> f0, f1, f2; // addr6: eax*2+0xfffff1
                //f0 = ctx[getRegName(op1->field[0])];
                f0 = se.readReg(op1->field[0]);
                uint32_t temp_concrete1 = stoul(op1->field[1]);
                uint32_t temp_concrete2 = stoul(op1->field[3]);

                f1 = std::make_shared<BitVector>(CONCRETE, temp_concrete1, se.isImmSym(temp_concrete1));
                f2 = std::make_shared<BitVector>(CONCRETE, temp_concrete2, se.isImmSym(temp_concrete2));
                std::string sign = op1->field[2];
                if (op1->field[1] == "1") {
                    res = f0;
                } else {
                    res = buildop2("bvimul", f0, f1);
                }
                if (sign == "+")
                    res = buildop2("bvadd", res, f2);
                else
                    res = buildop2("bvsub", res, f2);
                se.writeReg(op0->field[0], res);
                return true;
            }
            case 3: {
                std::shared_ptr<BitVector> f0, f1;          // addr3: eax*2
                //f0 = ctx[getRegName(op1->field[0])];
                f0 = se.readReg(op1->field[0]);

                uint32_t temp_concrete = stoul(op1->field[1]);
                f1 = std::make_shared<BitVector>(CONCRETE, temp_concrete, se.isImmSym(temp_concrete));
                res = buildop2("bvimul", f0, f1);
                //ctx[getRegName(op0->field[0])] = res;
                se.writeReg(op0->field[0], res);
                return true;
            }

            case 2: {
                v1 = se.readReg(op1->field[0]);
                se.writeReg(op0->field[0], v1);
                return true;
            }

            case 1: {
                std::shared_ptr<BitVector> f0;
                uint32_t temp_concrete = stoul(op1->field[0], 0, 16);
                f0 = std::make_shared<BitVector>(CONCRETE, temp_concrete, se.isImmSym(temp_concrete));
                se.writeReg(op0->field[0], f0);
                return true;
            }
            default:
                ERROR("Other tags in addr is not ready for lea!");
                return false;
        }

    }


    bool Dyn_X86_INS_XCHG::symbolic_execution(tana::SEEngine &se)
    {
        std::shared_ptr<Operand> op0 = this->oprd[0];
        std::shared_ptr<Operand> op1 = this->oprd[1];
        std::shared_ptr<BitVector> v0, v1, res;
        auto opcode_id = this->instruction_id;

        if (op1->type == Operand::Reg)
        {
            v1 = se.readReg(op1->field[0]);
            if (op0->type == Operand::Reg)
            {
                v0 = se.readReg(op0->field[0]);
                v1 = se.readReg(op1->field[0]);

                se.writeReg(op1->field[0], v0);
                se.writeReg(op0->field[0], v1);

                return true;
            }
            if (op0->type == Operand::Mem)
            {
                v0 = se.readMem(this->memory_address, op0->bit);
                v1 = se.readReg(op1->field[0]);

                //ctx[getRegName(op1->field[0])] = v0; // xchg mem, reg
                //memory[it->memory_address] = v1;
                se.writeReg(op1->field[0], v0);
                se.writeMem(this->memory_address, op0->bit, v1);

                return true;
            }
            ERROR("xchg error: 1");
            return false;
        }
        if (op1->type == Operand::Mem) {
            v1 = se.readMem(this->memory_address, op1->bit);
            if (op0->type == Operand::Reg) {
                v0 = se.readReg(op0->field[0]);
                se.writeReg(op0->field[0], v1); // xchg reg, mem
                se.writeMem(this->memory_address, op1->bit, v0);
            }
            ERROR("xchg error 3");
            return false;

        }
        ERROR("xchg error: 2");
        return false;
    }

    bool Dyn_X86_INS_SBB::symbolic_execution(tana::SEEngine &se)
    {
        std::shared_ptr<Operand> op0 = this->oprd[0];
        std::shared_ptr<Operand> op1 = this->oprd[1];

        if (op1->type == Operand::Reg) {

        }
        if (op1->type == Operand::Mem) {

        }
        return true;
    }


    bool Dyn_X86_INS_IMUL::symbolic_execution(tana::SEEngine &se)
    {
        std::shared_ptr<Operand> op0 = this->oprd[0];
        std::shared_ptr<Operand> op1 = this->oprd[1];
        std::shared_ptr<Operand> op2 = this->oprd[2];
        std::shared_ptr<BitVector> v1, v2, v3, res;
        auto opcode_id = this->instruction_id;
        auto opcstr = "bv" + x86::insn_id2string(opcode_id);
        if(this->get_operand_number() == 1)
        {
            //TODO
            return true;
        }


        if (op0->type == Operand::Reg &&
            op1->type == Operand::Reg &&
            op2->type == Operand::ImmValue) { // imul reg, reg, imm
            v1 = se.readReg(op1->field[0]);
            uint32_t temp_concrete = stoul(op2->field[0], 0, 16);
            v2 = std::make_shared<BitVector>(CONCRETE, temp_concrete, se.isImmSym(temp_concrete));
            res = buildop2(opcstr, v1, v2);
            se.writeReg(op0->field[0], res);
            return true;
        }

        ERROR("three operands instructions other than imul are not handled!");
        return false;

    }


    bool Dyn_X86_INS_SHLD::symbolic_execution(tana::SEEngine &se)
    {
        std::shared_ptr<Operand> op0 = this->oprd[0];
        std::shared_ptr<Operand> op1 = this->oprd[1];
        std::shared_ptr<Operand> op2 = this->oprd[2];
        std::shared_ptr<BitVector> v1, v2, v3, res;
        auto opcode_id = this->instruction_id;
        auto opcstr = "bv" + x86::insn_id2string(opcode_id);

        if (op0->type == Operand::Reg &&
            op1->type == Operand::Reg &&
            op2->type == Operand::ImmValue) {                                           // shld shrd reg, reg, imm
            v1 = se.readReg(op0->field[0]);
            v2 = se.readReg(op1->field[0]);
            uint32_t temp_concrete = stoul(op2->field[0], 0, 16);
            v3 = std::make_shared<BitVector>(CONCRETE, temp_concrete, se.isImmSym(temp_concrete));
            res = buildop3(opcstr, v1, v2, v3);
            se.writeReg(op0->field[0], res);
            return true;
        }

        ERROR("shld is not handled!");
        return false;

    }

    bool Dyn_X86_INS_SHRD::symbolic_execution(tana::SEEngine &se)
    {
        std::shared_ptr<Operand> op0 = this->oprd[0];
        std::shared_ptr<Operand> op1 = this->oprd[1];
        std::shared_ptr<Operand> op2 = this->oprd[2];
        std::shared_ptr<BitVector> v1, v2, v3, res;
        auto opcode_id = this->instruction_id;
        auto opcstr = "bv" + x86::insn_id2string(opcode_id);

        if (op0->type == Operand::Reg &&
            op1->type == Operand::Reg &&
            op2->type == Operand::ImmValue) {                                           // shld shrd reg, reg, imm
            v1 = se.readReg(op0->field[0]);
            v2 = se.readReg(op1->field[0]);
            uint32_t temp_concrete = stoul(op2->field[0], 0, 16);
            v3 = std::make_shared<BitVector>(CONCRETE, temp_concrete, se.isImmSym(temp_concrete));
            res = buildop3(opcstr, v1, v2, v3);
            se.writeReg(op0->field[0], res);
            return true;
        }

        ERROR("shrd is not handled!");
        return false;
    }

    bool Dyn_X86_INS_ADD::symbolic_execution(tana::SEEngine &se)
    {
        return inst_dyn_details::two_operand(se, this);
    }

    bool Dyn_X86_INS_SUB::symbolic_execution(tana::SEEngine &se)
    {
        return inst_dyn_details::two_operand(se, this);
    }

    bool Dyn_X86_INS_AND::symbolic_execution(tana::SEEngine &se)
    {
        return inst_dyn_details::two_operand(se, this);
    }

    bool Dyn_X86_INS_ADC::symbolic_execution(tana::SEEngine &se)
    {
        if(!(this->vcpu.eflags_state))
            return inst_dyn_details::two_operand(se, this);

        bool CF = this->vcpu.CF();

        std::shared_ptr<Operand> op0 = this->oprd[0];
        std::shared_ptr<Operand> op1 = this->oprd[1];
        std::shared_ptr<BitVector> v1, v0, res;
        auto opcode_id = this->instruction_id;
        auto opcstr = x86::insn_id2string(opcode_id);

        std::shared_ptr<BitVector> v_one =  std::make_shared<BitVector>(CONCRETE, 1, se.isImmSym(1));

        if (op1->type == Operand::ImmValue) {
            uint32_t temp_concrete = stoul(op1->field[0], 0, 16);
            v1 = std::make_shared<BitVector>(CONCRETE, temp_concrete, se.isImmSym(temp_concrete));
        } else if (op1->type == Operand::Reg) {
            v1 = se.readReg(op1->field[0]);
        } else if (op1->type == Operand::Mem) {
            v1 = se.readMem(this->memory_address, op1->bit);
        } else {
            ERROR("other instructions: op1 is not ImmValue, Reg, or Mem!");
            return false;
        }

        if (op0->type == Operand::Reg) { // dest op is reg
            v0 = se.readReg(op0->field[0]);
            res = buildop2("bvadd", v0, v1);
            if(CF)
            {
                res = buildop2("bvadd", res, v_one);
            }
            se.writeReg(op0->field[0], res);
            return true;
        } else if (op0->type == Operand::Mem) { // dest op is mem
            v0 = se.readMem(this->memory_address, op0->bit);
            res = buildop2("bvadd", v0, v1);
            if(CF)
            {
                res = buildop2("bvadd", res, v_one);
            }
            se.writeMem(this->memory_address, op0->bit, res);
            return true;
        } else {
            ERROR("other instructions: op2 is not ImmValue, Reg, or Mem!");
            return false;
        }


    }

    bool Dyn_X86_INS_ROR::symbolic_execution(tana::SEEngine &se)
    {
        return inst_dyn_details::two_operand(se, this);
    }

    bool Dyn_X86_INS_ROL::symbolic_execution(tana::SEEngine &se)
    {
        return inst_dyn_details::two_operand(se, this);
    }

    bool Dyn_X86_INS_OR::symbolic_execution(tana::SEEngine &se)
    {
        return inst_dyn_details::two_operand(se, this);
    }

    bool Dyn_X86_INS_XOR::symbolic_execution(tana::SEEngine &se)
    {
        return inst_dyn_details::two_operand(se, this);
    }

    bool Dyn_X86_INS_SHL::symbolic_execution(tana::SEEngine &se)
    {
        return inst_dyn_details::two_operand(se, this);
    }

    bool Dyn_X86_INS_SHR::symbolic_execution(tana::SEEngine &se)
    {
        return inst_dyn_details::two_operand(se, this);
    }

    bool Dyn_X86_INS_SAR::symbolic_execution(tana::SEEngine &se)
    {
        return inst_dyn_details::two_operand(se, this);
    }


    bool Dyn_X86_INS_CALL::symbolic_execution(tana::SEEngine &se)
    {
        return true;
    }

    bool Dyn_X86_INS_RET::symbolic_execution(tana::SEEngine &se)
    {
        return true;
    }

    bool Dyn_X86_INS_LEAVE::symbolic_execution(tana::SEEngine &se)
    {

        // EBP = ESP
        auto v_ebp = se.readReg("ebp");
        se.writeReg("esp", v_ebp);

        // POP EBP
        auto v0 = se.readMem(this->memory_address, REGISTER_SIZE);
        se.writeReg("ebp", v0);

        return true;
    }

    bool Dyn_X86_INS_ENTER::symbolic_execution(tana::SEEngine &se)
    {
        return true;
    }

    bool Dyn_X86_INS_DIV::symbolic_execution(tana::SEEngine &se)
    {
        std::shared_ptr<Operand> op0 = this->oprd[0];
        auto operand_size = op0->bit;
        assert(operand_size == 8 || operand_size == 16 || operand_size == 32);
        std::shared_ptr<BitVector> dividend, divisor, quotient, remainder;
        if(operand_size == 8)
        {
            dividend = se.readReg("ax");
        }
        if(operand_size == 16)
        {
            auto temp1 = se.readReg("ax");
            auto temp2 = se.readReg("dx");
            dividend = se.Concat(temp2, temp1);
        }
        if(operand_size == 32)
        {
            auto temp1 = se.readReg("eax");
            auto temp2 = se.readReg("edx");
            dividend = se.Concat(temp2, temp1);
        }


        if(op0->type == Operand::Mem)
        {
           divisor = se.readMem(memory_address, op0->bit);
        }

        if(op0->type == Operand::Reg)
        {
           divisor = se.readReg(op0->field[0]);
        }

        quotient = buildop2("bvquo", dividend, divisor);
        remainder = buildop2("bvrem", dividend, divisor);

        if(operand_size == 8)
        {
            se.writeReg("al", quotient);
            se.writeReg("ah", remainder);
        }

        if(operand_size == 16)
        {
            se.writeReg("ax", quotient);
            se.writeReg("dx", remainder);
        }

        if(operand_size == 32)
        {
            se.writeReg("eax", quotient);
            se.writeReg("edx", remainder);
        }

        return true;

    }


}