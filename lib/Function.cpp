/*************************************************************************
	> File Name: Function.cpp
	> Author: Qinkun Bao
	> Mail: qinkunbao@gmail.com
	> Created Time: Mon Apr 22 21:56:51 2019
 ************************************************************************/


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
            string addr, function_name, module_name, fun_size;
            getline(strbuf, addr, ';');
            getline(strbuf, module_name, ';');
            getline(strbuf, function_name, ';');
            getline(strbuf, fun_size, ';');


            Routine *func = new Routine();
            func->rtn_name = function_name;
            func->module_name = module_name;
            func->start_addr = stoul(addr, nullptr, 16);
            func->size = stoul(fun_size, nullptr, 10);
            func->end_addr = func->start_addr + func->size;

            auto pos = existing_rtn.find(func->start_addr);
            if(pos != existing_rtn.end())
            { continue;}

            existing_rtn.insert(func->start_addr);
            rtn_libraries.push_back(*func);
            ++num_fun;
            if (!(num_fun % 1000)) {
                std::cout << "Parsing Functions: " << num_fun << std::endl;
            }
            //std::cout << "112" << std::endl;
        }
        ptr_cache = rtn_libraries.begin();
    }

    std::string Function::findTaintedRTN(tana_type::T_ADDRESS addr) {
        for (auto iter = rtn_libraries.begin(); iter != rtn_libraries.end(); ++iter) {
            if ((addr >= (iter->start_addr)) && (addr <= (iter->end_addr))) {
                return "Function Name: " + iter->rtn_name + " Module Name: " + iter->module_name + " Offset: "
                                         + std::to_string(addr - iter->start_addr);
            }
        }
        return "NOT Found";
    }

    std::string Function::getFunName(tana::tana_type::T_ADDRESS addr) {
        if ((addr >= (ptr_cache->start_addr)) && (addr <= (ptr_cache->end_addr))) {
            return ptr_cache->rtn_name;
        }
        for (auto iter = rtn_libraries.begin(); iter != rtn_libraries.end(); ++iter) {
            if ((addr >= (iter->start_addr)) && (addr <= (iter->end_addr))) {
                ptr_cache = iter;
                return iter->rtn_name;
            }
        }
        return "NOT Found";
    }

    Function::~Function() {
    }

}