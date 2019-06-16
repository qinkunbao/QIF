/*************************************************************************
	> File Name: MonteCarlo.cpp
	> Author: Qinkun Bao
	> Mail: qinkunbao@gmail.com
	> Created Time: Mon May 20 21:06:14 2019
 ************************************************************************/
#include <random>
#include <algorithm>
#include "MonteCarlo.h"


namespace tana {
    std::vector<uint8_t>
    MonteCarlo::getRandomVector(unsigned int size) {
        std::vector<uint8_t> randVector(size);
        for (uint32_t index = 0; index < size; ++index) {
            std::random_device random_device; // create object for seeding
            std::minstd_rand engine{random_device()}; // create engine and seed it
            std::uniform_int_distribution<uint8_t > dist(0, 255); // create distribution for integers with [1; 9] range
            auto random_number = dist(engine); // finally get a pseudo-randomrandom integer number
            randVector[index] = random_number;
        }

        return randVector;

    }


    std::map<int, uint32_t>
    MonteCarlo::input2val(const std::vector<uint8_t> &input, std::vector<int> &bv) {
        std::map<int, uint32_t> input_v;
        assert(input.size() == bv.size());
        auto input_size = bv.size();
        for (int i = 0; i < input_size; ++i) {
            input_v.insert(std::pair<int, uint32_t>(bv[i], input[i]));
        }
        return input_v;
    }

    std::vector<int>
    MonteCarlo::getAllKeys(
            const std::vector<std::tuple<uint32_t, std::shared_ptr<tana::Constrain>, LeakageType>> &constrains){

        std::vector<int> input_key_vector;

        for(const auto &element :constrains)
        {
            auto &cons = std::get<1>(element);
            std::vector<int> input_k1 = cons->getInputKeys();
            input_key_vector.insert(std::end(input_key_vector), std::begin(input_k1), std::end(input_k1));
        }

        sort( input_key_vector.begin(), input_key_vector.end() );
        input_key_vector.erase( unique( input_key_vector.begin(), input_key_vector.end() ), input_key_vector.end() );

        return input_key_vector;

    }

    bool MonteCarlo::constrainSatisify(const std::vector<std::tuple<uint32_t, std::shared_ptr<tana::Constrain>, LeakageType >> &constrains,
                           const std::map<int, uint32_t > &input_map)
    {
        for(const auto &element :constrains)
        {
            auto &cons = std::get<1>(element);
            if(!cons->validate(input_map))
            {
                return false;
            }
        }

        return true;

    }


    float MonteCarlo::calculateMonteCarlo(const std::vector<std::tuple<uint32_t, std::shared_ptr<tana::Constrain>, LeakageType >> &constrains,
                                  uint64_t sample_num)
    {
        std::vector<int > input_vector = getAllKeys(constrains);
        auto input_vector_size = input_vector.size();

        uint64_t satisfied_number = 0, total_num = sample_num;

        for(uint64_t i = 0; i < sample_num; ++i)
        {
            auto random_vector = getRandomVector(input_vector_size);
            auto random_vector_map = input2val(random_vector, input_vector);
            if(constrainSatisify(constrains, random_vector_map))
            {
                ++satisfied_number;
            }

        }

        float result = (static_cast<float>(satisfied_number)) / (static_cast<float>(total_num));

        return result;

    }


    FastMonteCarlo::FastMonteCarlo(uint64_t sample_num, std::vector<std::tuple<uint32_t, std::shared_ptr<tana::Constrain>, LeakageType >> con)
    :num_sample(sample_num), constrains(con), num_satisfied(0), dist(0, 255)
    {
        tests.reserve(sample_num);
        input_vector = MonteCarlo::getAllKeys(con);
        unsigned int input_demension = input_vector.size();
        for(uint64_t i = 0; i < sample_num; ++i)
        {
            tests.push_back(std::make_unique<std::pair<std::vector<uint8_t >, bool>>(std::make_pair(getRandomVector(input_demension), true)));
        }
    }

    void FastMonteCarlo::testConstrain(const std::shared_ptr<tana::Constrain> &con)
    {
        for(auto& it : tests)
        {
            std::pair<std::vector<uint8_t>, bool>& one_test = *it;
            if(one_test.second) {
                auto random_vector_map = MonteCarlo::input2val(one_test.first, input_vector);
                bool flag = con->validate(random_vector_map);
                one_test.second = flag;
            }
        }
    }

    void FastMonteCarlo::run(){

        for(const auto &element :constrains)
        {
            auto &cons = std::get<1>(element);
            this->testConstrain(cons);
        }
        for(const auto &test: tests)
        {
            if(test->second)
            {
                ++num_satisfied;
            }
        }


    }

    float FastMonteCarlo::getResult()
    {
        float result = (static_cast<float>(num_satisfied)) / (static_cast<float>(num_sample));
        return result;

    }

    std::vector<uint8_t > FastMonteCarlo::getRandomVector(unsigned int size)
    {
        std::vector<uint8_t> randVector(size);
        for (uint32_t index = 0; index < size; ++index) {

            auto random_number = dist(engine); // finally get a pseudo-randomrandom integer number
            randVector[index] = random_number;
        }

        return randVector;
    }
}
