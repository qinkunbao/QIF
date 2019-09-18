#include <iostream>
#include <algorithm>
#include <string>
//#include <filesystem>
#include "ins_parser.hpp"
#include "StaticSEEngine.hpp"
#include "VarMap.hpp"
#include "InputParser.hpp"
#include "cmd.hpp"

using namespace tana;
using namespace std;


class block_result{
    vector<shared_ptr<BitVector>> value;
    shared_ptr<Block> block;
};

string disassemble_cmd(const string &elf){
    string cmd = "python ../disassembler/disassembler.py " + elf;
    return cmd::exec(cmd.c_str());
}

/*
vector<string> return_all_files(const string &path){
    vector<string> file_name;
    for (const auto & entry : filesystem::directory_iterator(path)) {
        std::cout << entry.path() << std::endl;
        file_name.push_back(entry.path());
    }
}
*/

std::string random_string(size_t length)
{
    srand(time(nullptr));
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
    std::string file_name(filename);
    std::string blocks_file_name = file_name.substr(0, file_name.size() - 4) + "_blocks.json";
    stringstream ss;

    std::ifstream infile1(file_name), infile2(blocks_file_name);

    if (!infile1.is_open() || !infile2.is_open())
    {
        cerr << "Open file error!, one of "<< file_name << "or" << blocks_file_name << "failed to open\n";
        return 1;
    }

    std::vector<Block> block_list;
    parse_static_trace(infile1, infile2, block_list);


    auto se = new StaticSEEngine(false);

    for(auto &b : block_list)
    {
        b.print();
        se->initFromBlock(b);
        se->run();

        std::vector<std::shared_ptr<BitVector>> output_se = se->getAllOutput();
        //output_se = se->reduceValues(output_se);
        std::cout << "size: " << output_se.size() << std::endl;
        // Print all formulas in output_se
        uint32_t length = 0;
        for(size_t i = 0; i < output_se.size(); i++) {
            //(output_se[i]) = se->formula_simplfy(output_se[i]);

            length = output_se[i]->printV(ss);
            cout << "Formula index: " << i << " Length: " << length <<  " Input Number: "<< (output_se[i])->symbol_num()\
		     <<  "\nFormula: " << ss.str() << "\n";
            ss.str("");
            cout << "\n";
        }

        se->reset();
        std::cout << "===========================" << std::endl;
    }

    delete se;
    return 0;
}



int main(int argc, char **argv)
{

    if(argc == 1)
    {
        cout << "Usage: " << argv[0] << " <Binary> or <Instruction File>"<< "\n";
        cout << "Option one:" << argv[0] << " -b ELF1 ELF2\n";
        cout << "Option two:" << argv[0] << " -f Instruction_Folder_One Instruction_Folder_Two\n";
        cout << "Option three:" << argv[0] << " InstructionFile\n";
        return 1;
    }

    if(argc == 2)
    {
        printFormulas(argv[1]);

        return 1;
    }

    if(argc <= 3)
    {
        cout << "Usage: " << argv[0] << " <Binary> or <Instruction File>"<< "\n";
        cout << "Option one:" << argv[0] << " -b ELF1 ELF2\n";
        cout << "Option two:" << argv[0] << " -f Instruction_Folder_One Instruction_Folder_Two\n";
        cout << "Option three:" << argv[0] << " InstructionFile\n";
        return 1;
    }

    InputParser input(argc, argv);

    std::string file_name1;
    std::string file_name2;

    if (input.cmdOptionExists("-f")) {
        file_name1 = argv[2];
        file_name2 = argv[3];
    }

    if (input.cmdOptionExists("-b"))
    {

        std::string binary_file_name1(argv[1]);
        std::string binary_file_name2(argv[2]);
        disassemble_cmd(binary_file_name1);
        disassemble_cmd(binary_file_name2);

    }

    std::string blocks_file_name1 = file_name1.substr(0, file_name1.size() - 4) + "_blocks.json";
    std::string blocks_file_name2 = file_name2.substr(0, file_name2.size() - 4) + "_blocks.json";

    std::ifstream in_file1(file_name1), in_file1_block(blocks_file_name1);
    std::ifstream in_file2(file_name2), in_file2_block(blocks_file_name2);

    if(!in_file1.is_open() || !in_file1_block.is_open())
    {
        cerr << "Open file error!, one of "<< file_name1 << "or" << blocks_file_name1 << "failed to open\n";
    }

    if(!in_file2.is_open() || !in_file2_block.is_open())
    {
        cerr << "Open file error!, one of "<< file_name2 << "or" << blocks_file_name2 << "failed to open\n";
    }

    std::vector<Block> block_list1;
    std::vector<Block> block_list2;

    parse_static_trace(in_file1, in_file1_block, block_list1);
    parse_static_trace(in_file2, in_file2_block, block_list2);

    auto se1 = new StaticSEEngine(false);
    auto se2 = new StaticSEEngine(false);

    vector<vector<shared_ptr<BitVector>>> res1_list;
    vector<vector<shared_ptr<BitVector>>> res2_list;

    for(auto &b1 : block_list1)
    {

        se1->initFromBlock(b1);
        se1->run();
        auto output_se1 = se1->getAllOutput();
        res1_list.push_back(output_se1);
        se1->reset();
    }

    for(auto &b2 : block_list2)
    {

        se2->initFromBlock(b2);
        se2->run();
        auto output_se2 = se2->getAllOutput();
        res2_list.push_back(output_se2);
        se2->reset();
    }

    stringstream ss, match_info;


    int match_formula = 0;

    for(const auto &res1 : res1_list)
    {
        for(const auto &res2 : res2_list)
        {
            auto result = varmap::matchFunction(se1, se2, res1, res2);
            if(!result.empty())
            {
                cout << "Found " << result.size() << " possible matches" << std::endl;
                cout << argv[2] << endl;
                cout << argv[3] << endl;

                match_info << "The number of formulas in the reference: " << res1.size() << "\n";
                match_info << "The number of formulas in the target: " << res2.size() << "\n";

                match_info << "Matching formulas:" << result.size() << "\n\n";

                match_formula += result.size();

                for (const auto &it : result) {
                    match_info << "Reference Index: " << it.first << "\n";
                    match_info << "Target Index: " << it.second << "\n";

                    (res1[it.first])->printV(ss);
                    match_info << "Reference Formula: " << ss.str() << "\n";
                    ss.str("");

                    (res2[it.second])->printV(ss);
                    match_info << "Target Formula: " << ss.str() << "\n";
                    ss.str("");

                    match_info << "\n";
                }
            }

        }
    }

    string rfile = "match_"+ std::to_string(match_formula) + "_" + random_string(10) + ".txt";
    string info = match_info.str();
    ofstream match_file;

    if(info.length() > 0) {
        match_file.open(rfile);
        match_file << file_name1 << "\n";
        match_file << file_name2 << "\n";
        match_file << info;
        match_file.close();

    }

    delete se1;
    delete se2;

    return 0;
}
