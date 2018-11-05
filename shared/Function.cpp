#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <map>
#include <sstream>
#include "Function.h"
#include "ins_types.h"


using namespace std;

namespace tana {
    Function::Function(ifstream *function_file) {
        string line;
        uint32_t num_fun = 0;
        while (function_file->good()) {
            getline(*function_file, line);
            if (line.empty()) {
                continue;
            }

            istringstream strbuf(line);
            string addr, function_name, module_name;
            getline(strbuf, addr, ';');
            getline(strbuf, module_name, ';');
            getline(strbuf, function_name, ';');

            Routine *func = new Routine();
            func->rtn_name = function_name;
            func->module_name = module_name;
            func->start_addr = stoul(addr, 0, 16);
            for (auto it = rtn_libraries.begin(); it != rtn_libraries.end(); ++it) {
                if (it->start_addr == func->start_addr) {
                    continue;
                }
            }
            rtn_libraries.push_back(*func);
            ++num_fun;
            if (!(num_fun % 1000)) {
                std::cout << "Parsing Functions: " << num_fun << std::endl;
            }
            //std::cout << "112" << std::endl;
        }
        rtn_libraries.sort([](const Routine &a, const Routine &b) { return a.start_addr < b.start_addr; });
        for (auto iter = rtn_libraries.begin(); next(iter, 1) != rtn_libraries.end(); ++iter) {
            iter->end_addr = next(iter, 1)->start_addr;
        }
        rtn_libraries.pop_back();
    }

    std::string Function::findTaintedRTN(t_type::T_ADDRESS addr) {
        for (auto iter = rtn_libraries.begin(); iter != rtn_libraries.end(); iter++) {
            if ((addr >= (iter->start_addr)) && (addr <= (iter->end_addr))) {
                return iter->rtn_name + " Module Name: " + iter->module_name;
            }
        }
        return "NOT Found";
    }

    Function::~Function() {
    }

}