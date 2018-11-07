#include <cassert>
#include <iostream>
#include "Memory.h"


namespace bittaint {

    Byte::Byte():ByteMap (BYTESIZE)
    {

    }

    const std::vector<BitMap> &Byte::readbyte() const {
        return ByteMap;
    }

    void Byte::writebyte(std::vector<BitMap> map) {
        assert(map.size() == BYTESIZE);
        for (int index = 0; index < BYTESIZE; ++index) {
            ByteMap[index] = map[index];
        }
    }

    const BitMap Byte::readbit(int bit_index) const {
        return ByteMap[bit_index];
    }

    void Byte::writebit(BitMap &bm, int bit_index) {
        ByteMap[bit_index] = bm;
    }


    Memory::Memory(bittaint::MemoryAddress addr,  uint32_t size)
    {
        for(int i = 0; i < size; ++i)
        {
            Byte byte;
            for (int j = 0; j < BYTESIZE; ++j)
            {
                BitMap bit;
                bit.push_back(i * BYTESIZE + j);
                byte.writebit(bit, j);
            }
            data_memory.insert(std::make_pair(addr + i, byte));
        }
    }

    const std::vector<Byte> Memory::read_data(MemoryAddress addr, uint32_t size) const{
        std::vector<Byte> data(size);
        for (uint32_t i = 0; i < size; ++i) {
            auto memory_res = data_memory.find(addr + i);
            if (memory_res != data_memory.end())
            {
                Byte byte_res = (memory_res->second);
                data.push_back(byte_res);
            }
            else {
                Byte empty_byte;
                data.push_back(empty_byte);
            }
        }
        return data;
    }

    void Memory::write_data(MemoryAddress addr, std::vector<Byte> byte_map) {
        uint32_t size = byte_map.size();
        for(uint32_t i = 0; i < size; ++i)
        {
            data_memory[i + addr] = byte_map[i];
        }

    }

    void Memory::taint_code(bittaint::MemoryAddress addr, bittaint::InstructionMap map) {
        code_memory.insert(std::make_pair(addr, map));
    }

    bool Memory::istainted(bittaint::MemoryAddress addr, uint32_t size) const
    {
        bool is_tainted = false;
        for (uint32_t i = 0; i < size; ++i) {
                auto memory_res = data_memory.find(addr + i);
                if (memory_res != data_memory.end())
                {
                    Byte byte_res = (memory_res->second);
                    std::vector<BitMap>  bitmap = byte_res.readbyte();
                    for(auto &it : bitmap)
                    {
                        if(!it.empty())
                            is_tainted = true;
                    }
                }

        }

        return is_tainted;
    }

    Register::Register() : eax(REG_BYTE), ebx(REG_BYTE), ecx(REG_BYTE), edx(REG_BYTE), \
                           edi(REG_BYTE), esi(REG_BYTE), ebp(REG_BYTE), esp(REG_BYTE)
    {}

    std::vector<Byte> Register::read_register(tana::x86::x86_reg reg_id) const {
        std::vector<Byte> ret_buffer;
        switch (reg_id) {
            case tana::x86::X86_REG_AL: {
                Byte data = eax[0];
                ret_buffer.push_back(data);
            }
                break;

            case tana::x86::X86_REG_AH: {
                Byte data = eax[1];
                ret_buffer.push_back(data);
            }
                break;

            case tana::x86::X86_REG_AX: {
                Byte data1 = eax[0];
                Byte data2 = eax[1];
                ret_buffer.push_back(data1);
                ret_buffer.push_back(data2);
            }
                break;

            case tana::x86::X86_REG_EAX:{
                Byte data1 = eax[0];
                Byte data2 = eax[1];
                Byte data3 = eax[2];
                Byte data4 = eax[3];
                ret_buffer.push_back(data1);
                ret_buffer.push_back(data2);
                ret_buffer.push_back(data3);
                ret_buffer.push_back(data4);
            }
                break;
            case tana::x86::X86_REG_BL:
            {
                Byte data = ebx[0];
                ret_buffer.push_back(data);
            }
                break;
            case tana::x86::X86_REG_BH:
            {
                Byte data = ebx[1];
                ret_buffer.push_back(data);
            }
                break;
            case tana::x86::X86_REG_BX:
            {
                Byte data1 = ebx[0];
                Byte data2 = ebx[1];
                ret_buffer.push_back(data1);
                ret_buffer.push_back(data2);
            }
                break;
            case tana::x86::X86_REG_EBX:
            {
                Byte data1 = ebx[0];
                Byte data2 = ebx[1];
                Byte data3 = ebx[2];
                Byte data4 = ebx[3];
                ret_buffer.push_back(data1);
                ret_buffer.push_back(data2);
                ret_buffer.push_back(data3);
                ret_buffer.push_back(data4);
            }
                break;

            case tana::x86::X86_REG_CL:
            {
                Byte data = ecx[0];
                ret_buffer.push_back(data);
            }
                break;
            case tana::x86::X86_REG_CH:
            {
                Byte data = ecx[1];
                ret_buffer.push_back(data);
            }
                break;
            case tana::x86::X86_REG_CX:
            {
                Byte data1 = ecx[0];
                Byte data2 = ecx[1];
                ret_buffer.push_back(data1);
                ret_buffer.push_back(data2);
            }
                break;
            case tana::x86::X86_REG_ECX:
            {
                Byte data1 = ecx[0];
                Byte data2 = ecx[1];
                Byte data3 = ecx[2];
                Byte data4 = ecx[3];
                ret_buffer.push_back(data1);
                ret_buffer.push_back(data2);
                ret_buffer.push_back(data3);
                ret_buffer.push_back(data4);
            }
                break;

            case tana::x86::X86_REG_DL:
            {
                Byte data = edx[0];
                ret_buffer.push_back(data);
            }
                break;
            case tana::x86::X86_REG_DH:
            {
                Byte data = edx[1];
                ret_buffer.push_back(data);
            }
                break;

            case tana::x86::X86_REG_DX:
            {
                Byte data1 = edx[0];
                Byte data2 = edx[1];
                ret_buffer.push_back(data1);
                ret_buffer.push_back(data2);
            }
                break;

            case tana::x86::X86_REG_EDX:
            {
                Byte data1 = edx[0];
                Byte data2 = edx[1];
                Byte data3 = edx[2];
                Byte data4 = edx[3];
                ret_buffer.push_back(data1);
                ret_buffer.push_back(data2);
                ret_buffer.push_back(data3);
                ret_buffer.push_back(data4);
            }
                break;

            case tana::x86::X86_REG_SI:
            {
                Byte data1 = esi[0];
                Byte data2 = esi[1];
                ret_buffer.push_back(data1);
                ret_buffer.push_back(data2);
            }
                break;

            case tana::x86::X86_REG_ESI:
            {
                Byte data1 = esi[0];
                Byte data2 = esi[1];
                Byte data3 = esi[2];
                Byte data4 = esi[3];
                ret_buffer.push_back(data1);
                ret_buffer.push_back(data2);
                ret_buffer.push_back(data3);
                ret_buffer.push_back(data4);
            }
                break;

            case tana::x86::X86_REG_DI:
            {
                Byte data1 = edi[0];
                Byte data2 = edi[1];
                ret_buffer.push_back(data1);
                ret_buffer.push_back(data2);
            }
                break;

            case tana::x86::X86_REG_EDI:
            {
                Byte data1 = edi[0];
                Byte data2 = edi[1];
                Byte data3 = edi[2];
                Byte data4 = edi[3];
                ret_buffer.push_back(data1);
                ret_buffer.push_back(data2);
                ret_buffer.push_back(data3);
                ret_buffer.push_back(data4);
            }
                break;

            case tana::x86::X86_REG_BP:
            {
                Byte data1 = ebp[0];
                Byte data2 = ebp[1];
                ret_buffer.push_back(data1);
                ret_buffer.push_back(data2);
            }
                break;

            case tana::x86::X86_REG_EBP:
            {
                Byte data1 = ebp[0];
                Byte data2 = ebp[1];
                Byte data3 = ebp[2];
                Byte data4 = ebp[3];
                ret_buffer.push_back(data1);
                ret_buffer.push_back(data2);
                ret_buffer.push_back(data3);
                ret_buffer.push_back(data4);
            }
                break;


            case tana::x86::X86_REG_SP:
            {
                Byte data1 = esp[0];
                Byte data2 = esp[1];
                ret_buffer.push_back(data1);
                ret_buffer.push_back(data2);
            }
                break;

            case tana::x86::X86_REG_ESP:
            {
                Byte data1 = esp[0];
                Byte data2 = esp[1];
                Byte data3 = esp[2];
                Byte data4 = esp[3];
                ret_buffer.push_back(data1);
                ret_buffer.push_back(data2);
                ret_buffer.push_back(data3);
                ret_buffer.push_back(data4);
            }
                break;

            default:
                std::cout << "register " << __FILE__ << ": " << __LINE__ << std::endl;
                exit(0);


        }

        return ret_buffer;
    }

    tana::x86::x86_reg Register::str2id(std::string reg_str) {
        return tana::x86::reg_string2id(reg_str);
    }

    void Register::write_register(tana::x86::x86_reg reg_id, std::vector<bittaint::Byte> bytes) {
        uint32_t reg_size = tana::x86::get_reg_size(reg_id);
        assert(reg_size == bytes.size());
        switch (reg_id)
        {
            case tana::x86::X86_REG_AL: {
                eax[0] = bytes[0];
            }
                break;

            case tana::x86::X86_REG_AH: {
                eax[1] = bytes[0];
            }
                break;

            case tana::x86::X86_REG_AX: {
                eax[0] = bytes[0];
                eax[1] = bytes[1];
            }
                break;

            case tana::x86::X86_REG_EAX:{
                eax[0] = bytes[0];
                eax[1] = bytes[1];
                eax[2] = bytes[2];
                eax[3] = bytes[3];
            }
                break;
            case tana::x86::X86_REG_BL:
            {
                ebx[0] = bytes[0];

            }
                break;
            case tana::x86::X86_REG_BH:
            {
                ebx[1] = bytes[0];
            }
                break;
            case tana::x86::X86_REG_BX:
            {
                ebx[0] = bytes[0];
                ebx[1] = bytes[1];
            }
                break;
            case tana::x86::X86_REG_EBX:
            {
                ebx[0] = bytes[0];
                ebx[1] = bytes[1];
                ebx[2] = bytes[2];
                ebx[3] = bytes[3];
            }
                break;

            case tana::x86::X86_REG_CL:
            {
                ecx[0] = bytes[0];
            }
                break;
            case tana::x86::X86_REG_CH:
            {
                ecx[1] = bytes[0];
            }
                break;
            case tana::x86::X86_REG_CX:
            {
                ecx[0] = bytes[0];
                ecx[1] = bytes[1];

            }
                break;
            case tana::x86::X86_REG_ECX:
            {
                ecx[0] = bytes[0];
                ecx[1] = bytes[1];
                ecx[2] = bytes[2];
                ecx[3] = bytes[3];
            }
                break;

            case tana::x86::X86_REG_DL:
            {
                edx[0] = bytes[0];
            }
                break;
            case tana::x86::X86_REG_DH:
            {
                edx[1] = bytes[0];
            }
                break;

            case tana::x86::X86_REG_DX:
            {
                edx[0] = bytes[0];
                edx[1] = bytes[1];
            }
                break;

            case tana::x86::X86_REG_EDX:
            {
                edx[0] = bytes[0];
                edx[1] = bytes[1];
                edx[2] = bytes[2];
                edx[3] = bytes[3];
            }
                break;

            case tana::x86::X86_REG_SI:
            {
                esi[0] = bytes[0];
                esi[1] = bytes[1];
            }
                break;

            case tana::x86::X86_REG_ESI:
            {
                esi[0] = bytes[0];
                esi[1] = bytes[1];
                esi[2] = bytes[2];
                esi[3] = bytes[3];
            }
                break;

            case tana::x86::X86_REG_DI:
            {
                edi[0] = bytes[0];
                edi[1] = bytes[1];
            }
                break;

            case tana::x86::X86_REG_EDI:
            {
                edi[0] = bytes[0];
                edi[1] = bytes[1];
                edi[2] = bytes[2];
                edi[3] = bytes[3];
            }
                break;

            case tana::x86::X86_REG_BP:
            {
                ebp[0] = bytes[0];
                ebp[1] = bytes[1];
            }
                break;

            case tana::x86::X86_REG_EBP:
            {
                ebp[0] = bytes[0];
                ebp[1] = bytes[1];
                ebp[2] = bytes[2];
                ebp[3] = bytes[3];
            }
                break;


            case tana::x86::X86_REG_SP:
            {
                esp[0] = bytes[0];
                esp[1] = bytes[1];
            }
                break;

            case tana::x86::X86_REG_ESP:
            {
                esp[0] = bytes[0];
                esp[1] = bytes[1];
                esp[2] = bytes[2];
                esp[3] = bytes[3];
            }
                break;
            default:
                exit(0);

        }


    }

    std::string Register::id2str(tana::x86::x86_reg reg_id) {
        return tana::x86::reg_id2string(reg_id);
    }


}
