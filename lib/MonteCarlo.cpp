/*************************************************************************
        > File Name: MonteCarlo.cpp
        > Author: Qinkun Bao
        > Mail: qinkunbao@gmail.com
        > Created Time: Mon May 20 21:06:14 2019
 ************************************************************************/
#include <algorithm>
#include <random>
#include <set>
#include <iostream>
#include <fstream>
#include "MonteCarlo.h"
#include "error.h"

namespace tana {
    std::vector<uint8_t> MonteCarlo::getRandomVector(unsigned int size) {
        std::vector<uint8_t> randVector(size);
        for (uint32_t index = 0; index < size; ++index) {
            std::random_device random_device;         // create object for seeding
            std::minstd_rand engine{random_device()}; // create engine and seed it
            std::uniform_int_distribution<uint8_t> dist(
                    0, 255); // create distribution for integers with [1; 9] range
            auto random_number =
                    dist(engine); // finally get a pseudo-randomrandom integer number
            randVector[index] = random_number;
        }

        return randVector;
    }

    std::map<int, uint32_t> MonteCarlo::input2val(const std::vector<uint8_t> &input,
                                                  std::vector<int> &bv) {
        std::map<int, uint32_t> input_v;
        assert(input.size() >= bv.size());
        auto input_size = bv.size();
        for (int i = 0; i < input_size; ++i) {
            input_v.insert(std::pair<int, uint32_t>(bv[i], input[i]));
        }
        return input_v;
    }

    std::vector<int> MonteCarlo::getAllKeys(
            const std::vector<std::tuple<uint32_t, std::shared_ptr<tana::Constrain>,
                    LeakageType>> &constrains) {

        std::vector<int> input_key_vector;

        for (const auto &element : constrains) {
            auto &cons = std::get<1>(element);
            std::vector<int> input_k1 = cons->getInputKeys();
            input_key_vector.insert(std::end(input_key_vector), std::begin(input_k1),
                                    std::end(input_k1));
        }

        sort(input_key_vector.begin(), input_key_vector.end());
        input_key_vector.erase(
                unique(input_key_vector.begin(), input_key_vector.end()),
                input_key_vector.end());

        return input_key_vector;
    }

    bool MonteCarlo::constrainSatisfy(
            const std::vector<std::tuple<uint32_t, std::shared_ptr<tana::Constrain>,
                    LeakageType>> &constrains,
            const std::map<int, uint32_t> &input_map) {
        for (const auto &element : constrains) {
            auto &cons = std::get<1>(element);
            if (!cons->validate(input_map)) {
                return false;
            }
        }

        return true;
    }

    float MonteCarlo::calculateMonteCarlo(
            const std::vector<std::tuple<uint32_t, std::shared_ptr<tana::Constrain>,
                    LeakageType>> &constrains,
            uint64_t sample_num) {
        std::vector<int> input_vector = getAllKeys(constrains);
        auto input_vector_size = input_vector.size();

        uint64_t satisfied_number = 0, total_num = sample_num;

        for (uint64_t i = 0; i < sample_num; ++i) {
            auto random_vector = getRandomVector(input_vector_size);
            auto random_vector_map = input2val(random_vector, input_vector);
            if (constrainSatisfy(constrains, random_vector_map)) {
                ++satisfied_number;
            }
        }

        float result =
                (static_cast<float>(satisfied_number)) / (static_cast<float>(total_num));

        return result;
    }

    FastMonteCarlo::FastMonteCarlo(
            uint64_t sample_num,
            std::vector<std::tuple<uint32_t, std::shared_ptr<tana::Constrain>, LeakageType>> con,
            std::vector<uint8_t> key_value, std::map<int, uint32_t> key_value_map_c)
            : num_sample(sample_num), constrains(con), num_satisfied(0), dist(0, 255),
              input_seed(key_value), isFunctionInformationAvailable(false), func(nullptr),
              key_value_map(key_value_map_c){
        tests.reserve(sample_num);
        input_vector = MonteCarlo::getAllKeys(con);
        unsigned int input_dimensions = input_vector.size();

        int step = 0, step_size = sample_num / 10;
        spiltConstrainsByAddress();
        for (uint64_t i = 0; i < sample_num; ++i) {
            tests.push_back(std::make_unique<std::pair<std::vector<uint8_t>, bool>>(
                    std::make_pair(getRandomVector(input_dimensions), true)));
            if (i / step_size > step) {
                step = i / step_size;
                std::cout << "Generating samples: " << step << "0%" << std::endl;
            }
        }
    }

    FastMonteCarlo::FastMonteCarlo(
            uint64_t sample_num,
            std::vector<std::tuple<uint32_t, std::shared_ptr<tana::Constrain>, LeakageType>> con,
            std::vector<uint8_t> key_value, std::shared_ptr<Function> fun, std::map<int, uint32_t> key_value_map_c)
            : num_sample(sample_num), constrains(con), num_satisfied(0), dist(0, 255),
              input_seed(key_value), isFunctionInformationAvailable(true), func(fun),
              key_value_map(key_value_map_c){
        tests.reserve(sample_num);
        input_vector = MonteCarlo::getAllKeys(con);
        unsigned int input_dimensions = input_vector.size();
        if(func == nullptr)
        {
            isFunctionInformationAvailable = false;
        }

        int step = 0, step_size = sample_num / 10;
        spiltConstrainsByAddress();
        for (uint64_t i = 0; i < sample_num; ++i) {
            tests.push_back(std::make_unique<std::pair<std::vector<uint8_t>, bool>>(
                    std::make_pair(getRandomVector(input_dimensions), true)));
            if (i / step_size > step) {
                step = i / step_size;
                std::cout << "Generating samples: " << step << "0%" << std::endl;
            }
        }
    }

    bool FastMonteCarlo::verifyConstrain() {
        bool flag;
        //auto key_value_map = MonteCarlo::input2val(input_seed, input_vector);
        //debug_map(key_value_map);
        auto it = constrains.begin();
        std::set<uint32_t> addr_set;
        while (it != constrains.end()) {
            auto &cons = std::get<1>(*it);
            flag = cons->validate(key_value_map);
            if (flag) {
                ++it;
            } else {
                std::cout << "Failed Constrains: " << std::endl;
                std::cout << std::hex << std::get<0>(*it) << std::dec << " : ";
                std::cout << *cons << std::endl;
                cons->validate(key_value_map);
                if(isFunctionInformationAvailable) {
                    std::cout << func->getFunctionAndLibrary(std::get<0>(*it)) << "\n" << std::endl;
                }
                addr_set.insert(std::get<0>(*it));
                it = constrains.erase(it);
            }
        }
        std::cout << "\n Number of Failed Constrains: " << addr_set.size() <<"\n"<< std::endl;
        return true;
    }

    void FastMonteCarlo::calculateConstrains(const std::string &result) {
        int num_DA = 0, num_CF = 0;
        for (const auto &cons_vector :constrains_group_addr) {
            auto one_cons = cons_vector.front();
            if (std::get<2>(one_cons) == LeakageType::CFLeakage) {
                ++num_CF;
            }
            if (std::get<2>(one_cons) == LeakageType::DALeakage) {
                ++num_DA;
            }
        }
        std::cout << "Total Constrains: " << constrains_group_addr.size() << std::endl;
        std::cout << "Control Transfer: " << num_CF << std::endl;
        std::cout << "Data Access: " << num_DA << std::endl;

        std::ofstream myfile;
        myfile.open (result);
        myfile << "Total Constrains: " << constrains_group_addr.size() << std::endl;
        myfile << "Control Transfer: " << num_CF << std::endl;
        myfile << "Data Access: " << num_DA << std::endl;
        myfile.close();
    }

    void FastMonteCarlo::testConstrain(
            const std::shared_ptr<tana::Constrain> &con) {
        for (auto &it : tests) {
            std::pair<std::vector<uint8_t>, bool> &one_test = *it;
            if (one_test.second) {
                auto random_vector_map =
                        MonteCarlo::input2val(one_test.first, input_vector);
                bool flag = con->validate(random_vector_map);
                one_test.second = flag;

            }
        }
    }

    void FastMonteCarlo::run() {

        std::cout << "Start Computing "
                  << "Constrain: " << constrains.size() << std::endl;
        this->reset_tests();
        for (const auto &element : constrains) {
            auto &cons = std::get<1>(element);
            this->testConstrain(cons);
            // std::cout << "finishing one constrain";
        }
        for (const auto &test : tests) {
            if (test->second) {
                ++num_satisfied;
            }
        }
    }

    void FastMonteCarlo::run_addr_group() {

        auto it = constrains_group_addr.begin();
        while (it != constrains_group_addr.end()) {
            uint64_t num_satisfied_for_group = 0;
            this->reset_tests();
            for (const auto &element : (*it)) {
                auto &cons = std::get<1>(element);
                this->testConstrain(cons);
                // std::cout << "finishing one constrain";
            }
            for (const auto &test : tests) {
                if (test->second) {
                    ++num_satisfied_for_group;
                }
            }
            if(num_satisfied_for_group == 0)
            {
                ++it;
                continue;
            }
            if ((this->num_sample / num_satisfied_for_group) == 1) {
                it = constrains_group_addr.erase(it);
            } else {
                uint32_t addr = std::get<0>((*it).front());
                auto result = std::make_tuple(addr, num_satisfied_for_group);
                num_satisfied_group.push_back(result);
                ++it;
            }
        }
    }

    float FastMonteCarlo::getResult() {
        float result =
                (static_cast<float>(num_satisfied)) / (static_cast<float>(num_sample));
        return result;
    }

    std::vector<uint8_t> FastMonteCarlo::getRandomVector(unsigned int size) {
        std::vector<uint8_t> randVector(size);
        for (uint32_t index = 0; index < size; ++index) {

            auto random_number =
                    dist(engine); // finally get a pseudo-randomrandom integer number
            randVector[index] = random_number;
        }

        return randVector;
    }

    void FastMonteCarlo::spiltConstrainsByAddress() {
        for (const auto &constrain : constrains) {
            uint32_t addr = std::get<0>(constrain);
            std::vector<std::tuple<uint32_t, std::shared_ptr<tana::Constrain>,
                    LeakageType>> *constrain_group =
                    internal_find_constrain_group_by_addr(addr);

            if (constrain_group == nullptr) {
                std::vector<
                        std::tuple<uint32_t, std::shared_ptr<tana::Constrain>, LeakageType>>
                        constrains_group;
                constrains_group.push_back(constrain);
                constrains_group_addr.push_back(constrains_group);

            } else {
                constrain_group->push_back(constrain);
            }
        }
    }

    std::vector<std::tuple<uint32_t, std::shared_ptr<tana::Constrain>, LeakageType>>
    *FastMonteCarlo::internal_find_constrain_group_by_addr(uint32_t addr) {
        for (auto &constrain_group : constrains_group_addr) {
            uint32_t con_addr = std::get<0>(constrain_group.front());
            if (con_addr == addr) {
                return &constrain_group;
            }
        }

        return nullptr;
    }

    void FastMonteCarlo::reset_tests() {
        for (auto &it : tests) {
            it->second = true;
        }
    }

    void FastMonteCarlo::print_group_result(const std::string &result) {
        auto sample_num = tests.size();
        std::cout << "Information Leak for each address: \n";
        for (auto &it : num_satisfied_group) {
            uint32_t addr = std::get<0>(it);
            uint64_t num = std::get<1>(it);
            if (num != 0) {
                float portion =
                        (static_cast<float>(num)) / (static_cast<float>(sample_num));
                float leaked_information = abs(-log(portion) / log(2));
                std::cout << "Address: " << std::hex << addr << std::dec
                          << " Leaked:" << leaked_information << " bits"
                          << " Num of Satisfied: " << num << std::endl;
                if(isFunctionInformationAvailable) {
                    std::cout << func->getFunctionAndLibrary(addr) << "\n" << std::endl;
                }
            } else {
                std::cout << "Address: " << std::hex << addr << std::dec;
                std::cout << " Monte Carlo Failed" << std::endl;
                if(isFunctionInformationAvailable) {
                    std::cout << func->getFunctionAndLibrary(addr) << "\n" << std::endl;
                }
            }
        }

        std::ofstream myfile;
        myfile.open (result, std::ios_base::app);
        int constrain_index = 0;
        for (auto &it : num_satisfied_group) {
            uint32_t addr = std::get<0>(it);
            uint64_t num = std::get<1>(it);
            myfile << "------------------------------------------------------------\n";
            if (num != 0) {
                float portion =
                        (static_cast<float>(num)) / (static_cast<float>(sample_num));
                float leaked_information = abs(-log(portion) / log(2));
                myfile << "Address: " << std::hex << addr << std::dec
                       << " Leaked:" << leaked_information << " bits"
                       << " Num of Satisfied: " << num << std::endl;
                if(isFunctionInformationAvailable) {
                    myfile << func->getFunctionAndLibrary(addr) << "\n" << std::endl;
                }
            } else {
                myfile << "Address: " << std::hex << addr << std::dec;
                myfile << " Monte Carlo Failed" << std::endl;
                if(isFunctionInformationAvailable) {
                    myfile << func->getFunctionAndLibrary(addr) << "\n" << std::endl;
                }
            }

            for (auto &con : constrains_group_addr) {
                if(std::get<0>(con[0]) == addr) {
                    myfile << "Number of constrains: " << con.size();

                    for (auto &each_con : con) {
                        myfile << "\n";
                        myfile << *(std::get<1>(each_con));
                    }
                }
            }

            ++constrain_index;
            myfile << "------------------------------------------------------------\n";
        }
        myfile.close();
    }

} // namespace tana
