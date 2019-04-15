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

