/*************************************************************************
	> File Name: Constrains.cpp
	> Author: Qinkun Bao
	> Mail: qinkunbao@gmail.com
	> Created Time: Mon Apr 22 21:56:56 2019
 ************************************************************************/

#include<iostream>
#include "Constrains.h"
#include "Engine.h"
#include "error.h"

#define ERROR(MESSAGE) tana::default_error_handler(__FILE__, __LINE__, MESSAGE)


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

    uint32_t Constrain::getNumSymbols()
    {
        return r->symbol_num();
    }

    bool Constrain::validate()
    {
        if(r->symbol_num() > 0)
        {
            return false;
        }

        auto &bv = (r)->opr;
        std::shared_ptr<BitVector> v_left = bv->val[0];
        auto relation = bv->opty;
        std::shared_ptr<BitVector> v_right = bv->val[1];

        uint32_t v_left_con = SEEngine::eval(v_left);
        uint32_t v_right_con = SEEngine::eval(v_right);

        switch (relation){
            case BVOper ::greater:
                return v_left_con > v_right_con;
            case BVOper ::less:
                return v_left_con < v_right_con;
            case BVOper ::equal:
                return v_left_con == v_right_con;
            case BVOper ::noequal:
                return v_left_con != v_right_con;
            case BVOper ::bvand:
                return v_left_con && v_right_con;
            case BVOper ::bvor:
                return v_left_con || v_right_con;
            default:
                ERROR("Invalid BVOper");
                return false;

        }



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

