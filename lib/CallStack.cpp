/*************************************************************************
	> File Name: CallStack.cpp
	> Author: 
	> Mail: 
	> Created Time: Tue Aug  6 14:10:11 2019
 ************************************************************************/


#include <iostream>
#include <tuple>
#include "CallStack.h"

using namespace std;


namespace tana {



    CallStack::CallStack(string name, int key_id, std::string start_fun):key_id(key_id), key_name(name)
    {
        int depth = 0;
        auto first_call = std::make_tuple(start_fun, depth);

        call_stack_sites.push_back(first_call);
    }

    int CallStack::updateStack(int input_key_id, const std::string &fun_name)
    {
        if(input_key_id != key_id)
            return 0;

        tuple<string, int> current_stack = call_stack_sites.back();

        string current_fun = get<0>(current_stack);
        int current_stack_max = get<1>(current_stack);

        if(current_fun == fun_name)
        {
            return current_stack_max;
        }

        auto call = std::make_tuple(fun_name, current_stack_max+1);
        call_stack_sites.push_back(call);

        return current_stack_max + 1;
    }

    std::ostream &operator<<(std::ostream &os, const CallStack &c)
    {
        os << c.key_name;
        for(const auto &call : c.call_stack_sites)
        {
            os << " -> ";
            const string& fun_name = get<0>(call);
            os << fun_name;

        }

        return os;
    }
}