  /*************************************************************************
  2     > File Name: BitTaint.h
  3     > Author: Qinkun Bao
  4     > Mail: qinkunbao@gmail.com
  5     > Created Time: Thu 01 Nov 2018 03:28:51 PM EDT
  6  ************************************************************************/

#pragma once

#include <vector>
#include <string>
#include "ins_parser.h"

#include "Memory.h"

  namespace tana {
      namespace bittaint {

          class BitDefUseChain {
          private:
              //The symbol name of the sensitive symbol
              std::string symbol_name;
              //The symbol address in the memory//The index of the bit in the origin symbol
              MemoryAddress symbol_def_address;
              //The index of the bit in the origin symbol
              int bit_index;
              //Memory Address that the bit is used
              std::vector<MemoryAddress> bit_use;
          public:
              BitDefUseChain(std::string name, MemoryAddress def_address, int index) : symbol_name(name),
                                                                                       symbol_def_address(def_address),
                                                                                       bit_index(index) {
              }

              void bitUse(MemoryAddress m_addr) {
                  bit_use.push_back(m_addr);
              }
          };


          class BitTaint {
          private:
              std::vector<BitDefUseChain> DefUse;
              Memory mem;
              Register reg;
              std::vector<tana::Inst_Dyn>::iterator eip;
              bool forward = true;
              std::vector<tana::Inst_Dyn>::iterator start;
              std::vector<tana::Inst_Dyn>::iterator end;

              void update_def_use(std::vector<int> bits);

              void check_memory_access(uint32_t mem_addr, uint32_t mem_size);

              static std::vector<Byte> bvadd(const std::vector<Byte> &a, const std::vector<Byte> &b);

              static std::vector<Byte> bvadd(const std::vector<Byte> &a, uint32_t b);

              static std::vector<Byte> bvsub(const std::vector<Byte> &a, const std::vector<Byte> &b);

              static std::vector<Byte> bvsub(const std::vector<Byte> &a, uint32_t b);

              static std::vector<Byte> bvmul(const std::vector<Byte> &a, const std::vector<Byte> &b);

              static std::vector<Byte> bvmul(const std::vector<Byte> &a, uint32_t b);

              static std::vector<Byte> bvudiv(const std::vector<Byte> &a, const std::vector<Byte> &b);

              static std::vector<Byte> bvudiv(const std::vector<Byte> &a, uint32_t b);

              static std::vector<Byte> bvor(const std::vector<Byte> &a, const std::vector<Byte> &b);

              static std::vector<Byte> bvor(const std::vector<Byte> &a, uint32_t b);

              static std::vector<Byte> bvand(const std::vector<Byte> &a, const std::vector<Byte> &b);

              static std::vector<Byte> bvand(const std::vector<Byte> &a, uint32_t b);


              static std::vector<Byte> bvxor(const std::vector<Byte> &a, const std::vector<Byte> &b);

              static std::vector<Byte> bvxor(const std::vector<Byte> &a, uint32_t b);

              static std::vector<Byte> bvshl(const std::vector<Byte> &a, const std::vector<Byte> &b);

              static std::vector<Byte> bvshl(const std::vector<Byte> &a, uint32_t b);

              static std::vector<Byte> bvlshr(const std::vector<Byte> &a, const std::vector<Byte> &b);

              static std::vector<Byte> bvlshr(const std::vector<Byte> &a, uint32_t b);

              static std::vector<Byte> bvurem(const std::vector<Byte> &a, const std::vector<Byte> &b);

              static std::vector<Byte> bvnot(const std::vector<Byte> &a);

              static std::vector<Byte> bvneg(const std::vector<Byte> &a);

              static bool bvcmp(const std::vector<Byte> &a, const std::vector<Byte> &b);

              static bool notcmp(const std::vector<Byte> &a, const std::vector<Byte> &b);


              int DO_X86_INS_CALL(const tana::Inst_Dyn &it);

              int DO_X86_INS_PUSH(const tana::Inst_Dyn &it);

              int DO_X86_INS_POP(const tana::Inst_Dyn &it);

              int DO_X86_INS_NEG(const tana::Inst_Dyn &it);

              int DO_X86_INS_NOT(const tana::Inst_Dyn &it);

              int DO_X86_INS_INC(const tana::Inst_Dyn &it);

              int DO_X86_INS_DEC(const tana::Inst_Dyn &it);

              int DO_X86_INS_MOVSX(const tana::Inst_Dyn &it);

              int DO_X86_INS_MOVZX(const tana::Inst_Dyn &it);

              int DO_X86_INS_CMOVB(const tana::Inst_Dyn &it);

              int DO_X86_INS_MOV(const tana::Inst_Dyn &it);

              int DO_X86_INS_LEA(const tana::Inst_Dyn &it);

              int DO_X86_INS_XCHG(const tana::Inst_Dyn &it);

              int DO_X86_INS_SUB(const tana::Inst_Dyn &it);

              int DO_X86_INS_SBB(const tana::Inst_Dyn &it);

              int DO_X86_INS_IMUL(const tana::Inst_Dyn &it);

              int DO_X86_INS_MUL(const tana::Inst_Dyn &it);

              int DO_X86_INS_SHR(const tana::Inst_Dyn &it);

              int DO_X86_INS_SHL(const tana::Inst_Dyn &it);

              int DO_X86_INS_SHLD(const tana::Inst_Dyn &it);

              int DO_X86_INS_SHRD(const tana::Inst_Dyn &it);

              int DO_X86_INS_ADD(const tana::Inst_Dyn &it);

              int DO_X86_INS_ADC(const tana::Inst_Dyn &it);

              int DO_X86_INS_RET(const tana::Inst_Dyn &it);

              int DO_X86_INS_AND(const tana::Inst_Dyn &it);

              int DO_X86_INS_LEAVE(const tana::Inst_Dyn &it);

              int DO_X86_INS_XOR(const tana::Inst_Dyn &it);

          public:
              BitTaint(std::string symbol_name, uint32_t memory_address, uint32_t size, \
          std::vector<tana::Inst_Dyn>::iterator start, std::vector<tana::Inst_Dyn>::iterator end);

              BitTaint(std::string symbol_name, uint32_t memory_address, uint32_t size, bool forward);

              int execute_insn(std::vector<tana::Inst_Dyn>::iterator it);

              void run();

          };

      }
  }