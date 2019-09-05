#include <iostream>
#include <fstream>
#include "DebugInfo.h"

using namespace std;
using namespace tana;

int main()
{
    ifstream debug_file("../test/debug_openssl_0.9.7.txt");
    auto test_debuginfo = DebugInfo(debug_file);
    return 0;
}
