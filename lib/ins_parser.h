#pragma once
#include <iostream>
#include <fstream>
#include <list>
#include <sstream>
#include <memory>
#include "ins_types.h"

namespace tana {


    class Block {
    public:
        uint32_t addr;
        uint32_t end_addr;
        uint32_t inputs;
        uint32_t jump;
        uint32_t ninstr;
        uint32_t outputs;
        uint32_t size;
        bool traced;
    };


    std::shared_ptr<Operand> createOperand(std::string s, uint32_t addr);

    bool parse_trace(std::ifstream *trace_file, std::vector<Inst_Dyn> *L, uint32_t max_instructions,
                     t_type::T_ADDRESS &addr_taint, t_type::T_SIZE &size_taint, uint32_t num);

    bool parse_trace(std::ifstream *trace_file, std::vector<Inst_Dyn> *L, uint32_t max_instructions, uint32_t num);

    bool parse_trace(std::ifstream *trace_file, std::vector<Inst_Dyn> *L);

    bool parse_trace(std::ifstream *trace_file, t_type::T_ADDRESS &addr_taint,\
                     t_type::T_SIZE &size_taint, std::vector<Inst_Dyn> *L );

    bool parse_static_trace (std::ifstream &trace_file, std::ifstream &json_file, \
                             std::vector<std::vector<Inst_Static>> &L);

}