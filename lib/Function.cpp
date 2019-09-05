/*************************************************************************
	> File Name: Function.cpp
	> Author: Qinkun Bao
	> Mail: qinkunbao@gmail.com
	> Created Time: Mon Apr 22 21:56:51 2019
 ************************************************************************/


#include <iostream>
#include <string>
#include <list>
#include <random>
#include "Function.h"
#include "ins_types.h"


using namespace std;

namespace tana {
    Function::Function(ifstream &function_file) {
        string line;
        uint32_t num_fun = 0;
        while (function_file.good()) {
            getline(function_file, line);
            if (line.empty()) {
                continue;
            }

            istringstream strbuf(line);
            string addr, function_name, module_name, fun_size;
            getline(strbuf, addr, ';');
            getline(strbuf, module_name, ';');
            getline(strbuf, function_name, ';');
            getline(strbuf, fun_size, ';');


            auto func = std::make_shared<Routine>();
            func->rtn_name = function_name;
            func->module_name = module_name;
            func->start_addr = stoul(addr, nullptr, 16);
            func->size = stoul(fun_size, nullptr, 10);
            func->end_addr = func->start_addr + func->size;

            auto pos = existing_rtn.find(func->start_addr);
            if(pos != existing_rtn.end())
            { continue;}

            existing_rtn.insert(func->start_addr);
            fun_rtns.push_back(func);
            ++num_fun;
            if (!(num_fun % 1000)) {
                std::cout << "Parsing Functions: " << num_fun << std::endl;
            }
            //std::cout << "112" << std::endl;
        }
        ptr_cache = fun_rtns.front();
    }

    Function::Function(const std::string &function_file_name)
    {
        ifstream function_file(function_file_name);
        string line;
        uint32_t num_fun = 0;
        while (function_file.good()) {
            getline(function_file, line);
            if (line.empty()) {
                continue;
            }

            istringstream strbuf(line);
            string addr, function_name, module_name, fun_size;
            getline(strbuf, addr, ';');
            getline(strbuf, module_name, ';');
            getline(strbuf, function_name, ';');
            getline(strbuf, fun_size, ';');


            auto func = std::make_shared<Routine>();
            func->rtn_name = function_name;
            func->module_name = module_name;
            func->start_addr = stoul(addr, nullptr, 16);
            func->size = stoul(fun_size, nullptr, 10);
            func->end_addr = func->start_addr + func->size;

            auto pos = existing_rtn.find(func->start_addr);
            if(pos != existing_rtn.end())
            { continue;}

            existing_rtn.insert(func->start_addr);
            fun_rtns.push_back(func);
            ++num_fun;
            if (!(num_fun % 1000)) {
                std::cout << "Parsing Functions: " << num_fun << std::endl;
            }
        }
        ptr_cache = fun_rtns.front();

    }

    std::string Function::getFunctionAndLibrary(tana_type::T_ADDRESS addr) {
        for (const auto& iter : fun_rtns) {
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
        for (const auto& iter : fun_rtns) {
            if ((addr >= (iter->start_addr)) && (addr <= (iter->end_addr))) {
                ptr_cache = iter;
                return iter->rtn_name;
            }
        }
        return "NOT Found";
    }

    std::shared_ptr<Routine> Function::pickOneRandomElement()
    {
        std::random_device random_device;
        std::mt19937 engine{random_device()};
        std::uniform_int_distribution<int> dist(0, fun_rtns.size() - 1);
        return fun_rtns[dist(engine)];

    }



}