/*************************************************************************
	> File Name: CallStack.cpp
	> Author: Qinkun Bao
	> Mail: qinkunbao@gmail.com
	> Created Time: Tue Aug  6 14:10:11 2019
 ************************************************************************/


#include <iostream>
#include <tuple>
#include "CallStack.h"

using namespace std;


namespace tana {



    CallStackKey::CallStackKey(string name, int key_id, const std::string &start_fun): key_id(key_id), key_name(name),
                                                                                stack_depth(0)
    {
        auto first_call = std::make_tuple(start_fun, stack_depth);

        leakage_stack_sites.push_back(first_call);
    }

    int CallStackKey::updateStack(int input_key_id, const std::string &fun_name)
    {
        if(input_key_id != key_id)
            return 0;

        tuple<string, int> current_stack = leakage_stack_sites.back();

        string current_fun = get<0>(current_stack);
        int current_stack_max = get<1>(current_stack);

        if(current_fun == fun_name)
        {
            return current_stack_max;
        }

        ++stack_depth;
        stack_funs.push(fun_name);

        auto call = std::make_tuple(fun_name, stack_depth);
        leakage_stack_sites.push_back(call);

        return stack_depth;
    }

    std::ostream &operator<<(std::ostream &os, const CallStackKey &c)
    {
        os << c.key_name;
        for(const auto &call : c.leakage_stack_sites)
        {
            os << " -> ";
            const string& fun_name = get<0>(call);
            os << fun_name;

        }

        return os;
    }

    int CallStackKey::retStack(const std::string &fun_name)
    {
        auto current_fun = stack_funs.top();
        if (current_fun != fun_name)
        {
            return 0;
        }
        --stack_depth;
        stack_funs.pop();
        return stack_depth;

    }

    CallStack::CallStack(const std::string &start_fun_name,
                         const std::vector<std::tuple<int, std::string>> &key_value_set)
    {

        for(const auto &key_tuple : key_value_set)
        {
            int key_id = get<0>(key_tuple);
            string key_str;
            key_str = get<1>(key_tuple);
            auto element = std::make_unique<CallStackKey>(key_str, key_id, start_fun_name);
            stacks[key_id] = std::move(element);
        }

    }
}