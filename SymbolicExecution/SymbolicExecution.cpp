#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <memory>
#include "ins_parser.hpp"
#include "DynSEEngine.hpp"
#include "VarMap.hpp"

using namespace tana;
using namespace std;

bool Imm2SymState = false;


std::string random_string(size_t length)
{
	srand(time(NULL));
	auto randchar = []() -> char
	{
		const char charset[] =
			"0123456789"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz";
		const size_t max_index = (sizeof(charset) - 1);
		return charset[rand() % max_index];
	};
	std::string str(length, 0);
	generate_n(str.begin(), length, randchar);
	return str;
}

int printFormulas(char* filename)
{
	std::ifstream infile(filename);

	if (!infile.is_open()) {
		fprintf(stderr, "Failed to open file %s\n", filename);
		return 1;
	}

	std::vector<unique_ptr<Inst_Base>> inst_list;
	parse_trace(infile, inst_list);
	infile.close();

	// Bit symbolic execution
	auto *se = new DynSEEngine(Imm2SymState);
	se->initAllRegSymol(inst_list.begin(), inst_list.end());
	se->run();

	std::vector<std::shared_ptr<BitVector>> output_se = se->getAllOutput();
	//output_se = se->reduceValues(output_se);
	std::cout << "size: " << output_se.size() << std::endl;
	// Print all formulas in output_se
	stringstream ss;
	uint32_t length = 0;
	for(size_t i = 0; i < output_se.size(); i++) {
        (output_se[i]) = se->formula_simplfy(output_se[i]);

        length = output_se[i]->printV(ss);
        cout << "Formula index: " << i << " Length: " << length <<  " Input Number: "<< (output_se[i])->symbol_num()\
		     <<  "\nFormula: " << ss.str() << "\n";
        ss.str("");
        cout << "\n";
	}
	delete se;

	return 0;
}


int main(int argc, char **argv) {

    if(argc == 2) {
        printFormulas(argv[1]);

        return 1;

	}

	if (argc != 3) {
		std::cout << "Usage:" << std::endl; 
        std::cout << argv[0] <<" <reference> <target> " << std::endl;
		std::cout << "or" << std::endl;
        std::cout << argv[0] <<" <file> " << std::endl;
        std::cout << "The first case will compare <reference> and <target> and output any matches" << std::endl;
        std::cout << "The second case will output the formulas in the trace <file>" << std::endl;
		return 1;
	}
	stringstream ss;

    std::ifstream infile1(argv[1]), infile2(argv[2]);

	if (!infile1.is_open() || !infile2.is_open()) {
		fprintf(stderr, "Open file error!, one of '%s' or '%s' failed to open\n", argv[1], argv[2]);
		return 1;
	}


	std::vector<unique_ptr<Inst_Base>> inst_list1, inst_list2;

	parse_trace(infile1, inst_list1);
	parse_trace(infile2, inst_list2);

	infile1.close();
	infile2.close();


	// Symbolic execution
	auto se1 = new DynSEEngine(Imm2SymState);
	se1->initAllRegSymol(inst_list1.begin(), inst_list1.end());
	se1->run();

	auto se2 = new DynSEEngine(Imm2SymState);
	se2->initAllRegSymol(inst_list2.begin(), inst_list2.end());
	se2->run();

	auto output_se1 = se1->getAllOutput();
	auto output_se2 = se2->getAllOutput();
	output_se1 = se1->reduceValues(output_se1);
	output_se2 = se1->reduceValues(output_se2);

	std::cout << "reference size " << output_se1.size() << std::endl;
	std::cout << "target size " << output_se2.size() << std::endl;

	if (output_se1.size() > (output_se2.size() * 3))
		return 0;
	if (output_se2.size() > (output_se1.size() * 3))
		return 0;

	std::cout << output_se1.size() << " output values" << std::endl;
	auto result = varmap::matchFunction(se1, se2, output_se1, output_se2);
	string src(argv[1]);
	string dest(argv[2]);
	delete se1;
	delete se2;

	
	if (result.size() > 0) {
		cout << "Found " << result.size() << " possible matches" << std::endl;
		cout << argv[1] << endl;
		cout << argv[2] << endl;
		string rfile = "match_"+ std::to_string(result.size()) + "_" + random_string(10) + ".txt";
		ofstream result_file;
		result_file.open(rfile);
		
		result_file << "Reference: " << argv[1] << "\n";
		result_file << "Target: " << argv[2] << "\n";

		result_file << "The number of formulas in the reference: " << output_se1.size() << "\n";
		result_file << "The number of formulas in the target: " << output_se2.size() << "\n";

		result_file << "Matching formulas:" << result.size() << "\n\n";

		for (const auto &it : result) {
			result_file << "Reference Index: " << it.first << "\n";
			result_file << "Target Index: " << it.second << "\n";

            (output_se1[it.first])->printV(ss);
			result_file << "Reference Formula: " << ss.str() << "\n";
			ss.str("");

            (output_se2[it.second])->printV(ss);
			result_file << "Target Formula: " << ss.str() << "\n";
			ss.str("");

            result_file << "\n";
		}
		result_file.close();
	}
    return 0;
}

