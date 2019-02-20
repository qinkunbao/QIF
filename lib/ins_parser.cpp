#include <regex>
<<<<<<< HEAD:lib/ins_parser.cpp
#include <nlohmann/json.hpp>

=======
>>>>>>> 87e7a135fdd3fe5ffe1bd6714e2a79056d5304d7:lib/ins_parser.cpp

#include "ins_parser.h"

namespace tana {

    std::shared_ptr<Operand> createAddrOperand(std::string s);

    std::shared_ptr<Operand> createDataOperand(std::string s, uint32_t addr);

    std::shared_ptr<Operand> createOperand(std::string s, uint32_t addr);

    std::shared_ptr<Operand> createAddrOperand(std::string s) {
        // regular expressions addresses
        std::regex addr1("0x[[:xdigit:]]+");
        std::regex addr2("eax|ebx|ecx|edx|esi|edi|esp|ebp");
        std::regex addr3("(eax|ebx|ecx|edx|esi|edi|esp|ebp)\\*([[:digit:]])");

        std::regex addr4("(eax|ebx|ecx|edx|esi|edi|esp|ebp)(\\+|-)(0x[[:xdigit:]]+)");
        std::regex addr5("(eax|ebx|ecx|edx|esi|edi|esp|ebp)\\+(eax|ebx|ecx|edx|esi|edi|esp|ebp)\\*([[:digit:]])");
        std::regex addr6("(eax|ebx|ecx|edx|esi|edi|esp|ebp)\\*([[:digit:]])(\\+|-)(0x[[:xdigit:]]+)");

        std::regex addr7(
                "(eax|ebx|ecx|edx|esi|edi|esp|ebp)\\+(eax|ebx|ecx|edx|esi|edi|esp|ebp)\\*([[:digit:]])(\\+|-)(0x[[:xdigit:]]+)");


        //Operand *opr = new Operand();
        std::shared_ptr<Operand> opr(new Operand());
        std::smatch m;

        // pay attention to the matching order: long sequence should be matched first,
        // then the subsequence.
        if (regex_search(s, m, addr7)) { // addr7: eax+ebx*2+0xfffff1
            opr->type = Operand::Mem;
            opr->tag = 7;
            opr->field[0] = m[1]; // eax
            opr->field[1] = m[2]; // ebx
            opr->field[2] = m[3]; // 2
            opr->field[3] = m[4]; // +
            opr->field[4] = m[5]; // 0xfffff1
        } else if (regex_search(s, m, addr4)) { // addr4: eax+0xfffff1
            // cout << "addr 4: " << s << endl;
            opr->type = Operand::Mem;
            opr->tag = 4;
            opr->field[0] = m[1];
            opr->field[1] = m[2];
            opr->field[2] = m[3];
        } else if (regex_search(s, m, addr5)) { // addr5: eax+ebx*2
            opr->type = Operand::Mem;
            opr->tag = 5;
            opr->field[0] = m[1]; // eax
            opr->field[1] = m[2]; // ebx
            opr->field[2] = m[3]; // 2
        } else if (regex_search(s, m, addr6)) { // addr6: eax*2+0xfffff1
            opr->type = Operand::Mem;
            opr->tag = 6;
            opr->field[0] = m[1]; // eax
            opr->field[1] = m[2]; // 2
            opr->field[2] = m[3]; // +
            opr->field[3] = m[4]; // 0xfffff1
        } else if (regex_search(s, m, addr3)) { // addr3: eax*2
            opr->type = Operand::Mem;
            opr->tag = 3;
            opr->field[0] = m[1];
            opr->field[1] = m[2];
        } else if (regex_search(s, m, addr1)) { // addr1: Immdiate value address
            opr->type = Operand::Mem;
            opr->tag = 1;
            opr->field[0] = m[0];
        } else if (regex_search(s, m, addr2)) { // addr2: 32 bit register address
            // cout << "addr 2: " << s << endl;
            opr->type = Operand::Mem;
            opr->tag = 2;
            opr->field[0] = m[0];
        } else {
            std::cout << "Unknown addr operands: " << s << std::endl;
        }

        return opr;
    }

    std::shared_ptr<Operand> createDataOperand(std::string s, uint32_t addr) {
        // Regular expressions for Immvalue and Registers
        std::regex immvalue("0x[[:xdigit:]]+");
        std::regex reg8("al|ah|bl|bh|cl|ch|dl|dh");
        std::regex reg16("ax|bx|cx|dx|si|di|bp|cs|ds|es|fs|gs|ss");
        std::regex reg32("eax|ebx|ecx|edx|esi|edi|esp|ebp|st0|st1|st2|st3|st4|st5");

        //Operand *opr = new Operand();
        std::shared_ptr<Operand> opr(new Operand());
        std::smatch m;
        if (regex_search(s, m, reg32)) { // 32 bit register
            opr->type = Operand::Reg;
            opr->bit = 32;
            opr->field[0] = m[0];
        } else if (regex_search(s, m, reg16)) { // 16 bit register
            opr->type = Operand::Reg;
            opr->bit = 16;
            opr->field[0] = m[0];
        } else if (regex_search(s, m, reg8)) { // 8 bit register
            opr->type = Operand::Reg;
            opr->bit = 8;
            opr->field[0] = m[0];
        } else if (regex_search(s, m, immvalue)) {
            opr->type = Operand::ImmValue;
            opr->bit = 32;
            opr->field[0] = m[0];
        } else {
            std::cout << "Unknown data operands: " << s << "Addr: " << std::hex << addr << std::dec << "\n";
        }

        return opr;
    }

    std::shared_ptr<Operand> createOperand(std::string s, uint32_t addr) {
        std::regex ptr("ptr \\[(.*)\\]");
        std::regex byteptr("byte ptr \\[(.*)\\]");
        std::regex wordptr("word ptr \\[(.*)\\]");
        std::regex dwordptr("dword ptr \\[(.*)\\]");
        std::regex segptr("dword ptr (fs|gs):\\[(.*)\\]");
        std::smatch m;


        std::shared_ptr<Operand> opr;

        if (s.find("ptr") != std::string::npos) { // Operand is a mem access addr
            if (regex_search(s, m, byteptr)) {
                opr = createAddrOperand(m[1]);
                opr->bit = 8;
            } else if (regex_search(s, m, dwordptr)) {
                opr = createAddrOperand(m[1]);
                opr->bit = 32;
            } else if (regex_search(s, m, segptr)) {
                opr = createAddrOperand(m[2]);
                opr->issegaddr = true;
                opr->bit = 32;
                opr->segreg = m[1];
            } else if (regex_search(s, m, wordptr)) {
                opr = createAddrOperand(m[1]);
                opr->bit = 16;
            } else if (regex_search(s, m, ptr)) {
                opr = createAddrOperand(m[1]);
                opr->bit = 0;
            } else {
                std::cout << "Unkown addr: " << s << std::endl;
            }
        } else {                   // Operand is data
            // cout << "data operand: " << s << endl;
            opr = createDataOperand(s, addr);
        }

        return opr;
    }

    void parseOperand(std::vector<Inst_Dyn>::iterator begin, std::vector<Inst_Dyn>::iterator end) {
        // parse operands
        for (auto it = begin; it != end; ++it) {
            for (uint32_t i = 0; i < it->get_operand_number(); ++i) {
                it->oprd[i] = createOperand(it->oprs[i], it->addrn);
            }
        }

    }

    bool parse_trace(std::ifstream *trace_file, std::vector<Inst_Dyn> *L) {
        uint32_t batch_size = 1;
        uint32_t id = 1;
        bool finish_parse = parse_trace(trace_file, L, batch_size, id);
        while (!finish_parse) {
            id = id + batch_size;
            finish_parse = parse_trace(trace_file, L, batch_size, id);
        }
        return finish_parse;
    }


    bool parse_trace(std::ifstream *trace_file, t_type::T_ADDRESS &addr_taint, \
                     t_type::T_SIZE &size_taint, std::vector<Inst_Dyn> *L) {
        uint32_t batch_size = 1000;
        uint32_t id = 1;
        bool finish_parse = parse_trace(trace_file, L, 1, addr_taint, size_taint, id);
        while (!finish_parse) {
            finish_parse = parse_trace(trace_file, L, batch_size, addr_taint, size_taint, id);
            id = id + batch_size;

        }
        return finish_parse;

    }


    bool parse_trace(std::ifstream *trace_file, std::vector<Inst_Dyn> *L, uint32_t max_instructions, uint32_t num) {
        t_type::T_ADDRESS addr_taint = 0;
        t_type::T_SIZE size_taint = 0;
        return parse_trace(trace_file, L, max_instructions, addr_taint, size_taint, num);
    }

    bool parse_trace(std::ifstream *trace_file, std::vector<Inst_Dyn> *L, uint32_t max_instructions,
                     t_type::T_ADDRESS &addr_taint, t_type::T_SIZE &size_taint, uint32_t num) {
        std::string line;
        uint32_t id_count = 1;
        while (trace_file->good() && (id_count <= max_instructions)) {
            getline(*trace_file, line);
            if (line.empty()) {
                break;
            }

            if (line.find("Start") != std::string::npos) {
                std::istringstream fun_buf(line);
                std::string start_taint, taint_len, temp_str;
                getline(fun_buf, temp_str, ';');
                getline(fun_buf, start_taint, ';');
                getline(fun_buf, taint_len, ';');
                addr_taint = stoul(start_taint, 0, 16);
                size_taint = stoul(taint_len, 0, 10);

                return false;

            }

            if (line.find("END") != std::string::npos) {
                return true;
            }

            std::istringstream strbuf(line);
            std::string temp, disasstr, temp_addr;

            //Inst *ins = new Inst();
            std::unique_ptr<Inst_Dyn> ins(new Inst_Dyn());

            ins->id = num++;
            id_count++;
            // instruction address
            getline(strbuf, temp_addr, ';');
            ins->addrn = stoul(temp_addr, 0, 16);

            // get dissassemble string
            getline(strbuf, disasstr, ';');
            //ins->dissass = disasstr;
            std::istringstream disasbuf(disasstr);

            std::string opcstr = "";
            getline(disasbuf, opcstr, ' ');

            //ins->opcstr = opcstr;
            ins->instruction_id = x86::insn_string2id(opcstr);

            while (disasbuf.good()) {
                getline(disasbuf, temp, ',');
                if (temp.find_first_not_of(' ') != std::string::npos)
                    ins->oprs.push_back(temp);
            }

            // get 8 register value
            for (int i = 0; i < GPR_NUM; ++i) {
                getline(strbuf, temp, ',');
                ins->vcpu.gpr[i] = std::stoul(temp, 0, 16);
            }

            getline(strbuf, temp, ',');
            ins->memory_address = std::stoul(temp, 0, 16);

            //Get EPFLAGS
            getline(strbuf, temp, ',');
            if (!temp.empty()) {
                ins->vcpu.set_eflags(std::stoul(temp, 0, 16));
            }

            L->push_back(*ins);

        }
        if (trace_file->good())
            return false;
        else
            return true;
    }

    bool parse_static_trace (std::ifstream &trace_file, std::ifstream &json_file, std::vector<std::vector<Inst_Base>> *L)
    {

        std::vector<Block> blocks;

        nlohmann::json blocks_json = nlohmann::json::array();
        json_file >> blocks_json;

        for(auto &element: blocks_json)
        {
            Block block;
            std::string str_addr, str_end_addr, str_inputs, str_jump, str_ninstr, str_outputs, str_size, str_traced;
            str_addr = element["addr"];
            block.addr = std::stoul(str_addr, 0 ,16);

            str_end_addr = element["end_addr"];
            block.end_addr = std::stoul(str_end_addr, 0 ,16);

            str_inputs = element["inputs"];
            block.inputs = std::stoul(str_inputs, 0 ,16);

            str_jump = element["jump"];
            block.jump = std::stoul(str_jump, 0 ,16);

            str_ninstr = element["ninstr"];
            block.ninstr = std::stoul(str_ninstr, 0 ,16);

            str_outputs = element["outputs"];
            block.outputs = std::stoul(str_outputs, 0 ,16);

            str_size = element["size"];
            block.size = std::stoul(str_size, 0 ,16);

            str_traced = element["traced"];
            block.traced = std::stoul(str_traced, 0 ,16);

            blocks.push_back(block);
        }



       // std::cout << trace_file.rdbuf();
        //std::cout << "````````````````````````````````````````\n";

        std::string line;
        while (trace_file.good())
        {
            getline(trace_file, line);
            std::cout << line << std::endl;
        }



    }
}