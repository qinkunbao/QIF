#include <sstream>
#include <bitset>
#include <string>
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

    BitVector::BitVector(ValueTy vty) : opr(nullptr) {
        id = ++idseed;
        valty = vty;
    }

    BitVector::BitVector(ValueTy vty, std::string con) : opr(nullptr) {
        id = ++idseed;
        valty = vty;
        conval = con;
    }

    BitVector::BitVector(ValueTy vty, uint32_t con, bool Imm2SymState) : opr(nullptr) {
        std::stringstream ss;
        if (Imm2SymState) {
            id = ++idseed;
            valty = SYMBOL;
            ss << "0x" << std::hex << con << std::dec;
            conval = ss.str();
        } else {
            id = ++idseed;
            valty = vty;
            ss << "0x" << std::hex << con << std::dec;
            conval = ss.str();
        }
    }

    BitVector::BitVector(ValueTy vty, uint32_t con) : opr(nullptr) {
        std::stringstream ss;
        id = ++idseed;
        valty = vty;
        ss << "0x" << std::hex << con << std::dec;
        conval = ss.str();

    }

    BitVector::BitVector(ValueTy vty, std::unique_ptr<Operation> oper)
    {
        id = ++idseed;
        valty = vty;
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

    uint32_t BitVector::concat(uint32_t op1, uint32_t op2)
    {
        std::bitset<REGISTER_SIZE> bit1(op1);
        std::bitset<REGISTER_SIZE> bit2(op2);
        std::string res;
        bool flag = false;
        for(int index = REGISTER_SIZE - 1; index >= 0; --index)
        {
            if(!bit1[index])
            {
                if(flag)
                {
                   res += "0";
                }
            }
            if(bit1[index])
            {
                flag = true;
                res += "1";
            }

        }
        flag = false;
        for(int index = REGISTER_SIZE - 1; index >= 0; --index)
        {
            if(!bit2[index])
            {
                if(flag)
                {
                    res += "0";
                }
            }
            if(bit2[index])
            {
                flag = true;
                res += "1";
            }

        }
        std::bitset<REGISTER_SIZE> res_bit(res);
        auto res_u = static_cast<uint32_t > (res_bit.to_ulong());
        return res_u;

    }

    bool BitVector::isSymbol() {
        if (valty == SYMBOL)
            return true;
        else
            return false;
    }

    std::string BitVector::print()
    {
        std::stringstream ss;
        if(valty == SYMBOL)
        {
            ss << "SYM_ID" << id;
        }
        if(valty == CONCRETE)
        {
            ss << "Con(" << conval << ")";
        }
        return ss.str();
    }


    bool BitVector::operator==(const BitVector &v1) {
        const std::unique_ptr<Operation> &opr1 = opr;
        const std::unique_ptr<Operation> &opr2 = v1.opr;
        if ((opr == nullptr) && (v1.opr == nullptr)) {
            if (valty != v1.valty)
                return false;
            if (valty == SYMBOL)
                return (id == v1.id);
            if (valty == CONCRETE)
                return (conval) == (v1.conval);
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


    std::shared_ptr<BitVector>
    buildop1(std::string opty, std::shared_ptr<BitVector> v1) {
        std::unique_ptr<Operation> oper = std::make_unique<Operation>(opty, v1);
        std::shared_ptr<BitVector> result;
        if (v1->isSymbol())
            result = std::make_shared<BitVector>(SYMBOL, std::move(oper));
        else {
            result = std::make_shared<BitVector>(CONCRETE, std::move(oper));
        }
        return result;
    }

    std::shared_ptr<BitVector>
    buildop2(std::string opty, std::shared_ptr<BitVector> v1, std::shared_ptr<BitVector> v2) {
        std::unique_ptr<Operation> oper = std::make_unique<Operation>(opty, v1, v2);
        std::shared_ptr<BitVector> result;

        if (v1->isSymbol() || v2->isSymbol())
            result = std::make_shared<BitVector>(SYMBOL, std::move(oper));
        else
            result = std::make_shared<BitVector>(CONCRETE, std::move(oper));

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