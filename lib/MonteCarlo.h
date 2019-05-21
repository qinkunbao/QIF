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
#include "Engine.h"


namespace tana::MonteCarlo {

    std::vector<uint8_t > getRandomVector(unsigned int size);

    std::map<int, uint32_t > input2val(const std::vector<uint8_t> &input,
                                                             std::vector<int> &bv);

    std::vector<int>
    getAllKeys(const std::vector<std::tuple<uint32_t, std::shared_ptr<tana::Constrain>>> &constrains);

    bool
    constrainSatisify(const std::vector<std::tuple<uint32_t, std::shared_ptr<tana::Constrain>>> &constrains,
                                       const std::map<int, uint32_t > &input_map);


    float
    calculateMonteCarlo(const std::vector<std::tuple<uint32_t, std::shared_ptr<tana::Constrain>>> &constrains,
                        uint64_t sample_num);


}