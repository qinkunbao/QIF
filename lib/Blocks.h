/*************************************************************************
	> File Name: Blocks.h
	> Author: Qinkun Bao
	> Mail: qinkunbao@gmail.com
	> Created Time: Mon Apr 22 21:56:51 2019
 ************************************************************************/

#pragma once

#include <vector>
#include <string>
#include <memory>
#include "ins_types.h"

namespace tana {

    class Block {
    public:
        std::vector<std::unique_ptr<Inst_Base>> inst_list;
        uint32_t addr;
        uint32_t end_addr;
        uint32_t inputs;
        uint32_t ninstr;
        uint32_t outputs;
        uint32_t size;
        int id = 0;
        bool traced;

        Block(uint32_t n_addr, uint32_t n_end_addr, uint32_t n_inputs, uint32_t n_ninstr, \
              uint32_t n_outputs, uint32_t n_size, uint32_t trace);

        bool init(std::vector<std::unique_ptr<Inst_Base>> &fun_inst);

        void print() const;

    };

}