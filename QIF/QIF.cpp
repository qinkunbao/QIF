/*************************************************************************
	> File Name: QIF.cpp
	> Author: 
	> Mail: 
	> Created Time: Mon May  6 15:25:32 2019
 ************************************************************************/

#include<iostream>
#include <memory>
#include "ins_parser.h"
#include "QIFSEEngine.h"

using namespace std;
using namespace tana;


int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <traces.txt> " << endl;
        return 1;
    }

    ifstream trace_file(argv[1]);
    if (!trace_file.is_open()) {
        cout << "Cann't open files" << endl;
        return 1;
    }

    vector<std::unique_ptr<Inst_Base>> inst_list;
    tana_type::T_ADDRESS start_addr = 0;
    tana_type::T_SIZE m_size = 0;

    parse_trace(&trace_file, start_addr, m_size, inst_list);

    std::cout << "Start Address: " << std::hex << start_addr << std::dec << " Length: " << m_size << std::endl;
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
    se->init(inst_list.begin(), inst_list.end(), start_addr, m_size);
    se->run();
    se->outputConstrains();
    return 1;
}