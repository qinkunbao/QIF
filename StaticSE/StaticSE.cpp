#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include "ins_parser.h"
#include "SEEngine.h"
#include "VarMap.h"

using namespace tana;
using namespace std;


int main(int argc, char **argv)
{
    if(argc != 3)
    {
        std::cout << "Usage:" << std::endl;
        std::cout << argv[0] <<" <reference> <target> " << std::endl;
        std::cout << "or" << std::endl;
        std::cout << argv[0] <<" <file> " << std::endl;
        std::cout << "The first case will compare <reference> and <target> and output any matches" << std::endl;
        std::cout << "The second case will output the formulas in the trace <file>" << std::endl;
        return 1;
    }

    stringstream ss;

    std::ifstream infile1(argv[1]), infile2(argv[2]);

    if (!infile1.is_open() || !infile2.is_open())
    {
        fprintf(stderr, "Open file error!, one of '%s' or '%s' failed to open\n", argv[1], argv[2]);
        return 1;
    }

    std::vector<std::vector<Inst_Static>> inst_list;
    parse_static_trace(infile1, infile2, inst_list);


    for(auto const &b : inst_list)
    {
        std::cout << "Block:  " << std::endl;
        for(auto const &inst : b)
        {
            inst.print();
        }

    }

    return 0;
}