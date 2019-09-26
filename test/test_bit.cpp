#include <iostream>
#include <bitset>


using namespace std;

int main()
{
    uint32_t num = 0xffffffff;
    bool test = (num & (0x80000000u)) >> 31u;

    std::cout << test << std::endl;
}
