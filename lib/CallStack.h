/*************************************************************************
	> File Name: CallStack.h
	> Author: Qinkun Bao
	> Mail: qinkunbao@gmail.com
	> Created Time: Tue Aug  6 14:10:20 2019
 ************************************************************************/


#pragma once

#include <vector>
#include <string>
#include <tuple>
#include <stack>
#include <map>
#include <memory>

namespace tana {

    class CallStackKey {

    private:
        std::vector<std::tuple<std::string, int>> leakage_stack_sites;
        std::stack<std::string> stack_funs;
        const int key_id;
        const std::string key_name;

        int stack_depth;


    public:

        CallStackKey(std::string key_name, int key_id, const std::string &start_fun);

        int updateStack(int key_id, const std::string &fun_name);

        friend std::ostream &operator<<(std::ostream &os, const CallStackKey &c);

        int retStack(const std::string &fun_name);


    };


    class CallStack {
    private:
        std::map<int, std::unique_ptr<CallStackKey>> stacks;
    public:
        CallStack(const std::string &start_fun_name,
                  const std::vector<std::tuple<int, std::string>> &key_value_set);
    };
}


