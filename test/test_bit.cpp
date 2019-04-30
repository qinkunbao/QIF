#include <iostream>
#include <bitset>

using namespace std;

int main()
{
    uint32_t num = 0xffffffff;
    bool test = (num & 0x80000000) >> 31;

    std::cout << test << std::endl;
}
