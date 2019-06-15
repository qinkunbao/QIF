/*************************************************************************
	> File Name: error.cpp
	> Author: Qinkun Bao
	> Mail: qinkunbao@gmail.com
	> Created Time: Mon Apr 22 21:56:51 2019
 ************************************************************************/

#include <string>
#include <iostream>

#include "error.h"

namespace tana {
    void default_error_handler(const char *file, int line, const char *message) {
        std::string file_name(file);
        std::string error_message(message);
        std::cout << "\n********************************" << std::endl;
        std::cout << "Error at :" << line << "\n";
        std::cout << "File name: " << file_name << "\n";
        std::cout << "Message: " << error_message << "\n";
        std::cout << "********************************\n" << std::endl;
        //exit(0);
    }

    void default_warn_handler(const char *file, int line, const char *message) {
        std::string file_name(file);
        std::string error_message(message);
        std::cout << "\n********************************" << std::endl;
        std::cout << "Error at :" << line << "\n";
        std::cout << "File name: " << file_name << "\n";
        std::cout << "Message: " << error_message << "\n";
        std::cout << "********************************\n" << std::endl;
    }
}