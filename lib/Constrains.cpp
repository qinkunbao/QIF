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
        std::shared_ptr<BitVector> bit_num = std::make_shared<BitVector>(CONCRETE, num);
        std::shared_ptr<BitVector> res = buildop2("bvsub", bit, bit_num);
        r = std::make_tuple(res, relation);
    }

    Constrain::Constrain(std::shared_ptr<tana::BitVector> b1, tana::Constrain::RelationType relation,
                         std::shared_ptr<tana::BitVector> b2) {

        auto res = buildop2("bvsub", b1, b2);
        r = std::make_tuple(res, relation);
    }

    std::ostream &operator<<(std::ostream &os, const Constrain &dt) {
        std::shared_ptr<BitVector> b = std::get<0>(dt.r);
        Constrain::RelationType relation = std::get<1>(dt.r);
        os << *b;
        if (relation == Constrain::RelationType::equal)
            os << " = 0";
        if (relation == Constrain::RelationType::less)
            os << " < 0";
        if (relation == Constrain::RelationType::greater)
            os << " > 0";
        if (relation == Constrain::RelationType::nonequal)
            os << " != 0";

        return os;
    }


}

