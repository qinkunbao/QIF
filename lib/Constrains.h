/*************************************************************************
	> File Name: Constrains.h
	> Author: Qinkun Bao
	> Mail: qinkunbao@gmail.com
	> Created Time: Mon Apr 22 21:56:51 2019
 ************************************************************************/

#pragma once

#include <tuple>
#include <memory>
#include <vector>
#include "BitVector.h"


namespace tana {


    class Constrain {
    private:
        std::shared_ptr<BitVector> r;
    public:
        Constrain() = default;
        Constrain(std::shared_ptr<BitVector> b, BVOper, uint32_t);
        Constrain(std::shared_ptr<BitVector> b1, BVOper, std::shared_ptr<BitVector> b2);

        void update(BVOper add_type, std::shared_ptr<BitVector> b, BVOper type, uint32_t num);
        friend std::ostream& operator<<(std::ostream& os, const Constrain& c);
    };

}