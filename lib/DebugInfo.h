/*************************************************************************
	> File Name: DynSEEngine.cpp
	> Author: Qinkun Bao & Zihao Wang
	> Mail: qinkunbao@gmail.com
	> Created Time: Tue Sep  3 17:19:02 EDT 2019
 ************************************************************************/

#pragma once


#include <vector>
#include <memory>
#include <string>
#include <fstream>


namespace tana{

    class DebugSymbol{
    public:
        std::string pwd;
        std::string file_name;
        uint32_t address;
        int line_number;
        DebugSymbol(const std::string& fPwd, const std::string& fileName, uint32_t binaryAddress, int lineNumber):
                   pwd(fPwd), file_name(fileName), address(binaryAddress), line_number(lineNumber){}
    };


    class DebugInfo{
    private:
        std::vector<std::shared_ptr<DebugSymbol>> line_info;
    public:
        explicit DebugInfo(std::ifstream &debug_file);
        explicit DebugInfo(std::string &debug_string);
        std::shared_ptr<DebugSymbol> locateSym(uint32_t addr);
        ~DebugInfo() = default;
    };


}