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
        se->writeMem(addr_str, T_BYTE, data);
    }

    // We think eax register store the return value
    auto ret_v = std::make_shared<BitVector>(ValueType::CONCRETE, ret);
    se->writeReg("eax", ret_v);

    return true;
}

bool LIBC_X86_Malloc::symbolic_execution(tana::SEEngine *se)
{
    assert(!this->is_static);
    uint32_t ret;
    ret = this->m_ret_value;
    auto ret_v = std::make_shared<BitVector>(ValueType::CONCRETE, ret);
    se->writeReg("eax", ret_v);
    return true;
}

bool LIBC_X86_Memcpy::symbolic_execution(tana::SEEngine *se)
{
    assert(!this->is_static);
    uint32_t destination, source, num, ret;
    destination = m_destination;
    source = m_source;
    num = m_num;
    ret = m_ret_value;
    uint32_t index;
    shared_ptr<BitVector> data;
    string source_addr_str, dest_addr_str;
    for(index = 0; index < num; ++index)
    {
        source_addr_str = uint32ToStr(source + index );
        dest_addr_str = uint32ToStr(destination + index);

        data = se->readMem(source_addr_str, T_DWORD);

        se->writeMem(dest_addr_str, T_DWORD, data);
    }

    auto rev_v = std::make_shared<BitVector>(ValueType ::CONCRETE, destination);
    se->writeReg("eax", rev_v);
    return true;
}

bool LIBC_X86_Mempcpy::symbolic_execution(tana::SEEngine *se)
{
    assert(!this->is_static);
    uint32_t destination, source, num, ret;
    destination = m_destination;
    source = m_source;
    num = m_num;
    ret = m_ret_value;
    uint32_t index;
    shared_ptr<BitVector> data;
    string source_addr_str, dest_addr_str;
    for(index = 0; index < num; ++index)
    {
        source_addr_str = uint32ToStr(source + index);
        dest_addr_str = uint32ToStr(destination + index);

        data = se->readMem(source_addr_str, T_DWORD);

        se->writeMem(dest_addr_str, T_DWORD, data);
    }

    auto rev_v = std::make_shared<BitVector>(ValueType ::CONCRETE, destination + index * T_DWORD);
    se->writeReg("eax", rev_v);
    return true;
}

bool LIBC_X86_Memset::symbolic_execution(tana::SEEngine *se)
{
    assert(!this->is_static);
    uint32_t ptr, value, num, ret;
    ptr = m_ptr;
    value = m_value;
    num = m_num;
    ret = m_ret_value;

    uint32_t index;
    string addr_str;
    shared_ptr<BitVector> data;
    for(index = 0; index < num; ++index)
    {
        data = make_shared<BitVector>(ValueType::CONCRETE, value);
        addr_str = uint32ToStr(ptr + index);
        se->writeMem(addr_str, T_BYTE_SIZE, data);
    }

    auto rev_v = std::make_shared<BitVector>(ValueType ::CONCRETE, ret);
    se->writeReg("eax", rev_v);

    return true;
}

