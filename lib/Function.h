/*************************************************************************
	> File Name: Function.h
	> Author: Qinkun Bao
	> Mail: qinkunbao@gmail.com
	> Created Time: Mon Apr 22 21:56:51 2019
 ************************************************************************/

#pragma once
#include <list>
#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <map>
#include <sstream>

#include "ins_types.h"

namespace tana {

    class Routine {
    public:
        tana_type::T_ADDRESS start_addr; // start address of the function
        tana_type::T_ADDRESS end_addr;   // end address of the function
        std::string rtn_name;
        std::string module_name;
        tana_type::T_ADDRESS size;

        Routine() : start_addr(0), end_addr(0), rtn_name(), module_name(), size(0) {}
    };

	class Function {
	private:
		std::list<tana::Routine> rtn_libraries;
		std::set<tana_type::T_ADDRESS> existing_rtn;


	public:
		explicit Function(std::ifstream *function_file);

		std::string findTaintedRTN(tana::tana_type::T_ADDRESS addr);

		~Function();
	};

}

