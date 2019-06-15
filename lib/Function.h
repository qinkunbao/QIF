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

	class Function {
	private:
		std::list<tana::Routine> rtn_libraries;


	public:
		Function(std::ifstream *function_file);

		std::string findTaintedRTN(tana::tana_type::T_ADDRESS addr);

		~Function();
	};

}

