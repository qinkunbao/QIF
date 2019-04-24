#pragma once

#include <string>
#include <map>
#include <iostream>

#include "ins_types.h"
#include "x86.h"

namespace tana {

    enum RegType {
        FULL = 0,    //eax, ebx, ecx, edx, esi, edi, esp, ebp
        HALF = 1,    //ax, bx, cx, dx
        QHIGH = 2,   //ah, bh, ch, dh
        QLOW = 3,     //al, bl, cl, dl
        INVALIDREG
    };

    class Registers {
    public:
        static std::string getRegName(x86::x86_reg reg);

        static std::string convertRegID2RegName(uint32_t id);

        static uint32_t getRegSize(x86::x86_reg name);

        static uint32_t getRegSize(std::string name);

        static x86::x86_reg convert2RegID(std::string reg_name);

        static void printTaintedRegs(bool taintedRegisters[GPR_NUM][REGISTER_SIZE]);

        static uint32_t getRegIndex(x86::x86_reg name);

        static RegType getRegType(x86::x86_reg reg);

        static RegType getRegType(std::string reg);

    };

}