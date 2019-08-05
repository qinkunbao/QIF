/*************************************************************************
	> File Name: QIF.cpp
	> Author: 
	> Mail: 
	> Created Time: Mon May  6 15:25:32 2019
 ************************************************************************/

#include <iostream>
#include <memory>
#include <sstream>
#include <chrono>
#include <cmath>
#include <string.h>
#include "MonteCarlo.h"
#include "ins_parser.h"
#include "QIFSEEngine.h"
#include "Function.h"

using namespace std::chrono;
using namespace std;
using namespace tana;


float getEntropy(std::vector<uint8_t> key_value,  \
                 uint64_t MonteCarloTimes, \
                 std::vector<std::tuple<uint32_t, std::shared_ptr<tana::Constrain>, LeakageType>>
                 constrains, \
                 std::string fileName) {
    using clock = std::chrono::system_clock;
    using ms = std::chrono::milliseconds;
    const auto before = clock::now();

    FastMonteCarlo res(MonteCarloTimes, constrains, key_value);
    res.verifyConstrain();
    res.run();
    res.run_addr_group();
    res.calculateConstrains();
    res.print_group_result(fileName);
    float MonteCarloResult = res.getResult();

    const auto duration = std::chrono::duration_cast<ms>(clock::now() - before);

    std::cout << "It took " << duration.count() / 1000.0 << " ms"
              << " to finish the monte carlo sampling" << std::endl;
    return abs(-log(MonteCarloResult) / log(2));
}

std::vector<std::string> split(std::string str,std::string sep){
    char* cstr=const_cast<char*>(str.c_str());
    char* current;
    std::vector<std::string> arr;
    current=strtok(cstr,sep.c_str());
    while(current!=NULL){
        arr.push_back(current);
        current=strtok(NULL,sep.c_str());
    }
    return arr;
}


int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <traces.txt> " << " <Monte Carlo Times> "
        << "\n";
        cout << "or\n";
        cout << "Usage: " << argv[0] << " <traces.txt> " << endl;
        return 1;
    }


    uint64_t MonteCarloTimes = 10000;
    if(argc == 3)
    {
      stringstream strValue;
      strValue << argv[2];
      uint64_t temp;
      strValue >> temp;
      MonteCarloTimes = temp;
    }

    if(argc == 4)
    {
        ifstream func_file(argv[3]);
        auto fun = new Function(&func_file);
    }

    ifstream trace_file(argv[1]);
    if (!trace_file.is_open()) {
        cout << "Can't open files" << endl;
        return 1;
    }

    std::string traceFileName(argv[1]);
    std::string sep("/");
    auto fileSegment = split(traceFileName, sep);
    auto fileName = "result_" + fileSegment.rbegin()[1] + fileSegment.back();

    vector<std::unique_ptr<Inst_Base>> inst_list;
    tana_type::T_ADDRESS start_addr = 0;
    tana_type::T_SIZE m_size = 0;
    vector<uint8_t> key_value;

    auto start = high_resolution_clock::now();

    int max_inst = 200000;
    parse_trace_qif(&trace_file, start_addr, m_size, inst_list, key_value, max_inst);

    std::cout << "Start Address: " << std::hex << start_addr << std::dec
              << " Length: " << m_size << std::endl;

    uint32_t eax, ebx, ecx, edx, esi, edi, esp, ebp;
    auto reg = (inst_list.front())->vcpu;
    eax = reg.gpr[0];
    ebx = reg.gpr[1];
    ecx = reg.gpr[2];
    edx = reg.gpr[3];
    esi = reg.gpr[4];
    edi = reg.gpr[5];
    esp = reg.gpr[6];
    ebp = reg.gpr[7];

    auto *se = new QIFSEEngine(eax, ebx, ecx, edx, esi, edi, esp, ebp);
    se->init(inst_list.begin(), inst_list.end(), start_addr, m_size/8, key_value);
    se->run();
    se->reduceConstrains();
    se->printConstrains();

    auto constrains = se->getConstrains();

    std::cout << "Start Monte Carlo:" << std::endl;
    std::cout << "Total Leaked Bits = "<< getEntropy(key_value, MonteCarloTimes, constrains, fileName)
    << std::endl;

    auto stop = high_resolution_clock::now();

    auto duration = duration_cast<microseconds>(stop - start);

    cout << "Time taken by QIF: "
         << duration.count()/1000000 << " seconds" << endl;


    return 0;
}