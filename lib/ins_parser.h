#pragma once
#include <iostream>
#include <fstream>
#include <list>
#include <sstream>
#include <memory>
#include "ins_types.h"
#include "Blocks.h"

namespace tana {


    std::shared_ptr<Operand> createOperand(std::string s, uint32_t addr);

    std::shared_ptr<Operand> createOperandStatic(std::string s, uint32_t addr);

    bool parse_trace(std::ifstream *trace_file, std::vector<std::unique_ptr<Inst_Dyn>> &L, uint32_t max_instructions,
                     tana_type::T_ADDRESS &addr_taint, tana_type::T_SIZE &size_taint, uint32_t num);

    bool parse_trace(std::ifstream *trace_file, std::vector<std::unique_ptr<Inst_Dyn>> &L, uint32_t max_instructions,
                     uint32_t num);

    bool parse_trace(std::ifstream *trace_file, std::vector<std::unique_ptr<Inst_Dyn>> &L);

    bool parse_trace(std::ifstream *trace_file, tana_type::T_ADDRESS &addr_taint,\
                     tana_type::T_SIZE &size_taint, std::vector<std::unique_ptr<Inst_Dyn>> &L );

    bool parse_static_trace (std::ifstream &trace_file, std::ifstream &json_file, \
                             std::vector<Block> &L);

}