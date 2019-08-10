/*************************************************************************
	> File Name: ins_parser.h
	> Author: Qinkun Bao
	> Mail: qinkunbao@gmail.com
	> Created Time: Mon Apr 22 21:56:51 2019
 ************************************************************************/

#pragma once

#include <iostream>
#include <fstream>
#include <list>
#include <sstream>
#include <memory>
#include "ins_types.h"
#include "Blocks.h"
#include "ins_semantics.h"

namespace tana {


    uint64_t file_inst_num(std::ifstream &trace_file);


    std::shared_ptr<Operand> createOperand(const std::string &s, uint32_t addr);

    std::shared_ptr<Operand> createOperandStatic(const std::string &s, uint32_t addr);

    bool parse_trace(std::ifstream &trace_file, std::vector<std::unique_ptr<Inst_Base>> &L, uint32_t max_instructions,
                     tana_type::T_ADDRESS &addr_taint, tana_type::T_SIZE &size_taint, uint32_t num);

    bool parse_trace(std::ifstream &trace_file, std::vector<std::unique_ptr<Inst_Base>> &L, uint32_t max_instructions,
                     uint32_t num);

    bool parse_trace(std::ifstream &trace_file, std::vector<std::unique_ptr<Inst_Base>> &L);

    bool parse_trace(std::ifstream &trace_file, tana_type::T_ADDRESS &addr_taint, \
                     tana_type::T_SIZE &size_taint, std::vector<std::unique_ptr<Inst_Base>> &L);

    bool parse_static_trace(std::ifstream &trace_file, std::ifstream &json_file, \
                             std::vector<Block> &L);

    bool parse_trace_qif(std::ifstream &trace_file, tana_type::T_ADDRESS &addr_taint, \
                         tana_type::T_SIZE &size_taint, std::vector<std::unique_ptr<Inst_Base>> &L,
                         std::vector<uint8_t> &key_value, uint64_t max_inst, uint64_t inst_size);

}