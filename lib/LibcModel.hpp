/*************************************************************************
	> File Name: LibcModel.hpp
	> Author: Qinkun Bao
	> Mail: qinkunbao@gmail.com
	> Created Time: Sun Sep 22 16:30:10 EDT 2019
 ************************************************************************/

#pragma once

#include "ins_semantics.hpp"
#include "ins_types.hpp"


namespace tana{

    class LIBC_X86_CALLOC : public Inst_Base{
    private:
        uint32_t m_num, m_size;
        uint32_t m_ret_value;

    public:
        LIBC_X86_CALLOC(uint32_t num, uint32_t size, uint32_t ret_value) : Inst_Base(false)
        {
            m_num = num;
            m_size = size;
            m_ret_value = ret_value;
        }

    };

    class LIBC_X86_MALLOC : public Inst_Base{
    private:
        uint32_t m_size;
        uint32_t m_ret_value;

    public:
        LIBC_X86_MALLOC(uint32_t size, uint32_t ret_value) : Inst_Base(false)
        {
            m_size = size;
            m_ret_value = ret_value;
        }

    };

    class LIBC_X86_memcpy : public Inst_Base{
    private:
        uint32_t m_destination, m_source, m_num;
        uint32_t m_ret_value;

    };

    class LIBC_X86_mempcpy : public Inst_Base{
    private:
        uint32_t m_destination, m_source, m_num;
        uint32_t m_ret_value;

    };

    class LIBC_X86_memset : public Inst_Base{
    private:
        uint32_t m_ptr, m_value, m_num;
        uint32_t m_ret_value;

    };


}