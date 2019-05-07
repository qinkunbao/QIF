/*************************************************************************
	> File Name: Constrains.cpp
	> Author: Qinkun Bao
	> Mail: qinkunbao@gmail.com
	> Created Time: Mon Apr 22 21:56:56 2019
 ************************************************************************/

#include<iostream>
#include "Constrains.h"

namespace tana {
    Constrain::Constrain(std::shared_ptr<BitVector> bit, BVOper relation, uint32_t num) {
        r = buildop2(relation, bit, num);
    }

    void Constrain::update(BVOper add_type, std::shared_ptr<tana::BitVector> b, BVOper type, uint32_t num)
    {
        auto constrain = buildop2(type, b, num);
        r = buildop2(add_type, r, constrain);
    }

    Constrain::Constrain(std::shared_ptr<tana::BitVector> b1, BVOper relation,
                         std::shared_ptr<tana::BitVector> b2) {

        r = buildop2(relation, b1, b2);

    }

    std::ostream &operator<<(std::ostream &os, const Constrain &dt) {

        auto &bv = (dt.r)->opr;
        auto con = bv->val[0];
        auto relation = bv->opty;
        auto num = bv->val[1];

        os << *con;
        switch (relation){
            case BVOper ::greater:
                os << " > ";
                break;
            case BVOper ::less:
                os << " < ";
                break;
            case BVOper ::equal:
                os << " == ";
                break;
            case BVOper ::noequal:
                os << " != ";
                break;
            case BVOper ::bvand:
                os << " && ";
                break;
            case BVOper ::bvor:
                os << " | ";
                break;
            default:
                os << "ERROR";
        }
        os << *num;


        os << "\n";
        return os;
    }


}

