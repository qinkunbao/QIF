#pragma once
#include <vector>
#include <map>
#include <string>
#include "x86.h"


namespace bittaint {


    const static int BYTESIZE = 8;
    const static int REGISTER_SIZE = 32;
    const static unsigned REG_BYTE = REGISTER_SIZE / BYTESIZE;
    using MemoryAddress = uint32_t ;
    using BitMap = std::vector<int>;
    using InstructionMap = std::vector<int>;

    class Byte {
    private:
        std::vector<BitMap> ByteMap;
    public:
        Byte();
        const std::vector<BitMap>& readbyte() const ;
        void writebyte(std::vector<BitMap> map);
        const BitMap readbit(int bit_index) const ;
        void writebit(BitMap& bm, int bit_index);

    };


    class Memory {
    private:
        std::map<MemoryAddress, Byte> data_memory;
        std::map<MemoryAddress, InstructionMap> code_memory;
    public:
        Memory(MemoryAddress addr, uint32_t);
        const std::vector<Byte> read_data(MemoryAddress addr, uint32_t size) const;
        void write_data(MemoryAddress addr, std::vector<Byte> byte_map);
        void taint_code(MemoryAddress addr, InstructionMap map);
        bool istainted(MemoryAddress addr, uint32_t size) const;

    };

    class Register{
    private:
        //Stupid, but easy to Debug!
        std::vector<Byte> eax;
        std::vector<Byte> ebx;
        std::vector<Byte> ecx;
        std::vector<Byte> edx;
        std::vector<Byte> esi;
        std::vector<Byte> edi;
        std::vector<Byte> ebp;
        std::vector<Byte> esp;
    public:
        Register();
        std::vector<Byte> read_register(tana::x86::x86_reg reg_id) const ;
        static tana::x86::x86_reg str2id(std::string reg_str);
        static std::string id2str(tana::x86::x86_reg reg_id);
        void write_register(tana::x86::x86_reg reg_id, std::vector<Byte>);
        bool istainted(tana::x86::x86_reg reg);
    };

}

