#pragma once
#include <iostream>
#include <fstream>
#include <list>
#include <sstream>
#include <memory>
#include "ins_types.h"

namespace tana {

    void parseOperand(std::vector<Inst>::iterator begin, std::vector<Inst>::iterator end);

    bool parse_trace(std::ifstream *trace_file, std::vector<Inst> *L, uint32_t max_instructions,
                     t_type::T_ADDRESS &addr_taint, t_type::T_SIZE &size_taint, uint32_t num);

    bool parse_trace(std::ifstream *trace_file, std::vector<Inst> *L, uint32_t max_instructions, uint32_t num);

    bool parse_trace(std::ifstream *trace_file, std::vector<Inst> *L);

    bool parse_trace(std::ifstream *trace_file, t_type::T_ADDRESS &addr_taint,\
                     t_type::T_SIZE &size_taint, std::vector<Inst> *L );
}