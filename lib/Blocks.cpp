#include "Blocks.h"
#include "ins_types.h"


namespace tana {

    Block::Block(uint32_t n_addr, uint32_t n_end_addr, uint32_t n_inputs, uint32_t n_ninstr, \
              uint32_t n_outputs, uint32_t n_size, uint32_t n_trace) : addr(n_addr), end_addr(n_end_addr), \
                                                                       inputs(n_inputs), \
                                                                       ninstr(n_ninstr), outputs(n_outputs), \
                                                                       size(n_size), traced(n_trace)
    {}

    bool Block::init(std::vector<tana::Inst_Static> &fun_inst)
    {
        bool copy_flag = false;
        for (auto const &ins : fun_inst)
        {
            if(ins.addrn == addr)
            {
                copy_flag = true;
            }

            if (!copy_flag)
            {
                continue;
            }

            Inst_Static ins_t = ins;
            inst_list.push_back(ins);

            if(ins.addrn == end_addr)
            {
                return true;
            }
        }

        return false;
    }

    void Block::print() const
    {
        std::cout << "Block: \n";
        std::cout << "Start Address: " << std::hex << addr << " End Address: " << end_addr <<std::dec << "\n";
        std::cout << "Size: " << size << std::endl;
        for(auto const &inst : inst_list) {
            inst.print();
        }
    }
}