/*************************************************************************
	> File Name: MonteCarlo.cpp
	> Author: 
	> Mail: 
	> Created Time: Mon May 20 21:06:14 2019
 ************************************************************************/
#include <random>
#include "MonteCarlo.h"


namespace tana {
    std::vector<uint8_t >
    MonteCarlo::getRandomVector(unsigned int size)
    {
        std::vector<uint8_t > randVector(size);
        for(uint32_t index = 0; index < size; ++index)
        {
            std::random_device random_device; // create object for seeding
            std::mt19937 engine{random_device()}; // create engine and seed it
            std::uniform_int_distribution<> dist(0,255); // create distribution for integers with [1; 9] range
            auto random_number = dist(engine); // finally get a pseudo-randomrandom integer number
            randVector[index] = random_number;
        }

        return randVector;

    }


    std::map<std::shared_ptr<BitVector>, uint32_t >
    MonteCarlo::input2val(const std::vector<uint8_t> &input, std::vector<std::shared_ptr<BitVector>> &bv)
    {
        std::map<std::shared_ptr<BitVector>, uint32_t> input_v;
        assert(input.size() == bv.size());
        auto input_size = bv.size();
        for(int i = 0; i < input_size; ++i )
        {
            input_v.insert(std::pair<std::shared_ptr<BitVector>, uint32_t >(bv[i], input[i]));
        }
        return input_v;
    }
}
