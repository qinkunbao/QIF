/*************************************************************************
	> File Name: Ngram.cpp
	> Author: Qinkun Bao
	> Mail: qinkunbao@gmail.com
	> Created Time: Thu Oct 10 21:46:18 2019
 ************************************************************************/

#include <iostream>
#include <memory>
#include "InputParser.hpp"
#include "ins_parser.hpp"
#include "ins_types.hpp"

using namespace std;
using namespace tana;


int main(int argc, char **argv){
    if (argc < 2) {

        string build_time(__TIMESTAMP__);

#ifdef NDEBUG
        cout << "Build Time: " << build_time << "\n";
        cout << "Release Build\n";
#else
        cout << "Build Time: " << build_time << "\n";
        cout << "Debug build\n";
#endif
        cout << "Usage: " << argv[0] << " -s <source_trace.txt> " << "-t <target_trace.txt>" << "\n";

        return 1;
    }

    string source_trace_file_name, target_trace_file_name;
    bool cmdStatus = false;
    InputParser input(argc, argv);

    if(input.cmdOptionExists("-s") &&
       input.cmdOptionExists("-t"))
    {
        source_trace_file_name = input.getCmdOption("-s");
        target_trace_file_name = input.getCmdOption("-t");
        cmdStatus = true;
    }

    if(!cmdStatus){
        cout << "Usage: " << argv[0] << " -s <source_trace.txt> " << "-t <target_trace.txt>" << "\n";

        return 1;
    }

    std::vector<unique_ptr<Inst_Base>> source_inst_vector, target_inst_vector;

    ifstream source_inst_file(source_trace_file_name);
    ifstream target_inst_file(target_trace_file_name);

    if(!source_inst_file.is_open() || !target_inst_file.is_open())
    {
        cout << "Can not open the trace file\n";
        return 1;
    }


    parse_trace(source_inst_file, source_inst_vector);
    parse_trace(target_inst_file, target_inst_vector);



    return 0;
}
