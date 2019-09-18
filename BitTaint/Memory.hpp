#pragma once
#include <vector>
#include <map>
#include <string>
#include "x86.hpp"


namespace tana {
    namespace bittaint {


        const static int BYTESIZE = 8;
        const static int REGISTER_SIZE = 32;
        const static unsigned REG_BYTE = REGISTER_SIZE / BYTESIZE;
        using MemoryAddress = uint32_t;
        using BitMap = std::vector<int>;
        using InstructionMap = std::vector<int>;

        std::vector<int> vector_union(const std::vector<int> &a, const std::vector<int> &b);

        class Byte {
        private:
            std::vector<BitMap> ByteMap;
        public:
            Byte();

            const std::vector<BitMap> &readbyte() const;

            void writebyte(std::vector<BitMap> map);

            BitMap readbit(int bit_index) const;

            void writebit(BitMap &bm, uint32_t bit_index);

            bool istainted() const;

            std::vector<int> get_tainted_flags() const;

        };


        class Memory {
        private:
            std::map<MemoryAddress, Byte> data_memory;
        public:
            Memory(MemoryAddress addr, uint32_t);

            std::vector<Byte> read_data(MemoryAddress addr, uint32_t size) const;

            void write_data(MemoryAddress addr, std::vector<Byte> byte_map);

            bool istainted(MemoryAddress addr, uint32_t size) const;

            std::vector<int> get_tainted_bit(MemoryAddress addr, uint32_t size) const;

        };

        class Register {
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
            BitMap OF, SF, ZF, AF, CF, PF;
        public:
            Register();

            std::vector<Byte> read_register(tana::x86::x86_reg reg_id) const;

            static tana::x86::x86_reg str2id(std::string reg_str);

            static std::string id2str(tana::x86::x86_reg reg_id);

            void write_register(tana::x86::x86_reg reg_id, std::vector<Byte>);

            bool istainted(tana::x86::x86_reg reg) const;

            std::vector<int> get_tainted_bit(tana::x86::x86_reg reg_id) const;
        };

    }
}
