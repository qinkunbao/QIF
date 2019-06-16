/*************************************************************************
	> File Name: MonteCarlo.h
	> Author: Qinkun Bao
	> Mail: qinkunbao@gmail.com
	> Created Time: Mon May 20 21:06:21 2019
 ************************************************************************/

#pragma once

#include <vector>
#include <map>
#include <memory>
#include <random>
#include "BitVector.h"
#include "Constrains.h"
#include "Engine.h"


namespace tana {
    namespace MonteCarlo {

        std::vector<uint8_t> getRandomVector(unsigned int size);

        std::map<int, uint32_t> input2val(const std::vector<uint8_t> &input,
                                          std::vector<int> &bv);

        std::vector<int>
        getAllKeys(const std::vector<std::tuple<uint32_t, std::shared_ptr<tana::Constrain>, LeakageType>> &constrains);

        bool
        constrainSatisify(const std::vector<std::tuple<uint32_t, std::shared_ptr<tana::Constrain>, LeakageType >> &constrains,
                          const std::map<int, uint32_t> &input_map);


        float
        calculateMonteCarlo(const std::vector<std::tuple<uint32_t, std::shared_ptr<tana::Constrain>, LeakageType>> &constrains,
                            uint64_t sample_num);


    }

    class FastMonteCarlo{
    private:
        std::vector<std::unique_ptr<std::pair<std::vector<uint8_t>, bool>>> tests;
        std::vector<std::tuple<uint32_t, std::shared_ptr<tana::Constrain>, LeakageType>> constrains;
        uint64_t num_sample;
        uint64_t num_satisfied;
        std::vector<int> input_vector;
        void testConstrain(const std::shared_ptr<tana::Constrain> &con);
        std::vector<uint8_t > getRandomVector(unsigned int size);
        std::random_device rd;
        std::uniform_int_distribution<uint8_t> dist;
        std::minstd_rand engine{rd()};

    public:
        FastMonteCarlo(uint64_t sample_num, std::vector<std::tuple<uint32_t, std::shared_ptr<tana::Constrain>, LeakageType>> constrains);
        void run();
        float getResult();

    };
}
