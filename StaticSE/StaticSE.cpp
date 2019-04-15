#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include "ins_parser.h"
#include "SEEngine.h"
#include "VarMap.h"

using namespace tana;
using namespace std;

int printFormulas(char* filename)
{
    std::string file_name(filename);
    std::string blocks_file_name = file_name.substr(0, file_name.size() - 4) + "_blocks.json";
    stringstream ss;

    std::ifstream infile1(file_name), infile2(blocks_file_name);

    if (!infile1.is_open() || !infile2.is_open())
    {
        fprintf(stderr, "Open file error!, one of '%s' or '%s' failed to open\n", file_name.c_str(), blocks_file_name.c_str());
        return 1;
    }

    std::vector<Block> block_list;
    parse_static_trace(infile1, infile2, block_list);


    for(auto const &b : block_list)
    {
        b.print();
    }

    return 0;
}



int main(int argc, char **argv)
{

    if(argc == 2)
    {
        printFormulas(argv[1]);

        return 1;
    }

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



    return 0;
}