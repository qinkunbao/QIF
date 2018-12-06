#include "Register.h"

namespace tana {
    using namespace x86;

    std::string Registers::getRegName(x86_reg reg) {
        return reg_id2string(reg);
    }

    t_type::RegPart Registers::getRegSize(x86_reg name) {
        return x86::get_reg_size(name);
    }

    x86_reg Registers::convert2RegID(std::string reg_name) {
        return reg_string2id(reg_name);
    }

    void Registers::printTaintedRegs(bool taintedRegisters[GPR_NUM][REGISTER_SIZE]) {
        for (int i = 0; i < GPR_NUM; ++i)
            for (int j = 0; j < REGISTER_SIZE; ++j)
                if (taintedRegisters[i][j])
                    std::cout << getRegName((x86_reg) i) << std::endl;
    }

    uint32_t Registers::getRegIndex(x86_reg name) {
        switch (name) {
            case X86_REG_EAX:
            case X86_REG_AX:
            case X86_REG_AH:
            case X86_REG_AL:
                return 0;

            case X86_REG_EBX:
            case X86_REG_BX:
            case X86_REG_BH:
            case X86_REG_BL:
                return 1;

            case X86_REG_ECX:
            case X86_REG_CX:
            case X86_REG_CH:
            case X86_REG_CL:
                return 2;

            case X86_REG_EDX:
            case X86_REG_DX:
            case X86_REG_DH:
            case X86_REG_DL:
                return 3;

            case X86_REG_ESI:
            case X86_REG_SI:
                return 4;

            case X86_REG_EDI:
            case X86_REG_DI:
                return 5;

            case X86_REG_ESP:
            case X86_REG_SP:
                return 6;

            case X86_REG_EBP:
            case X86_REG_BP:
                return 7;

            default:
                std::cout << "Error getRegIndex: " << name << std::endl;
                exit(0);
        }
    }

    std::string Registers::convertRegID2RegName(uint32_t id) {
        switch (id) {
            case 0:
                return "eax";
            case 1:
                return "ebx";
            case 2:
                return "ecx";
            case 3:
                return "edx";
            case 4:
                return "esi";
            case 5:
                return "edi";
            case 6:
                return "esp";
            case 7:
                return "ebp";
            default:
                std::cout << "Error convertRegID2RegName: " << id << std::endl;
                exit(0);
        }
    }

    RegType Registers::getRegType(x86_reg name) {
        switch (name) {
            case X86_REG_EAX:
            case X86_REG_EBX:
            case X86_REG_ECX:
            case X86_REG_EDX:
            case X86_REG_ESI:
            case X86_REG_EDI:
            case X86_REG_ESP:
            case X86_REG_EBP:
                return FULL;

            case X86_REG_AX:
            case X86_REG_BX:
            case X86_REG_CX:
            case X86_REG_DX:
            case X86_REG_SI:
            case X86_REG_DI:
            case X86_REG_BP:
            case X86_REG_SP:
                return HALF;

            case X86_REG_AH:
            case X86_REG_BH:
            case X86_REG_CH:
            case X86_REG_DH:
                return QHIGH;

            case X86_REG_AL:
            case X86_REG_BL:
            case X86_REG_CL:
            case X86_REG_DL:
                return QLOW;

            default:
                std::cout << "Error getRegType: " << name << std::endl;
                exit(0);
        }
    }

    RegType Registers::getRegType(std::string name) {

        auto reg = convert2RegID(name);
        return getRegType(reg);
    }

    uint32_t Registers::getRegMask(x86_reg reg) {
        switch (reg) {
            case X86_REG_EAX:
            case X86_REG_EBX:
            case X86_REG_ECX:
            case X86_REG_EDX:
            case X86_REG_ESI:
            case X86_REG_EDI:
            case X86_REG_ESP:
            case X86_REG_EBP:
                return reg_mask::FULL;

            case X86_REG_AX:
            case X86_REG_BX:
            case X86_REG_CX:
            case X86_REG_DX:
            case X86_REG_SI:
            case X86_REG_DI:
            case X86_REG_BP:
            case X86_REG_SP:
                return reg_mask::HALF;

            case X86_REG_AH:
            case X86_REG_BH:
            case X86_REG_CH:
            case X86_REG_DH:
                return reg_mask::EIGHT_H;

            case X86_REG_AL:
            case X86_REG_BL:
            case X86_REG_CL:
            case X86_REG_DL:
                return reg_mask::EIGHT_L;

            default:
                std::cout << "Error getRegType: " << reg << std::endl;
                exit(0);
        }
    }
}