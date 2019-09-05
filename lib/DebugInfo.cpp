/*****
    > File Name: DynSEEngine.cpp
    > Author: Qinkun Bao & Zihao Wang
    > Mail: qinkunbao@gmail.com
    > Created Time: Tue Sep  3 17:19:02 EDT 2019
 ************************************************************************/

#include "DebugInfo.h"

#include <string.h>

#include <fstream>
#include <iostream>
#include <memory>
#include <vector>

using namespace std;
using namespace tana;

DebugInfo::DebugInfo(std::ifstream &debug_file)
{
    // TODO by Zihao
    // TODO: Use string

    char buffer[256];
    string fPwd;
    string fileName;
    uint32_t binaryAddress;
    int lineNumber;

    if (!debug_file.is_open())
    {
        cout << "DebugInfo: Error opening debug file";
        exit(1);
    }

    // Find first line begin with 'CU:'
    while (debug_file.getline(buffer, 256) &&
           (buffer[strlen(buffer) - 1] != ':') &&
           debug_file.good())
    {
    }

    debug_file >> fileName;
    // Read content
    while (debug_file.good())
    {
        fPwd = fileName;
        if (fPwd[0] == 'C')
        {
            // Skip 'CU:'
            debug_file >> fPwd;
            // Skip table header
            debug_file.getline(buffer, 256);
            debug_file.getline(buffer, 256);
        }

        if (fPwd[fPwd.length() - 1] == ':')
        {
            fPwd.pop_back();
        }
        else
        {
            fPwd.resize(fPwd.size() - 5);
        }
        cout << fPwd << endl;

        debug_file >> fileName;
        while (debug_file.good() && fileName[fileName.length() - 1] != ':' && fileName[fileName.length() - 1] != ']')
        {
            debug_file >> dec >> binaryAddress >> hex >> lineNumber;
            // DebugSymbol::DebugSymbol(fPwd, fileName, binaryAddress, lineNumber);
            cout << fPwd << fileName << dec << binaryAddress << "0x" << hex
                 << lineNumber << endl;
            debug_file >> fileName;
        }
    }
}

shared_ptr<DebugSymbol> DebugInfo::locateSym(uint32_t addr)
{
    // TODO by Zihao
    return nullptr;
}
