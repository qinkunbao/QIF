#include <string>
#include <iostream>
#include "cmd.h"

int main()
{
    auto result = tana::cmd::exec("ls");
    std::cout << result << std::endl;
    return 0;
}