#include <cassert>
#include <algorithm>
#include <limits.h>
#include <sstream>
#include "ins_types.h"
#include "SEEngine.h"
#include "VarMap.h"
#include "error.h"

#define ERROR(MESSAGE) tana::default_error_handler(__FILE__, __LINE__, MESSAGE)


namespace tana {

// part of code from https://github.com/s3team/CryptoHunt

    bool SEEngine::isImmSym(uint32_t num) {
        if (imm2sym == false) {
            return false;
        }
        if (num < MAX_IMM_NUMBER) {
            return false;
        }
        std::vector<uint32_t> common = {0xff, 0xfff, 0xffff, 0xfffff, 0xffffff, 0xffffffff, 0xff00, 0xffff0000};
        auto find_result = std::find(common.begin(), common.end(), num);
        if (find_result != common.end()) {
            return false;
        }

        return true;

    }

    Operation::Operation(std::string opt, std::shared_ptr<Value> v1) {
        opty = opt;
        val[0] = v1;
        val[1] = nullptr;
        val[2] = nullptr;
    }

    Operation::Operation(std::string opt, std::shared_ptr<Value> v1, std::shared_ptr<Value> v2) {
        opty = opt;
        val[0] = v1;
        val[1] = v2;
        val[2] = nullptr;
    }

    Operation::Operation(std::string opt, std::shared_ptr<Value> v1, std::shared_ptr<Value> v2,
                         std::shared_ptr<Value> v3) {
        opty = opt;
        val[0] = v1;
        val[1] = v2;
        val[2] = v3;
    }

    int Value::idseed = 0;

    Value::Value(ValueTy vty) : opr(nullptr) {
        id = ++idseed;
        valty = vty;
    }

    Value::Value(ValueTy vty, std::string con) : opr(nullptr) {
        id = ++idseed;
        valty = vty;
        conval = con;
    }

    Value::Value(ValueTy vty, uint32_t con, SEEngine *se) : opr(nullptr) {
        std::stringstream ss;
        if (se->isImmSym(con)) {
            id = ++idseed;
            valty = SYMBOL;
            ss << "0x" << std::hex << con << std::dec;
            conval = ss.str();
        } else {
            id = ++idseed;
            valty = vty;
            ss << "0x" << std::hex << con << std::dec;
            conval = ss.str();
        }
    }

    Value::Value(ValueTy vty, uint32_t con) : opr(nullptr) {
        std::stringstream ss;
        id = ++idseed;
        valty = vty;
        ss << "0x" << std::hex << con << std::dec;
        conval = ss.str();

    }

    Value::Value(ValueTy vty, std::unique_ptr<Operation> oper) {
        id = ++idseed;
        valty = vty;
        opr = std::move(oper);
    }


    bool Value::isSymbol() {
        if (this->valty == SYMBOL)
            return true;
        else
            return false;
    }


    bool Value::operator==(const Value &v1) {
        const std::unique_ptr<Operation> &opr1 = opr;
        const std::unique_ptr<Operation> &opr2 = v1.opr;
        if ((opr == nullptr) && (v1.opr == nullptr)) {
            if (valty != v1.valty)
                return false;
            if (valty == SYMBOL)
                return (id == v1.id);
            if (valty == CONCRETE)
                return (conval) == (v1.conval);
        }
        if ((opr1 != nullptr) && (opr2 != nullptr)) {
            std::shared_ptr<Value> l1 = nullptr, l2 = nullptr, l3 = nullptr, r1 = nullptr, r2 = nullptr, r3 = nullptr;
            uint32_t num_l = 0;
            uint32_t num_r = 0;
            if (opr1->opty != opr2->opty)
                return false;
            if (opr1->val[0] != nullptr) {
                ++num_l;
                l1 = opr1->val[0];
            }
            if (opr1->val[1] != nullptr) {
                ++num_l;
                l2 = opr1->val[1];
            }
            if (opr1->val[2] != nullptr) {
                ++num_l;
                l3 = opr1->val[2];
            }
            if (opr2->val[0] != nullptr) {
                ++num_r;
                r1 = opr2->val[0];
            }
            if (opr2->val[1] != nullptr) {
                ++num_r;
                r2 = opr2->val[1];
            }
            if (opr2->val[2] != nullptr) {
                ++num_r;
                r3 = opr2->val[2];
            }
            if (num_l != num_r)
                return false;
            if (num_l == 1)
                return (*l1) == (*r1);
            if (num_l == 2)
                return ((*l1) == (*r1)) && ((*l2) == (*r2));
            if (num_l == 3)
                return ((*l1) == (*r1)) && ((*l2) == (*r2)) && ((*l3) == (*r3));
        }
        return false;
    }


    uint32_t SEEngine::arithmeticRightShift(uint32_t op1, uint32_t op2) {
        int32_t temp1 = op1;
        int32_t temp2 = op2;
        int32_t result = temp1 >> temp2;
        return uint32_t(result);
    }

    uint32_t SEEngine::rol32(uint32_t op1, uint32_t op2) {
        const unsigned int mask = (CHAR_BIT * sizeof(op1) - 1);  // assumes width is a power of 2.
        // assert ( (c<=mask) &&"rotate by type width or more");
        op2 &= mask;
        return (op1 << op2) | (op1 >> ((-op2) & mask));

    }

    uint32_t SEEngine::ror32(uint32_t op1, uint32_t op2) {
        const unsigned int mask = (CHAR_BIT * sizeof(op1) - 1);

        // assert ( (c<=mask) &&"rotate by type width or more");
        op2 &= mask;
        return (op1 >> op2) | (op1 << ((-op2) & mask));

    }

    uint32_t SEEngine::shld32(uint32_t op1, uint32_t op2, uint32_t op3) {
        uint32_t cnt;
        uint32_t res;
        assert(op3 < REGISTER_SIZE);
        cnt = op3 % REGISTER_SIZE;
        res = (op1 << cnt) | (op2 >> (REGISTER_SIZE - cnt));
        return res;
    }

    uint32_t SEEngine::shrd32(uint32_t op1, uint32_t op2, uint32_t op3) {
        uint32_t cnt;
        uint32_t res;
        assert(op3 < REGISTER_SIZE);
        cnt = op3 % REGISTER_SIZE;
        res = (op1 >> cnt) | (op2 << (REGISTER_SIZE - cnt));
        return res;
    }

    bool SEEngine::isRegSame(Inst& instruction1, Inst& instruction2) {
        vcpu_ctx inst1 = instruction1.vcpu;
        vcpu_ctx inst2 = instruction2.vcpu;
        for (uint32_t i = 0; i < GPR_NUM; ++i) {
            if (inst1.gpr[i] != inst2.gpr[i])
                return false;
        }
        return true;
    }

    SEEngine::SEEngine(bool state_type) {
        ctx = {{"eax", nullptr},
               {"ebx", nullptr},
               {"ecx", nullptr},
               {"edx", nullptr},
               {"esi", nullptr},
               {"edi", nullptr},
               {"esp", nullptr},
               {"ebp", nullptr}
        };
        imm2sym = state_type;
    }

    void SEEngine::getFormulaLength(std::shared_ptr<Value> v, uint32_t &len) {
        const std::unique_ptr<Operation> &op = v->opr;
        ++len;
        if (op == nullptr) {
            return;
        }

        if (op->val[0] != nullptr)
            getFormulaLength(op->val[0], len);
        if (op->val[1] != nullptr)
            getFormulaLength(op->val[1], len);
        if (op->val[2] != nullptr)
            getFormulaLength(op->val[2], len);
    }


    std::shared_ptr<Value>
    buildop1(std::string opty, std::shared_ptr<Value> v1) {
        std::unique_ptr<Operation> oper = std::make_unique<Operation>(opty, v1);
        std::shared_ptr<Value> result;
        if (v1->isSymbol())
            result = std::make_shared<Value>(SYMBOL, std::move(oper));
        else {
            result = std::make_shared<Value>(CONCRETE, std::move(oper));
        }
        return result;
    }

    std::shared_ptr<Value>
    buildop2(std::string opty, std::shared_ptr<Value> v1, std::shared_ptr<Value> v2) {
        std::unique_ptr<Operation> oper = std::make_unique<Operation>(opty, v1, v2);
        std::shared_ptr<Value> result;

        if (v1->isSymbol() || v2->isSymbol())
            result = std::make_shared<Value>(SYMBOL, std::move(oper));
        else
            result = std::make_shared<Value>(CONCRETE, std::move(oper));

        return result;
    }

    std::shared_ptr<Value>
    buildop3(std::string opty, std::shared_ptr<Value> v1, std::shared_ptr<Value> v2, std::shared_ptr<Value> v3) {
        std::unique_ptr<Operation> oper = std::make_unique<Operation>(opty, v1, v2, v3);
        std::shared_ptr<Value> result;

        if (v1->isSymbol() || v2->isSymbol() || v3->isSymbol())
            result = std::make_shared<Value>(SYMBOL, std::move(oper));
        else
            result = std::make_shared<Value>(CONCRETE, std::move(oper));

        return result;
    }

    std::shared_ptr<Value> SEEngine::getRegister(x86::x86_reg reg) {
        RegType type = Registers::getRegType(reg);
        switch (type) {
            case FULL: {
                auto index = Registers::getRegIndex(reg);
                std::string strName = Registers::convertRegID2RegName(index);
                return ctx[strName];
            }
            case QLOW:
            case HALF: {
                auto index = Registers::getRegIndex(reg);
                std::string strName = Registers::convertRegID2RegName(index);
                std::shared_ptr<Value> origin = ctx[strName];

                uint32_t mask = Registers::getRegMask(reg);
                std::shared_ptr<Value> temp_mask = std::make_shared<Value>(CONCRETE, mask);
                std::shared_ptr<Value> v1 = buildop2("and", origin, temp_mask);
                return v1;
            }
            case QHIGH: {
                auto index = Registers::getRegIndex(reg);
                std::string strName = Registers::convertRegID2RegName(index);
                std::shared_ptr<Value> origin = ctx[strName];

                uint32_t mask = Registers::getRegMask(reg);
                std::shared_ptr<Value> temp_mask = std::make_shared<Value>(CONCRETE, mask);
                std::shared_ptr<Value> v1 = buildop2("and", origin, temp_mask);

                std::shared_ptr<Value> temp_imm = std::make_shared<Value>(CONCRETE, 0x8);
                std::shared_ptr<Value> v_shift = buildop2("shr", v1, temp_imm);
                return v_shift;
            }
        }
    }

    std::shared_ptr<Value> SEEngine::getMemory(t_type::T_ADDRESS memory_address, t_type::T_SIZE size) {
        std::shared_ptr<Value> v0;
        if (memory_find(memory_address)) {
            v0 = memory[memory_address];
        } else {
            v0 = std::make_shared<Value>(SYMBOL);
            memory[memory_address] = v0;
        }
        if (size == T_BYTE_SIZE * T_DWORD) {
            return v0;
        }

        if (size == T_BYTE_SIZE * T_WORD) {
            std::shared_ptr<Value> temp_mask = std::make_shared<Value>(CONCRETE, 0x0000ffff);
            std::shared_ptr<Value> v1 = buildop2("and", v0, temp_mask);
            return v1;
        }

        std::shared_ptr<Value> temp_mask = std::make_shared<Value>(CONCRETE, 0x000000ff);
        std::shared_ptr<Value> v1 = buildop2("and", v0, temp_mask);
        return v1;
    }

    std::shared_ptr<Value>
    SEEngine::Concat(std::shared_ptr<Value> v1, std::shared_ptr<Value> v2) {
        std::shared_ptr<Value> low = nullptr, high = nullptr, res = nullptr;
        assert((v1->low_bit == (v2->high_bit - 1)) || (v2->low_bit == (v1->high_bit - 1)));
        if (v1->low_bit == (v2->high_bit - 1)) {
            high = v1;
            low = v2;
        }
        if (v2->low_bit == (v1->high_bit - 1)) {
            high = v2;
            low = v1;
        }
        res = buildop2("bvconcat", high, low);
        return res;
    }

    std::shared_ptr<Value>
    SEEngine::Extract(std::shared_ptr<Value> v, int low, int high) {
        assert(high > low);
        std::shared_ptr<Value> res = buildop1("bvextract", v);
        res->high_bit = high;
        res->low_bit = low;
        return res;
    }

    void
    SEEngine::init(std::shared_ptr<Value> v1, std::shared_ptr<Value> v2,
                   std::shared_ptr<Value> v3, std::shared_ptr<Value> v4,
                   std::shared_ptr<Value> v5, std::shared_ptr<Value> v6,
                   std::shared_ptr<Value> v7, std::shared_ptr<Value> v8,
                   std::vector<Inst>::iterator it1,
                   std::vector<Inst>::iterator it2) {
        ctx["eax"] = v1;
        ctx["ebx"] = v2;
        ctx["ecx"] = v3;
        ctx["edx"] = v4;
        ctx["esi"] = v5;
        ctx["edi"] = v6;
        ctx["esp"] = v7;
        ctx["ebp"] = v8;

        this->start = it1;
        this->end = it2;
    }

    std::string
    SEEngine::getRegName(std::string regName) {
        auto reg = Registers::convert2RegID(regName);
        uint32_t index = Registers::getRegIndex(reg);
        std::string strName = Registers::convertRegID2RegName(index);
        return strName;
    }

    void
    SEEngine::init(std::vector<Inst>::iterator it1,
                   std::vector<Inst>::iterator it2) {
        this->start = it1;
        this->end = it2;
    }

    void
    SEEngine::initAllRegSymol(std::vector<Inst>::iterator it1,
                              std::vector<Inst>::iterator it2) {
        ctx["eax"] = std::make_shared<Value>(SYMBOL);
        ctx["ebx"] = std::make_shared<Value>(SYMBOL);
        ctx["ecx"] = std::make_shared<Value>(SYMBOL);
        ctx["edx"] = std::make_shared<Value>(SYMBOL);
        ctx["esi"] = std::make_shared<Value>(SYMBOL);
        ctx["edi"] = std::make_shared<Value>(SYMBOL);
        ctx["esp"] = std::make_shared<Value>(SYMBOL);
        ctx["ebp"] = std::make_shared<Value>(SYMBOL);

        this->start = it1;
        this->end = it2;
    }


    int
    SEEngine::symexec() {

        for (auto it = start; it != end; ++it) {
            // cout << hex << it->addrn << ": ";
            // cout << it->opcstr << '\n';
            // skip no effect instructions
            uint32_t oprnum = it->get_operand_number();

            if (x86::SymbolicExecutionNoEffect(it->instruction_id)) continue;

            if (oprnum == 0) continue;

            auto opcode_id = it->instruction_id;
            auto opcstr = x86::insn_id2string(opcode_id);

            if (oprnum == 1) {
                std::shared_ptr<Operand> op0 = it->oprd[0];
                std::shared_ptr<Value> v0, res;
                switch (opcode_id) {
                    case x86::X86_INS_DIV: {
                        auto eaxID = Registers::convert2RegID("eax");
                        auto edxID = Registers::convert2RegID("edx");
                        if (op0->type == Operand::Reg) {

                        }
                        if (op0->type == Operand::Mem) {

                        }
                        // TODO
                    }
                        break;
                    case x86::X86_INS_PUSH: {
                        if (op0->type == Operand::ImmValue) {
                            uint32_t temp_concrete = stoul(op0->field[0], 0, 16);
                            v0 = std::make_shared<Value>(CONCRETE, temp_concrete, this);
                            memory[it->memory_address] = v0;
                        } else if (op0->type == Operand::Reg) {
                            assert(Registers::getRegType(op0->field[0]) == FULL);
                            memory[it->memory_address] = ctx[getRegName(op0->field[0])];
                        } else if (op0->type == Operand::Mem) {
                            // The memaddr in the trace is the read address
                            // We need to compute the write address
                            auto reg = Registers::convert2RegID("esp");
                            uint32_t esp_index = Registers::getRegIndex(reg);
                            uint32_t esp_value = it->vcpu.gpr[esp_index];
                            v0 = getMemory(it->memory_address, op0->bit);
                            memory[esp_value - 4] = v0;
                        } else {
                            ERROR("push error: the operand is not Imm, Reg or Mem!");
                        }
                    }
                        break;
                    case x86::X86_INS_POP: {
                        if (op0->type == Operand::Reg) {
                            assert(Registers::getRegType(op0->field[0]) == FULL);
                            v0 = getMemory(it->memory_address, op0->bit);
                            ctx[getRegName(op0->field[0])] = v0;
                        } else {
                            ERROR("pop error: the operand is not Reg!");
                        }
                    }
                        break;
                    case x86::X86_INS_NEG:
                    case x86::X86_INS_NOT: {
                        if (op0->type == Operand::Reg) {
                            assert(Registers::getRegType(op0->field[0]) == FULL);
                            v0 = ctx[getRegName(op0->field[0])];
                            res = buildop1(opcstr, v0);
                            ctx[getRegName(op0->field[0])] = res;
                        } else if (op0->type == Operand::Mem) {
                            ERROR("neg error: the operand is not Reg!");
                        }
                    }
                        break;
                    case x86::X86_INS_INC:
                    case x86::X86_INS_DEC: {
                        if (op0->type == Operand::Reg) {
                            v0 = ctx[getRegName(op0->field[0])];
                        }

                        if (op0->type == Operand::Mem) {
                            v0 = getMemory(it->memory_address, op0->bit);
                        }
                        res = buildop1(opcstr, v0);

                        if (op0->type == Operand::Reg) {
                            ctx[getRegName(op0->field[0])] = res;
                        }

                        if (op0->type == Operand::Mem) {
                            memory[it->memory_address] = res;
                        }
                    }
                        break;

                    default:
                        std::string err_msg = "instruction " + it->get_opcode_operand() + " is not handled!";
                        ERROR(err_msg.c_str());

                }
            }

            if (oprnum == 2) {
                std::shared_ptr<Operand> op0 = it->oprd[0];
                std::shared_ptr<Operand> op1 = it->oprd[1];
                std::shared_ptr<Value> v0, v1, res;
                switch (opcode_id) {
                    case x86::X86_INS_MOVSX:
                    case x86::X86_INS_MOVZX: {   // Hack way
                        assert(op0->type == Operand::Reg);

                        if (op1->type == Operand::Reg) {
                            auto reg = Registers::convert2RegID(op1->field[0]);
                            v1 = getRegister(reg);
                            ctx[getRegName(op0->field[0])] = v1;
                            break;

                        }
                        if (op1->type == Operand::Mem) {
                            //TODO
                            v1 = getMemory(it->memory_address, op1->bit);
                            ctx[getRegName(op0->field[0])] = v1;
                        }
                    }
                        break;
                    case x86::X86_INS_CMOVB:

                        //case tana::instruction::t_cmovz:
                        if (next(it) == end) {
                            break;
                        }
                        if (isRegSame(*it, *next(it))) {
                            break;
                        }
                    case x86::X86_INS_MOV: {
                        if (op0->type == Operand::Reg) {
                            if (op1->type == Operand::ImmValue) { // mov reg, 0x1111
                                uint32_t temp_concrete = stoul(op0->field[0], 0, 16);
                                v1 = std::make_shared<Value>(CONCRETE, temp_concrete, this);
                                ctx[getRegName(op0->field[0])] = v1;
                            } else if (op1->type == Operand::Reg) { // mov reg, reg
                                ctx[getRegName(op0->field[0])] = ctx[getRegName(op1->field[0])];
                            } else if (op1->type == Operand::Mem) { // mov reg, dword ptr [ebp+0x1]
                                /* 1. Get mem address
                                2. check whether the mem address has been accessed
                                3. if not, create a new value
                                4. else load the value in that memory
                                */
                                v1 = getMemory(it->memory_address, op1->bit);
                                ctx[getRegName(op0->field[0])] = v1;
                            } else {
                                std::cout << "op1 is not ImmValue, Reg or Mem" << std::endl;
                                return 1;
                            }
                        } else if (op0->type == Operand::Mem) {
                            if (op1->type == Operand::ImmValue) { // mov dword ptr [ebp+0x1], 0x1111
                                uint32_t temp_concrete = stoul(op1->field[0], 0, 16);
                                memory[it->memory_address] = std::make_shared<Value>(CONCRETE, temp_concrete, this);
                            } else if (op1->type == Operand::Reg) { // mov dword ptr [ebp+0x1], reg
                                memory[it->memory_address] = ctx[getRegName(op1->field[0])];
                            }
                        } else {
                            ERROR("Error: The first operand in MOV is not Reg or Mem!");
                        }
                    }
                        break;
                    case x86::X86_INS_LEA:

                        /* lea reg, ptr [edx+eax*1]
                        interpret lea instruction based on different address type
                        1. op0 must be reg
                        2. op1 must be addr
                        */
                        if (op0->type != Operand::Reg || op1->type != Operand::Mem) {
                            ERROR("lea format error!");
                        }
                        switch (op1->tag) {
                            case 5: {
                                std::shared_ptr<Value> f0, f1, f2; // corresponding field[0-2] in operand
                                f0 = ctx[getRegName(op1->field[0])];
                                f1 = ctx[getRegName(op1->field[1])];
                                if (op1->field[2] == "1") {
                                    res = buildop2("add", f0, f1);
                                    ctx[getRegName(op0->field[0])] = res;
                                    break;
                                }
                                uint32_t temp_concrete = stoul(op1->field[2], 0, 16);
                                f2 = std::make_shared<Value>(CONCRETE, temp_concrete, this);
                                res = buildop2("imul", f1, f2);
                                res = buildop2("add", f0, res);
                                ctx[getRegName(op0->field[0])] = res;
                                break;
                            }
                            case 7: {
                                std::shared_ptr<Value> f0, f1, f2, f3; // addr7: eax+ebx*2+0xfffff1
                                f0 = ctx[getRegName(op1->field[0])];       //eax
                                f1 = ctx[getRegName(op1->field[1])];       //ebx
                                uint32_t temp_concrete1 = stoul(op1->field[2], 0, 16);
                                f2 = std::make_shared<Value>(CONCRETE, temp_concrete1, this);   //2
                                std::string sign = op1->field[3];          //+
                                uint32_t temp_concrete2 = stoul(op1->field[4], 0, 16);
                                f3 = std::make_shared<Value>(CONCRETE, temp_concrete2, this);   //0xfffff1
                                assert((sign == "+") || (sign == "-"));
                                if (op1->field[2] == "1") {
                                    res = buildop2("add", f0, f1);
                                } else {
                                    res = buildop2("imul", f1, f2);
                                    res = buildop2("add", f0, res);
                                }
                                if (sign == "+")
                                    res = buildop2("add", res, f3);
                                else
                                    res = buildop2("sub", res, f3);
                                ctx[getRegName(op0->field[0])] = res;
                                break;
                            }
                            case 4: {
                                std::shared_ptr<Value> f0, f1; // addr4: eax+0xfffff1
                                f0 = ctx[getRegName(op1->field[0])];       //eax
                                uint32_t temp_concrete = stoul(op1->field[2], 0, 16);
                                f1 = std::make_shared<Value>(CONCRETE, temp_concrete, this);   //0xfffff1
                                std::string sign = op1->field[1];          //+
                                if (sign == "+")
                                    res = buildop2("add", f0, f1);
                                else
                                    res = buildop2("sub", f0, f1);
                                ctx[getRegName(op0->field[0])] = res;
                                break;
                            }
                            case 6: {
                                std::shared_ptr<Value> f0, f1, f2; // addr6: eax*2+0xfffff1
                                f0 = ctx[getRegName(op1->field[0])];
                                uint32_t temp_concrete1 = stoul(op1->field[1]);
                                uint32_t temp_concrete2 = stoul(op1->field[3]);

                                f1 = std::make_shared<Value>(CONCRETE, temp_concrete1, this);
                                f2 = std::make_shared<Value>(CONCRETE, temp_concrete2, this);
                                std::string sign = op1->field[2];
                                if (op1->field[1] == "1") {
                                    res = f0;
                                } else {
                                    res = buildop2("imul", f0, f1);
                                }
                                if (sign == "+")
                                    res = buildop2("add", res, f2);
                                else
                                    res = buildop2("sub", res, f2);
                                ctx[getRegName(op0->field[0])] = res;
                                break;
                            }
                            case 3: {
                                std::shared_ptr<Value> f0, f1;          // addr3: eax*2
                                f0 = ctx[getRegName(op1->field[0])];

                                uint32_t temp_concrete = stoul(op1->field[1]);
                                f1 = std::make_shared<Value>(CONCRETE, temp_concrete, this);
                                res = buildop2("imul", f0, f1);
                                ctx[getRegName(op0->field[0])] = res;
                                break;
                            }

                            case 2: {
                                ctx[getRegName(op0->field[0])] = ctx[getRegName(op1->field[0])];
                                break;
                            }

                            case 1: {
                                std::shared_ptr<Value> f0;
                                uint32_t temp_concrete = stoul(op1->field[0], 0, 16);
                                f0 = std::make_shared<Value>(CONCRETE, temp_concrete, this);
                                ctx[getRegName(op0->field[0])] = f0;
                                break;
                            }
                            default:
                                ERROR("Other tags in addr is not ready for lea!");
                        }
                        break;
                    case x86::X86_INS_XCHG:
                        if (op1->type == Operand::Reg) {
                            v1 = ctx[getRegName(op1->field[0])];
                            if (op0->type == Operand::Reg) {
                                v0 = ctx[getRegName(op0->field[0])];
                                ctx[getRegName(op1->field[0])] = v0; // xchg reg, reg
                                ctx[getRegName(op0->field[0])] = v1;
                            } else if (op0->type == Operand::Mem) {
                                v0 = getMemory(it->memory_address, op0->bit);
                                ctx[getRegName(op1->field[0])] = v0; // xchg mem, reg
                                memory[it->memory_address] = v1;
                            } else {
                                ERROR("xchg error: 1");
                            }
                        } else if (op1->type == Operand::Mem) {
                            v1 = getMemory(it->memory_address, op1->bit);
                            if (op0->type == Operand::Reg) {
                                v0 = ctx[getRegName(op0->field[0])];
                                ctx[getRegName(op0->field[0])] = v1; // xchg reg, mem
                                memory[it->memory_address] = v0;
                            } else {
                                ERROR("xchg error 3");
                            }
                        } else {
                            ERROR("xchg error: 2");
                        }
                        break;
                    case x86::X86_INS_SBB:
                        if (op1->type == Operand::Reg) {

                        }
                        if (op1->type == Operand::Mem) {

                        }

                    default:
                        if (op1->type == Operand::ImmValue) {
                            uint32_t temp_concrete = stoul(op1->field[0], 0, 16);
                            v1 = std::make_shared<Value>(CONCRETE, temp_concrete, this);
                        } else if (op1->type == Operand::Reg) {
                            v1 = ctx[getRegName(op1->field[0])];
                        } else if (op1->type == Operand::Mem) {
                            v1 = getMemory(it->memory_address, op1->bit);
                        } else {
                            ERROR("other instructions: op1 is not ImmValue, Reg, or Mem!");

                        }

                        if (op0->type == Operand::Reg) { // dest op is reg
                            v0 = ctx[getRegName(op0->field[0])];
                            res = buildop2(opcstr, v0, v1);
                            ctx[getRegName(op0->field[0])] = res;
                        } else if (op0->type == Operand::Mem) { // dest op is mem
                            v0 = getMemory(it->memory_address, op0->bit);
                            res = buildop2(opcstr, v0, v1);
                            memory[it->memory_address] = res;
                        } else {
                            ERROR("other instructions: op2 is not ImmValue, Reg, or Mem!");
                        }
                        break;
                }
            }

            if (oprnum == 3) {
                std::shared_ptr<Operand> op0 = it->oprd[0];
                std::shared_ptr<Operand> op1 = it->oprd[1];
                std::shared_ptr<Operand> op2 = it->oprd[2];
                std::shared_ptr<Value> v1, v2, v3, res;
                switch (opcode_id) {
                    case x86::X86_INS_IMUL:
                        if (op0->type == Operand::Reg &&
                            op1->type == Operand::Reg &&
                            op2->type == Operand::ImmValue) { // imul reg, reg, imm
                            v1 = ctx[getRegName(op1->field[0])];

                            uint32_t temp_concrete = stoul(op2->field[0], 0, 16);
                            v2 = std::make_shared<Value>(CONCRETE, temp_concrete, this);
                            res = buildop2(opcstr, v1, v2);
                            ctx[getRegName(op0->field[0])] = res;
                        } else {
                            ERROR("three operands instructions other than imul are not handled!");
                        }
                        break;
                    case x86::X86_INS_SHLD:
                    case x86::X86_INS_SHRD:
                        if (op0->type == Operand::Reg &&
                            op1->type == Operand::Reg &&
                            op2->type ==
                            Operand::ImmValue) {                                           // shld shrd reg, reg, imm
                            v1 = ctx[getRegName(op0->field[0])];
                            v2 = ctx[getRegName(op1->field[0])];
                            uint32_t temp_concrete = stoul(op2->field[0], 0, 16);
                            v3 = std::make_shared<Value>(CONCRETE, temp_concrete, this);
                            res = buildop3(opcstr, v1, v2, v3);
                            ctx[getRegName(op0->field[0])] = res;
                        } else {
                            ERROR("shrd or shld is not handled!");
                        }
                    default:
                        break;
                }
            }

            if (oprnum > 3) {
                ERROR("all instructions: number of operands is larger than 4!");
            }
        }
        return 0;
    }


    uint32_t
    SEEngine::conexec(std::shared_ptr<Value> f, std::map<std::shared_ptr<Value>, uint32_t> *input) {
        std::set<std::shared_ptr<Value>> inmapkeys;
        std::set<std::shared_ptr<Value>> inputsym = varmap::getInputs(f);
        for (auto it = input->begin(); it != input->end(); ++it) {
            inmapkeys.insert(it->first);
        }

        if (inmapkeys != inputsym) {
            ERROR("Some inputs don't have parameters!");
            return 1;
        }

        return eval(f, input);
    }

    bool isTree(std::shared_ptr<Value> v) {

        std::list<std::shared_ptr<Value>> list_que;
        list_que.push_back(v);
        uint32_t count = 0;
        while (!list_que.empty()) {
            std::shared_ptr<Value> v = list_que.front();
            list_que.pop_front();
            ++count;
            //auto result = std::find(list_que.begin(), list_que.end(), v);
            //if (result != list_que.end()) {
            //	return false;
            //}
            const std::unique_ptr<Operation> &op = v->opr;
            if (op != nullptr) {
                if (op->val[0] != nullptr) list_que.push_back(op->val[0]);
                if (op->val[1] != nullptr) list_que.push_back(op->val[1]);
                if (op->val[2] != nullptr) list_que.push_back(op->val[2]);
            }
            if ((list_que.size() > FORMULA_MAX_LENGTH) || (count > FORMULA_MAX_LENGTH))
                return false;
        }
        return true;
    }


    std::vector<std::shared_ptr<Value>>
    SEEngine::getAllOutput() {
        std::vector<std::shared_ptr<Value>> outputs;
        std::shared_ptr<Value> v;

        // symbols in registers
        v = ctx["eax"];
        if ((v->opr != nullptr) && (isTree(v)))
            outputs.push_back(v);
        v = ctx["ebx"];
        if ((v->opr != nullptr) && (isTree(v)))
            outputs.push_back(v);
        v = ctx["ecx"];
        if ((v->opr != nullptr) && (isTree(v)))
            outputs.push_back(v);
        v = ctx["edx"];
        if ((v->opr != nullptr) && (isTree(v)))
            outputs.push_back(v);

        // symbols in memory
        for (auto const &x : memory) {

            v = x.second;
            if (v == nullptr) {
                continue;
            }
            if ((v->opr != nullptr) && (isTree(v)))
                outputs.push_back(v);
        }
        //Erase Duplicates in a vector
        //std::sort(outputs.begin(), outputs.end());
        //outputs.erase(std::unique(outputs.begin(), outputs.end()), outputs.end());
        return outputs;
    }


    std::vector<std::shared_ptr<Value> > SEEngine::reduceValues(std::vector<std::shared_ptr<Value>> values) {
        auto values_size = values.size();
        std::vector<bool> redundancy_table(values_size, false);
        uint32_t values_size_after_reduction = 0;
        for (uint32_t i = 0; i < values_size; ++i) {
            for (uint32_t j = 0; j < values_size; ++j) {
                if (i == j) {
                    continue;
                }
                if (*values[i] == *values[j]) {
                    //std::cout << "i = " << i << "j = " << j << std::endl;
                    if (redundancy_table[i] == false) {
                        redundancy_table[j] = true;
                    }
                }
            }
        }


        for (uint32_t i = 0; i < values_size; ++i) {
            if (redundancy_table[i] == false) {
                ++values_size_after_reduction;
            }
        }

        std::vector<std::shared_ptr<Value> > values_reduced(values_size_after_reduction);
        uint32_t index = 0;
        for (uint32_t i = 0; i < values_size; ++i) {
            if (redundancy_table[i] == false) {
                values_reduced[index] = values[i];
                ++index;
            }
        }

        // Remove formulas that are either too long or too short
        assert(index == values_size_after_reduction);


        auto iter = values_reduced.begin();

        while (iter != values_reduced.end()) {
            uint32_t formula_size = 0;
            SEEngine::getFormulaLength(*iter, formula_size);
            if ((formula_size < FORMULA_MIN_LENGTH) || (formula_size > FORMULA_MAX_LENGTH)) {
                iter = values_reduced.erase(iter);
            } else {
                ++iter;
            }
        }

        return values_reduced;
    }


    uint32_t
    SEEngine::eval(std::shared_ptr<Value> v) { //for no input
        std::map<std::shared_ptr<Value>, uint32_t> varm;
        assert((varmap::getInputVector(v)).empty());
        return eval(v, &varm);
    }

    uint32_t
    SEEngine::eval(std::shared_ptr<Value> v, std::map<std::shared_ptr<Value>, uint32_t> *inmap) {
        const std::unique_ptr<Operation> &op = v->opr;
        if (op == nullptr) {
            if (v->valty == CONCRETE)
                return stoul(v->conval, 0, 16);
            else
                return (*inmap)[v];
        } else {
            uint32_t op0 = 0, op1 = 0;
            uint32_t op2 = 0;

            if (op->val[0] != nullptr) op0 = eval(op->val[0], inmap);
            if (op->val[1] != nullptr) op1 = eval(op->val[1], inmap);
            if (op->val[2] != nullptr) op2 = eval(op->val[2], inmap);

            if (op->opty == "add") {
                return op0 + op1;
            } else if (op->opty == "adc") {
                return op0 + op1 + 1;
            } else if (op->opty == "sub") {
                return op0 - op1;
            } else if (op->opty == "imul") {
                return op0 * op1;
            } else if (op->opty == "shld") {
                return shld32(op0, op1, op2);
            } else if (op->opty == "shrd") {
                return shrd32(op0, op1, op2);
            } else if (op->opty == "xor") {
                return op0 ^ op1;
            } else if (op->opty == "and") {
                return op0 & op1;
            } else if (op->opty == "or") {
                return op0 | op1;
            } else if (op->opty == "shl") {
                return op0 << op1;
            } else if (op->opty == "shr") {
                return op0 >> op1;
            } else if (op->opty == "sar") {
                return arithmeticRightShift(op0, op1);
            } else if (op->opty == "neg") {
                return ~op0 + 1;
            } else if (op->opty == "not") {
                return ~op0;
            } else if (op->opty == "inc") {
                return op0 + 1;
            } else if (op->opty == "dec") {
                return op0 - 1;
            } else if (op->opty == "rol") {
                return rol32(op0, op1);
            } else if (op->opty == "ror") {
                return ror32(op0, op1);
            } else {
                std::cout << "Instruction: [" << op->opty << "] is not interpreted!" << std::endl;
                if ((op->val[0] != nullptr) && ((op->val[1] != nullptr))) return op0 + op1;
                if (op->val[0] != nullptr) return op0;
                return 1;
            }
        }
    }
}