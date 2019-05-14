#include "Blocks.h"
#include "ins_types.h"


namespace tana {

    Block::Block(uint32_t n_addr, uint32_t n_end_addr, uint32_t n_inputs, uint32_t n_ninstr, \
              uint32_t n_outputs, uint32_t n_size, uint32_t n_trace) : addr(n_addr), end_addr(n_end_addr), \
                                                                       inputs(n_inputs), \
                                                                       ninstr(n_ninstr), outputs(n_outputs), \
                                                                       size(n_size), traced(n_trace)
    {}

    bool Block::init(std::vector<std::unique_ptr<Inst_Base>> &fun_inst)
    {
        bool copy_flag = false;
        bool finish_flag = false;
        for (auto &ins : fun_inst)
        {
            auto inst_addr = ins->addrn;
            if(inst_addr == addr)
            {
                copy_flag = true;
            }

            if (!copy_flag)
            {
                continue;
            }

            if(inst_addr >= end_addr)
            {
                if(inst_addr == end_addr)
                {
                    finish_flag = true;
                }

                break;
            }

            inst_list.push_back(std::move(ins));


        }
        auto start_pos = fun_inst.begin();
        while (start_pos != fun_inst.end())
        {
            if(*start_pos == nullptr)
            {
                fun_inst.erase(start_pos);
            }
            else
            ++start_pos;
        }

        return finish_flag;
    }

    void Block::print() const
    {
        std::cout << "Block: " << this->id << "\n";
        std::cout << "Start Address: " << std::hex << addr << " End Address: " << end_addr <<std::dec << "\n";
        std::cout << "Block Size: " << size << std::endl;
        for(auto const &inst : inst_list) {
            std::cout << inst << "\n";
        }

        std::cout << "\n";
    }



}