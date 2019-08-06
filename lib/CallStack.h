/*************************************************************************
	> File Name: CallStack.h
	> Author: 
	> Mail: 
	> Created Time: Tue Aug  6 14:10:20 2019
 ************************************************************************/


#pragma once

#include <vector>
#include <string>
#include <tuple>

namespace tana {

    class CallStack {

    private:
        std::vector<std::tuple<std::string, int>> call_stack_sites;
        int key_id;
        std::string key_name;


    public:

        CallStack(std::string key_name, int key_id, std::string start_fun);

        int updateStack(int key_id, const std::string &fun_name);

        friend std::ostream &operator<<(std::ostream &os, const CallStack &c);


    };
}


