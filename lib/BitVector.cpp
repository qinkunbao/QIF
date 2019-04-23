#include <sstream>
#include <bitset>
#include <string>
#include <climits>
#include <set>
#include "BitVector.h"





namespace tana {
    Operation::Operation(std::string opt, std::shared_ptr<BitVector> v1) {
        opty = opt;
        val[0] = v1;
        val[1] = nullptr;
        val[2] = nullptr;
    }

    Operation::Operation(std::string opt, std::shared_ptr<BitVector> v1, std::shared_ptr<BitVector> v2) {
        opty = opt;
        val[0] = v1;
        val[1] = v2;
        val[2] = nullptr;
    }

    Operation::Operation(std::string opt, std::shared_ptr<BitVector> v1, std::shared_ptr<BitVector> v2,
                         std::shared_ptr<BitVector> v3) {
        opty = opt;
        val[0] = v1;
        val[1] = v2;
        val[2] = v3;
    }

    int BitVector::idseed = 0;

    BitVector::BitVector(ValueType vty, std::string s_info) : opr(nullptr), val_type(vty), info(s_info), concrete_value(0)
    {
        id = ++idseed;
    }

    BitVector::BitVector(ValueType vty, uint32_t con, bool Imm2SymState) : opr(nullptr), concrete_value(con)
    {
        id = ++idseed;
        val_type = Imm2SymState ? SYMBOL : vty;
    }

    BitVector::BitVector(ValueType vty, uint32_t con) : opr(nullptr), val_type(vty), concrete_value(con)
    {
        id = ++idseed;
    }

    BitVector::BitVector(ValueType vty, std::unique_ptr<Operation> oper) :concrete_value(0), val_type(vty)
    {
        id = ++idseed;
        opr = std::move(oper);
    }



    uint32_t BitVector::arithmeticRightShift(uint32_t op1, uint32_t op2)
    {
        int32_t temp1 = op1;
        int32_t temp2 = op2;
        int32_t result = temp1 >> temp2;
        return uint32_t(result);
    }

    uint32_t BitVector::rol32(uint32_t op1, uint32_t op2) {
        const unsigned int mask = (CHAR_BIT * sizeof(op1) - 1);  // assumes width is a power of 2.
        // assert ( (c<=mask) &&"rotate by type width or more");
        op2 &= mask;
        return (op1 << op2) | (op1 >> ((-op2) & mask));

    }

    uint32_t BitVector::ror32(uint32_t op1, uint32_t op2) {
        const unsigned int mask = (CHAR_BIT * sizeof(op1) - 1);

        // assert ( (c<=mask) &&"rotate by type width or more");
        op2 &= mask;
        return (op1 >> op2) | (op1 << ((-op2) & mask));

    }

    uint32_t BitVector::shld32(uint32_t op1, uint32_t op2, uint32_t op3) {
        uint32_t cnt;
        uint32_t res;
        assert(op3 < REGISTER_SIZE);
        cnt = op3 % REGISTER_SIZE;
        res = (op1 << cnt) | (op2 >> (REGISTER_SIZE - cnt));
        return res;
    }

    uint32_t BitVector::shrd32(uint32_t op1, uint32_t op2, uint32_t op3) {
        uint32_t cnt;
        uint32_t res;
        assert(op3 < REGISTER_SIZE);
        cnt = op3 % REGISTER_SIZE;
        res = (op1 >> cnt) | (op2 << (REGISTER_SIZE - cnt));
        return res;
    }

    uint32_t BitVector::extract(uint32_t op1, uint32_t high, uint32_t low)
    {
        assert(high > low);
        assert(high <= REGISTER_SIZE);
        assert(low > 0);
        std::bitset<REGISTER_SIZE> bset(op1);
        uint32_t tmp = op1 << (REGISTER_SIZE - high);
        uint32_t res = tmp >> (REGISTER_SIZE - high + low - 1);
        return res;

    }

    uint32_t BitVector::concat(uint32_t op1, uint32_t op2, uint32_t op1_size, uint32_t op2_size)
    {
        std::bitset<REGISTER_SIZE> bit1(op1);
        std::bitset<REGISTER_SIZE> bit2(op2);
        std::string res;

        std::string bit1_str = bit1.to_string();
        std::string bit2_str = bit2.to_string();

        auto bit1_str_size = bit1_str.size();
        auto bit2_str_size = bit2_str.size();

        std::string bit1_sub = bit1_str.substr(bit1_str_size - op1_size, bit1_str_size);
        std::string bit2_sub = bit2_str.substr(bit2_str_size - op2_size, bit2_str_size);

        res = bit1_sub + bit2_sub;

        std::bitset<REGISTER_SIZE> res_bit(res);
        auto res_u = static_cast<uint32_t > (res_bit.to_ulong());
        return res_u;


    }

    uint32_t BitVector::zeroext(uint32_t op1)
    {
        return op1;
    }

    uint32_t BitVector::signext(uint32_t op1, uint32_t origin_size, uint32_t new_size)
    {
        std::bitset<REGISTER_SIZE> bit1(op1);
        std::string res;
        std::string bit1_str = bit1.to_string();
        auto bit1_str_size = bit1_str.size();
        auto sign_flag = bit1_str[bit1_str_size - origin_size];
        std::string str_ext(new_size - origin_size, sign_flag);
        res = str_ext + bit1_str.substr(bit1_str_size - origin_size, bit1_str_size);
        std::bitset<REGISTER_SIZE> res_bit(res);
        auto res_u = static_cast<uint32_t > (res_bit.to_ulong());
        return res_u;

    }

    bool BitVector::isSymbol() {
        return (val_type == SYMBOL) ? true : false;

    }

    std::string BitVector::print() const
    {
        std::stringstream ss;
        if(val_type == SYMBOL)
        {
            if(!info.empty()) {
                ss << "(SYM_ID" << id;
                ss << ", " << info << ")";
            } else{
                ss << "SYM_ID" << id;
            }
        }
        if(val_type == CONCRETE)
        {
            ss << "0x" << std::hex << concrete_value <<  std::dec;
        }
        return ss.str();
    }

    uint32_t BitVector::size()
    {
        return high_bit - low_bit + 1;
    }


    bool BitVector::operator==(const BitVector &v1) {
        const std::unique_ptr<Operation> &opr1 = opr;
        const std::unique_ptr<Operation> &opr2 = v1.opr;
        if ((opr == nullptr) && (v1.opr == nullptr)) {
            if (val_type != v1.val_type)
                return false;
            if (val_type == SYMBOL)
                return (id == v1.id);
            if (val_type == CONCRETE)
                return (concrete_value) == (v1.concrete_value);
        }
        if ((opr1 != nullptr) && (opr2 != nullptr)) {
            std::shared_ptr<BitVector> l1 = nullptr, l2 = nullptr, l3 = nullptr, r1 = nullptr, r2 = nullptr, r3 = nullptr;
            uint32_t num_l = 0;
            uint32_t num_r = 0;
            if (opr1->opty != opr2->opty)
                return false;
            if (opr1->val[0] != nullptr) {
                ++num_l;
                l1 = opr1->val[0];
            }
            if (opr1->val[1] != nullptr) {
                ++num_l;
                l2 = opr1->val[1];
            }
            if (opr1->val[2] != nullptr) {
                ++num_l;
                l3 = opr1->val[2];
            }
            if (opr2->val[0] != nullptr) {
                ++num_r;
                r1 = opr2->val[0];
            }
            if (opr2->val[1] != nullptr) {
                ++num_r;
                r2 = opr2->val[1];
            }
            if (opr2->val[2] != nullptr) {
                ++num_r;
                r3 = opr2->val[2];
            }
            if (num_l != num_r)
                return false;
            if (num_l == 1)
                return (*l1) == (*r1);
            if (num_l == 2)
                return ((*l1) == (*r1)) && ((*l2) == (*r2));
            if (num_l == 3)
                return ((*l1) == (*r1)) && ((*l2) == (*r2)) && ((*l3) == (*r3));
        }
        return false;
    }

    uint32_t BitVector::printV(std::stringstream &ss) const {
        uint32_t num = 0;
        printV(ss, num);
        return num;
    }

    std::ostream& operator<<(std::ostream& os, const BitVector& dt)
    {
        std::stringstream ss;
        dt.printV(ss);
        os << ss.str();
        return os;

    }


    void BitVector::printV(std::stringstream &ss, uint32_t &length) const{
        const std::unique_ptr<Operation> &op = this->opr;
        ++length;
        if (op == nullptr) {
            ss << this->print();
            return;
        }
        int num_opr = 0;
        if (op->val[0] != nullptr) ++num_opr;
        if (op->val[1] != nullptr) ++num_opr;
        if (op->val[2] != nullptr) ++num_opr;


        if (num_opr == 1) {
            ss << op->opty;
            ss << "(";
            (op->val[0])->printV(ss, length);
            ss << ")";

        }
        if (num_opr == 2) {
            ss << op->opty;
            ss << "(";
            (op->val[0])->printV(ss, length);
            ss << ",";
            (op->val[1])->printV(ss, length);
            ss << ")";
        }
        if (num_opr == 3) {
            ss << op->opty;
            ss << "(";
            (op->val[0])->printV(ss, length);
            ss << ",";
            (op->val[1])->printV(ss, length);
            ss << ",";
            (op->val[2])->printV(ss, length);
            ss << ")";
        }
    }

    void BitVector::symbol_num_internal(const std::shared_ptr<BitVector> &v, std::set<int> &input) const
    {
        const std::unique_ptr<Operation> &op = v->opr;
        if (op == nullptr) {
            if (v->val_type == CONCRETE)
                return;
            else
            {
                input.insert(v->id);
                return;
            }
        }

        if (op->val[0] != nullptr) symbol_num_internal(op->val[0], input);
        if (op->val[1] != nullptr) symbol_num_internal(op->val[1], input);
        if (op->val[2] != nullptr) symbol_num_internal(op->val[2], input);

    }

    uint32_t BitVector::symbol_num() const
    {
        std::set<int> input_num;
        const std::unique_ptr<Operation> &op = this->opr;

        if (op == nullptr) {
            if (this->val_type == SYMBOL)
                input_num.insert(this->id);
            return input_num.size();
        }
        if (op->val[0] != nullptr) symbol_num_internal(op->val[0], input_num);
        if (op->val[1] != nullptr) symbol_num_internal(op->val[1], input_num);
        if (op->val[2] != nullptr) symbol_num_internal(op->val[2], input_num);
        return input_num.size();
    }


    std::shared_ptr<BitVector>
    buildop1(std::string opty, std::shared_ptr<BitVector> v1) {
        std::unique_ptr<Operation> oper = std::make_unique<Operation>(opty, v1);
        std::shared_ptr<BitVector> result;
        if (v1->isSymbol())
            result = std::make_shared<BitVector>(SYMBOL, std::move(oper));
        else {
            result = std::make_shared<BitVector>(CONCRETE, std::move(oper));
        }
        result->high_bit = v1->high_bit;
        result->low_bit = v1->low_bit;
        return result;
    }

    std::shared_ptr<BitVector>
    buildop2(std::string opty, std::shared_ptr<BitVector> v1, std::shared_ptr<BitVector> v2) {
        std::unique_ptr<Operation> oper = std::make_unique<Operation>(opty, v1, v2);
        std::shared_ptr<BitVector> result;
        //assert(v1->size() == v2->size()|| !v2->isSymbol() || !v1->isSymbol());
        if (v1->isSymbol() || v2->isSymbol())
            result = std::make_shared<BitVector>(SYMBOL, std::move(oper));
        else
            result = std::make_shared<BitVector>(CONCRETE, std::move(oper));
        result->low_bit = v1->low_bit;
        result->high_bit = v1->high_bit;
        return result;
    }

    std::shared_ptr<BitVector>
    buildop3(std::string opty, std::shared_ptr<BitVector> v1, std::shared_ptr<BitVector> v2, std::shared_ptr<BitVector> v3) {
        std::unique_ptr<Operation> oper = std::make_unique<Operation>(opty, v1, v2, v3);
        std::shared_ptr<BitVector> result;

        if (v1->isSymbol() || v2->isSymbol() || v3->isSymbol())
            result = std::make_shared<BitVector>(SYMBOL, std::move(oper));
        else
            result = std::make_shared<BitVector>(CONCRETE, std::move(oper));

        return result;
    }

}