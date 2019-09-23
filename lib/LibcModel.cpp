/*************************************************************************
	> File Name: LibcModel.hpp
	> Author: Qinkun Bao
	> Mail: qinkunbao@gmail.com
	> Created Time: Sun Sep 22 16:30:10 EDT 2019
 ************************************************************************/

#include <string>
#include <sstream>
#include "LibcModel.hpp"
#include "BitVector.hpp"
#include "ins_types.hpp"
#include "Engine.hpp"

using namespace std;
using namespace tana;


string uint32ToStr(uint32_t addr)
{
    stringstream sstream;
    sstream << hex << addr << dec;
    return sstream.str();
}

bool LIBC_X86_Calloc::symbolic_execution(tana::SEEngine *se)
{
    assert(!this->is_static);
    uint32_t num, size, ret;
    num = this->m_num;
    size = this->m_size;
    ret = this->m_ret_value;
    uint32_t num_bytes = size * num;
    assert(ret % 4 ==0);
    string addr_str;
    for(uint32_t i = 0; i < num_bytes; ++i)
    {
        auto data = std::make_shared<BitVector>(ValueType::CONCRETE, 0);
        uint32_t addr = ret + i * T_DWORD;
        addr_str = uint32ToStr(addr);
        se->writeMem(addr_str, T_DWORD, data);
    }

    // We think eax register store the return value
    auto ret_v = std::make_shared<BitVector>(ValueType::CONCRETE, ret);
    se->writeReg("eax", ret_v);

    return true;
}

bool LIBC_X86_Malloc::symbolic_execution(tana::SEEngine *se)
{
    assert(!this->is_static);
    uint32_t size, ret;
    ret = this->m_ret_value;
    auto ret_v = std::make_shared<BitVector>(ValueType::CONCRETE, ret);
    se->writeReg("eax", ret_v);
    return true;
}

bool LIBC_X86_Memcpy::symbolic_execution(tana::SEEngine *se)
{
    assert(!this->is_static);
    uint32_t destination, source, num, ret;
    return true;
}

bool LIBC_X86_Mempcpy::symbolic_execution(tana::SEEngine *se)
{
    assert(!this->is_static);
    uint32_t destination, source, num, ret;
    return true;
}

bool LIBC_X86_Memset::symbolic_execution(tana::SEEngine *se)
{
    assert(!this->is_static);
    uint32_t ptr, value, num, ret;
    return true;
}

