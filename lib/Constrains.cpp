/*************************************************************************
	> File Name: Constrains.cpp
	> Author: Qinkun Bao
	> Mail: qinkunbao@gmail.com
	> Created Time: Mon Apr 22 21:56:56 2019
 ************************************************************************/

#include<iostream>
#include "Constrains.h"

namespace tana {
    Constrain::Constrain(std::shared_ptr<BitVector> bit, RelationType relation, uint32_t num) {
        std::shared_ptr<BitVector> bit_num = std::make_shared<BitVector>(ValueType ::CONCRETE, num);
        std::shared_ptr<BitVector> res = buildop2("bvsub", bit, bit_num);
        r.push_back(std::make_tuple(res, relation));
    }

    void Constrain::update(std::shared_ptr<tana::BitVector> b, tana::RelationType type, uint32_t num)
    {
        std::shared_ptr<BitVector> bit_num = std::make_shared<BitVector>(ValueType ::CONCRETE, num);
        std::shared_ptr<BitVector> res = buildop2("bvsub", b, bit_num);
        r.push_back(std::make_tuple(res, type));
    }

    Constrain::Constrain(std::shared_ptr<tana::BitVector> b1, tana::RelationType relation,
                         std::shared_ptr<tana::BitVector> b2) {

        auto res = buildop2("bvsub", b1, b2);
        r.push_back(std::make_tuple(res, relation));
    }

    std::ostream &operator<<(std::ostream &os, const Constrain &dt) {
        for(auto &con : dt.r) {
            std::shared_ptr<BitVector> b = std::get<0>(con);
            RelationType relation = std::get<1>(con);
            os << *b;
            if (relation == RelationType::equal)
                os << " = 0";
            if (relation == RelationType::less)
                os << " < 0";
            if (relation == RelationType::greater)
                os << " > 0";
            if (relation == RelationType::nonequal)
                os << " != 0";

            os << "\n";
        }
        return os;
    }


}

