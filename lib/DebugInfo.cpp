/*************************************************************************
	> File Name: DynSEEngine.cpp
	> Author: Qinkun Bao & Zihao Wang
	> Mail: qinkunbao@gmail.com
	> Created Time: Tue Sep  3 17:19:02 EDT 2019
 ************************************************************************/

#include <vector>
#include <memory>
#include <string>
#include <fstream>
#include "DebugInfo.h"

using namespace std;
using namespace tana;


DebugInfo::DebugInfo(std::ifstream &debug_file) {
    // TODO by Zihao
}

DebugInfo::DebugInfo(std::string &debug_string) {
    // TODO
}

shared_ptr<DebugSymbol> DebugInfo::locateSym(uint32_t addr)
{
    // TODO by Zihao
    return nullptr;
}
