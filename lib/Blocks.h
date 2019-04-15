#pragma once

#include <vector>
#include <string>
#include "ins_types.h"

namespace tana {

    class Block {
    private:
        std::vector<Inst_Static> inst_list;
    public:
        uint32_t addr;
        uint32_t end_addr;
        uint32_t inputs;
        uint32_t ninstr;
        uint32_t outputs;
        uint32_t size;
        bool traced;
        Block(uint32_t n_addr, uint32_t n_end_addr, uint32_t n_inputs, uint32_t n_ninstr, \
              uint32_t n_outputs, uint32_t n_size, uint32_t trace);
        bool init(std::vector<Inst_Static> &fun_inst);

        void print() const;
    };

}