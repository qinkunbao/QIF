#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <map>
#include <sstream>
#include <regex>
#include <memory>
#include "Tainter.h"
#include "Function.h"
#include "ins_parser.h"

using namespace std;
using namespace tana;

int main(int argc, char* argv[]) {
	if (argc != 3) {
		cout << "Usage: " << argv[0] << " <traces.txt> <libraries.txt>" << endl;
		return 1;
	}

	ifstream trace_file(argv[1]);
	ifstream library_file(argv[2]);
	if (!library_file.is_open() || !trace_file.is_open()) {
		cout << "Cann't open files" << endl;
		return 1;
	}
	tana::Function func(&library_file);

	vector<std::unique_ptr<Inst_Dyn>> inst_list;
	tana_type::T_ADDRESS start_addr = 0;
	tana_type::T_SIZE m_size = 0;

	uint32_t id = 1;
	uint32_t batch_size = 1000;
	parse_trace(&trace_file, inst_list, 1, start_addr, m_size, id);
	Tainter taint(start_addr, m_size);


	/*
	while (!parse_trace(&trace_file, inst_list, batch_size, start_addr, m_size, id)) {

		for (auto it = inst_list.begin(); it != inst_list.end(); ++it) {
			taint.taintIns(*it);
		}
		while (!inst_list.empty())
		{
			Inst_Dyn tmp = inst_list.front();
			inst_list.clear();

		}
		id = id + batch_size;
	}
	 */

	list<tana_type::T_ADDRESS> taintedAddr;
	taintedAddr = taint.getTaintedAddress();
	for (auto addr : taintedAddr) {
		string fun_name = func.findTaintedRTN(addr);
		if (fun_name.find("NOT Found") == string::npos)
			cout <<" Function Name: " << fun_name << " Address: " << std::hex << addr << std::dec << endl;
	}


	trace_file.close();
	library_file.close();
	return 0;
}