#include "Engine.h"
#include "error.h"
#include "VarMap.h"

#define ERROR(MESSAGE) tana::default_error_handler(__FILE__, __LINE__, MESSAGE)

namespace tana {

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


    SEEngine::SEEngine(bool state_type) {
        imm2sym = state_type;
        eflags = false;
    }

    int
    SEEngine::run() {
        for (auto inst = start; inst != end; ++inst) {
            auto it = inst->get();
            if (x86::SymbolicExecutionNoEffect(it->instruction_id))
                continue;
            bool status = it->symbolic_execution(this);

            if (!status) {
                ERROR("No recognized instruction");
                return false;
            }

        }
        return true;
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


    std::shared_ptr<BitVector>
    SEEngine::Concat(std::shared_ptr<BitVector> v1, std::shared_ptr<BitVector> v2) {
        std::shared_ptr<BitVector> low = nullptr, high = nullptr, res = nullptr;
        uint32_t size_res = v1->size() + v2->size();
        high = v1;
        low = v2;

        std::unique_ptr<Operation> oper = std::make_unique<Operation>("bvconcat", v1, v2);

        if (v1->isSymbol() || v2->isSymbol())
            res = std::make_shared<BitVector>(ValueType::SYMBOL, std::move(oper));
        else {
            uint32_t v1_value = eval(v1);
            v1_value = BitVector::extract(v1_value, v1->high_bit, v1->low_bit);
            uint32_t v2_value = eval(v2);
            v2_value = BitVector::extract(v2_value, v2->high_bit, v2->low_bit);
            uint32_t result_value = BitVector::concat(v1_value, v2_value, v1->size(), v2->size());
            res = std::make_shared<BitVector>(ValueType::SYMBOL, result_value);
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
        if (v->isSymbol()) {
            res = std::make_shared<BitVector>(ValueType::SYMBOL, std::move(oper));
        } else {
            uint32_t result = eval(v);
            result = BitVector::extract(result, high, low);
            res = std::make_shared<BitVector>(ValueType::SYMBOL, result);
        }
        res->high_bit = high;
        res->low_bit = low;
        return res;
    }

    std::shared_ptr<BitVector>
    SEEngine::ZeroExt(std::shared_ptr<tana::BitVector> v, tana::tana_type::T_SIZE size_new) {
        assert(size_new >= v->size());
        std::unique_ptr<Operation> oper = std::make_unique<Operation>("bvzeroext", v);
        std::shared_ptr<BitVector> res = nullptr;
        if (v->isSymbol()) {
            res = std::make_shared<BitVector>(ValueType::SYMBOL, std::move(oper));
        } else {
            uint32_t result = eval(v);
            result = BitVector::extract(result, size_new, 1);
            res = std::make_shared<BitVector>(ValueType::SYMBOL, result);
        }
        res->high_bit = size_new;
        res->low_bit = 1;
        return res;
    }

    std::shared_ptr<BitVector>
    SEEngine::SignExt(std::shared_ptr<tana::BitVector> v, tana::tana_type::T_SIZE orgin_size,
                      tana::tana_type::T_SIZE new_size) {
        assert(orgin_size < new_size);
        std::unique_ptr<Operation> oper = std::make_unique<Operation>("bvsignext", v);
        std::shared_ptr<BitVector> res = nullptr;
        if (v->isSymbol()) {
            res = std::make_shared<BitVector>(ValueType::SYMBOL, std::move(oper));
        } else {
            uint32_t result = eval(v);
            result = BitVector::signext(result, orgin_size, new_size);
            res = std::make_shared<BitVector>(ValueType::SYMBOL, result);
        }
        res->high_bit = new_size;
        res->low_bit = 1;
        return res;

    }


    void
    SEEngine::init(std::vector<std::unique_ptr<Inst_Base>>::iterator it1,
                   std::vector<std::unique_ptr<Inst_Base>>::iterator it2) {
        this->start = it1;
        this->end = it2;
    }

    void
    SEEngine::initAllRegSymol(std::vector<std::unique_ptr<Inst_Base>>::iterator it1,
                              std::vector<std::unique_ptr<Inst_Base>>::iterator it2) {

        this->start = it1;
        this->end = it2;
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


    std::vector<std::shared_ptr<BitVector> >
    SEEngine::reduceValues(std::vector<std::shared_ptr<BitVector>> values) {
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
            if (v->val_type == ValueType::SYMBOL)
                return v->concrete_value;
            else
                return (*inmap)[v];
        } else {
            uint32_t op0 = 0, op1 = 0;
            uint32_t op2 = 0;

            if (op->val[0] != nullptr) op0 = eval(op->val[0], inmap);
            if (op->val[1] != nullptr) op1 = eval(op->val[1], inmap);
            if (op->val[2] != nullptr) op2 = eval(op->val[2], inmap);

            if (op->opty == "bvzeroext")
                return BitVector::zeroext(op0);

            if (op->opty == "bvextract")
                return BitVector::extract(op0, v->high_bit, v->low_bit);

            if (op->opty == "bvconcat")
                return BitVector::concat(op0, op1, op->val[0]->size(), op->val[1]->size());

            if (op->opty == "bvadd")
                return op0 + op1;

            if (op->opty == "bvadc")
                return op0 + op1 + 1;

            if (op->opty == "bvsub")
                return op0 - op1;

            if (op->opty == "bvimul")
                return op0 * op1;

            if (op->opty == "bvshld")
                return BitVector::shld32(op0, op1, op2);

            if (op->opty == "bvshrd")
                return BitVector::shrd32(op0, op1, op2);

            if (op->opty == "bvxor")
                return op0 ^ op1;

            if (op->opty == "bvand")
                return op0 & op1;

            if (op->opty == "bvor")
                return op0 | op1;

            if (op->opty == "bvshl")
                return op0 << op1;

            if (op->opty == "bvshr")
                return op0 >> op1;

            if (op->opty == "bvsar")
                return BitVector::arithmeticRightShift(op0, op1);

            if (op->opty == "bvneg")
                return ~op0 + 1;

            if (op->opty == "bvnot")
                return ~op0;

            if (op->opty == "bvinc")
                return op0 + 1;

            if (op->opty == "bvdec")
                return op0 - 1;

            if (op->opty == "bvrol")
                return BitVector::rol32(op0, op1);

            if (op->opty == "bvror")
                return BitVector::ror32(op0, op1);

            if (op->opty == "bvquo")
                return op0 / op1;

            if (op->opty == "bvrem")
                return op0 % op1;

            if (op->opty == "equal")
                return op0 == op1;

            if (op->opty == "greater")
                return op0 > op1;

            if (op->opty == "less")
                return op0 < op1;

            std::cout << "Instruction: [" << op->opty << "] is not interpreted!" << std::endl;
            if ((op->val[0] != nullptr) && ((op->val[1] != nullptr))) return op0 + op1;
            if (op->val[0] != nullptr) return op0;
            return 1;

        }
    }

    std::shared_ptr<BitVector>
    SEEngine::formula_simplfy(std::shared_ptr<tana::BitVector> v) {
        const std::unique_ptr<Operation> &op = v->opr;
        if (op == nullptr) {
            return v;
        }
        uint32_t input_num = v->symbol_num();
        if (input_num == 0) {
            uint32_t res = eval(v);
            auto res_v = std::make_shared<BitVector>(ValueType::SYMBOL, res);
            return res_v;
        }

        if (op->val[0] != nullptr) op->val[0] = formula_simplfy(op->val[0]);
        if (op->val[1] != nullptr) op->val[1] = formula_simplfy(op->val[1]);
        if (op->val[2] != nullptr) op->val[2] = formula_simplfy(op->val[2]);

        return v;

    }


}