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

    enum class RelationType {
        equal, greater, less, nonequal
    };

    class Constrain {
    private:
        std::vector<std::tuple<std::shared_ptr<BitVector>, RelationType>> r;
    public:
        Constrain() = default;
        Constrain(std::shared_ptr<BitVector> b, RelationType, uint32_t);
        Constrain(std::shared_ptr<BitVector> b1, RelationType, std::shared_ptr<BitVector> b2);

        void update(std::shared_ptr<BitVector> b, RelationType type, uint32_t num);
        friend std::ostream& operator<<(std::ostream& os, const Constrain& c);
    };

}