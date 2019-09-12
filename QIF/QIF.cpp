/*************************************************************************
	> File Name: QIF.cpp
	> Author: 
	> Mail: 
	> Created Time: Mon May  6 15:25:32 2019
 ************************************************************************/

#include <iostream>
#include <memory>
#include <chrono>
#include <cmath>
#include <cstring>
#include <algorithm>
#include "MonteCarlo.h"
#include "ins_parser.h"
#include "QIFSEEngine.h"
#include "Function.h"
#include "Trace2ELF.h"
#include "InputParser.h"


using namespace std::chrono;
using namespace std;
using namespace tana;





float getEntropy(std::vector<uint8_t> key_value, \
                 uint64_t MonteCarloTimes, \
                 std::vector<std::tuple<uint32_t, std::shared_ptr<tana::Constrain>, LeakageType>> constrains, \
                 const std::string &fileName, \
                 std::shared_ptr<Function> func, \
                 std::map<int, uint32_t> key_value_map) {
    using clock = std::chrono::system_clock;
    using ms = std::chrono::milliseconds;
    const auto before = clock::now();

    FastMonteCarlo res(MonteCarloTimes, constrains, key_value, func, key_value_map);
    res.verifyConstrain();
    res.run();
    res.run_addr_group();
    res.calculateConstrains(fileName);
    res.print_group_result(fileName);
    float MonteCarloResult = res.getResult();

    const auto duration = std::chrono::duration_cast<ms>(clock::now() - before);

    std::cout << "It took " << duration.count() / 1000.0 << " ms"
              << " to finish the monte carlo sampling" << std::endl;
    return abs(-log(MonteCarloResult) / log(2));
}

std::vector<std::string> split(std::string str, std::string sep) {
    char *cstr = const_cast<char *>(str.c_str());
    char *current;
    std::vector<std::string> arr;
    current = strtok(cstr, sep.c_str());
    while (current != nullptr) {
        arr.push_back(current);
        current = strtok(nullptr, sep.c_str());
    }
    return arr;
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <traces.txt> " << "<options>" << "\n";
        cout << "Option one:   -t <Monte Carlo Times>\n";
        cout << "Option two:   -t <Monte Carlo Times> -f <Function Name>\n";
        cout << "Option three: -t <Monte Carlo Times> -f <Function Name> -d <Debug Info>\n";
        return 1;
    }

    std::shared_ptr<Function> func = nullptr;
    std::shared_ptr<Trace2ELF> t2e = nullptr;
    uint64_t MonteCarloTimes = 10000;
    InputParser input(argc, argv);

    std::string traceFileName(argv[1]);
    std::string sep("/");
    auto fileSegment = split(traceFileName, sep);
    auto fileName = "result_" + fileSegment.rbegin()[1] + fileSegment.back();
    bool cmdStatus = false;

    if (input.cmdOptionExists("-t")) {
        const std::string &mc_times = input.getCmdOption("-t");
        stringstream strValue;
        strValue << mc_times;
        uint64_t temp;
        strValue >> temp;
        MonteCarloTimes = temp;
        cmdStatus = true;
    }

    if (input.cmdOptionExists("-f")) {
        const std::string &fun_name = input.getCmdOption("-f");
        func = std::make_shared<Function>(fun_name);
        cmdStatus = true;
    }

    if (input.cmdOptionExists("-t") && input.cmdOptionExists("-f")) {
        const std::string &mc_times = input.getCmdOption("-t");
        stringstream strValue;
        strValue << mc_times;
        uint64_t temp;
        strValue >> temp;
        MonteCarloTimes = temp;

        const std::string &fun_name = input.getCmdOption("-f");
        ifstream func_file(fun_name);
        func = std::make_shared<Function>(func_file);
        cmdStatus = true;
    }

    if (input.cmdOptionExists("-t")
        && input.cmdOptionExists("-f")
        && input.cmdOptionExists("-d")) {
        const std::string &mc_times = input.getCmdOption("-t");
        stringstream strValue;
        strValue << mc_times;
        uint64_t temp;
        strValue >> temp;
        MonteCarloTimes = temp;

        const std::string &fun_name = input.getCmdOption("-f");
        func = std::make_shared<Function>(fun_name);

        const std::string &obj_name = input.getCmdOption("-f");
        t2e = std::make_shared<Trace2ELF>(obj_name, fun_name);

    }
    ifstream trace_file(argv[1]);
    if (!trace_file.is_open()) {
        cout << "Can't open files" << endl;
        return 1;
    }

    if (!cmdStatus) {
        cout << "Incorrect Input Option \n";
        exit(0);
    }


    vector<unique_ptr<Inst_Base>> inst_list;
    tana_type::T_ADDRESS start_addr = 0;
    tana_type::T_SIZE m_size = 0;
    vector<uint8_t> key_value;

    auto start = high_resolution_clock::now();

    int max_inst = 90000000;
    auto inst_size = file_inst_num(trace_file);
    std::cout << "Total Instructions:: " << inst_size << std::endl;
    trace_file.close();
    trace_file.open(argv[1]);

    parse_trace_qif(trace_file, start_addr, m_size, inst_list, key_value, max_inst, inst_size, func);

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
    if (argc == 4) {
        se->init(inst_list.begin(), inst_list.end(), start_addr, m_size / 8, key_value, func);
    } else {
        se->init(inst_list.begin(), inst_list.end(), start_addr, m_size / 8, key_value);

    }
    se->run();
    se->reduceConstrains();
    se->printConstrains();

    auto key_value_map = se->return_key_value_map();
    auto constraints = se->getConstraints();

    std::cout << "Start Monte Carlo:" << std::endl;
    std::cout << "Total Leaked Bits = " << getEntropy(key_value, MonteCarloTimes, constraints, fileName, func,
                                                      key_value_map)
              << std::endl;

    auto stop = high_resolution_clock::now();

    auto duration = duration_cast<microseconds>(stop - start);

    cout << "Time taken by QIF: "
         << duration.count() / 1000000 << " seconds" << endl;


    return 0;
}