/*************************************************************************
2     > File Name: Dataflow.cpp
3     > Author: Qinkun Bao
4     > Mail: qinkunbao@gmail.com
5     > Created Time: Thu 01 Nov 2018 03:51:01 PM EDT
6  ************************************************************************/


#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <string>
#include <memory>
#include "ins_parser.h"
#include "BitTaint.h"

using namespace tana;
using namespace std;

int main(int argc, char **argv) {
    if(argc != 2)
    {
        std::cout << "Usage:" << std::endl;
        std::cout << argv[0] <<" <Trace.txt> <Taint Source> " << std::endl;
        return 1;
    }

    std::ifstream infile(argv[1]);

    if (!infile.is_open()) {
        fprintf(stderr, "Failed to open file %s\n", argv[1]);
        return 1;
    }

    std::vector<std::unique_ptr<Inst_Dyn>> inst_list;

    t_type::T_ADDRESS m_addr;
    t_type::T_SIZE  m_size;

    parse_trace(&infile, m_addr, m_size, inst_list);
    infile.close();
    int inst_size = inst_list.size();

    //auto taint = new tana::bittaint::BitTaint("key", m_addr, m_size, inst_list.begin(), inst_list.end());

    //taint->run();

    return 0;
}