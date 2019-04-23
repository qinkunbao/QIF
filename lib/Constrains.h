/*************************************************************************
	> File Name: Constrains.h
	> Author: Qinkun Bao
	> Mail: qinkunbao@gmail.com
	> Created Time: Mon Apr 22 21:56:51 2019
 ************************************************************************/

#pragma once

#include <tuple>
#include <memory>
#include "BitVector.h"


namespace tana {

    class Constrain {
    public:
        enum RelationType {
            equal, greater, less, nonequal
        };
        std::tuple<std::shared_ptr<BitVector>, RelationType> r;
        Constrain()= delete;
        Constrain(std::shared_ptr<BitVector> b, RelationType, uint32_t);
        Constrain(std::shared_ptr<BitVector> b1, RelationType, std::shared_ptr<BitVector> b2);

        friend std::ostream& operator<<(std::ostream& os, const Constrain& c);
    };

}