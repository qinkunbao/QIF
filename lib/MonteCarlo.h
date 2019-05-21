/*************************************************************************
	> File Name: MonteCarlo.h
	> Author: 
	> Mail: 
	> Created Time: Mon May 20 21:06:21 2019
 ************************************************************************/

#pragma once

#include <vector>
#include <map>
#include <memory>
#include "BitVector.h"
#include "Constrains.h"


namespace tana::MonteCarlo {

    std::vector<uint8_t > getRandomVector(unsigned int size);

    std::map<std::shared_ptr<BitVector>, uint32_t > input2val(const std::vector<uint8_t> &input,
                                                             std::vector<std::shared_ptr<BitVector>> &bv);

    std::vector<std::shared_ptr<BitVector>>
    getAllKeys(const std::vector<std::tuple<uint32_t, std::shared_ptr<tana::Constrain>>> &constrains);



}