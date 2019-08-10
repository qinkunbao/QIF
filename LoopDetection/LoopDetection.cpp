#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <list>
#include <map>
#include <stack>
#include <vector>
#include <set>
#include <memory>
#include "ins_parser.h"
#include "Loop.h"

using namespace std;
using namespace tana;

int main(int argc, char **argv) {
    string file_name;
    if (argc != 2) {
        std::cout << "usage:" << argv[0] << "<tracefile>" << std::endl;
        return 1;
    }
    file_name = argv[1];
    std::ifstream infile(argv[1]);
    if (!infile.is_open()) {
        std::cout << "Open file error!" << std::endl;
        return 1;
    }
    vector<std::unique_ptr<Inst_Base>> inst_list;
    uint32_t batch_size = 2000;
    uint32_t id = 1;

    while (!parse_trace(infile, inst_list, batch_size, id))
    {
        id = id + batch_size;
    }

    id = inst_list.size();
    auto result = loop::loopDetection(inst_list, id);
    loop::outPrintLoops(result, file_name);

    return 0;
}
