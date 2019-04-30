#pragma once

#include "BitVector.h"
#include "ins_types.h"
#include "Constrains.h"



namespace tana {
   class Inst_Base;

    class SEEngine{


    public:
        bool imm2sym = false;

        bool eflags = false;

        std::vector<std::unique_ptr<Inst_Base>>::iterator start;
        std::vector<std::unique_ptr<Inst_Base>>::iterator end;

        explicit SEEngine(bool type);

        static std::shared_ptr<BitVector> ZeroExt(std::shared_ptr<BitVector> v, tana_type::T_SIZE);

        static std::shared_ptr<BitVector> SignExt(std::shared_ptr<BitVector> v, tana_type::T_SIZE orgin_size,
                                           tana_type::T_SIZE new_size);

        static std::shared_ptr<BitVector> Extract(std::shared_ptr<BitVector> v, int low, int high);
        static std::shared_ptr<BitVector> Concat(std::shared_ptr<BitVector> v1, std::shared_ptr<BitVector> v2);
        static std::shared_ptr<BitVector> Concat(std::shared_ptr<BitVector> v1, std::shared_ptr<BitVector> v2,
                                          std::shared_ptr<BitVector> v3);

        static void getFormulaLength(const std::shared_ptr<BitVector> &v, uint32_t &len);


        void init(std::vector<std::unique_ptr<Inst_Base>>::iterator it1,
                  std::vector<std::unique_ptr<Inst_Base>>::iterator it2);

        virtual void initAllRegSymol(std::vector<std::unique_ptr<Inst_Base>>::iterator it1,
                                     std::vector<std::unique_ptr<Inst_Base>>::iterator it2);

        bool isImmSym(uint32_t num);

        uint32_t conexec(std::shared_ptr<BitVector> f, std::map<std::shared_ptr<BitVector>, uint32_t> *input);

        std::vector<std::shared_ptr<BitVector> > reduceValues(std::vector<std::shared_ptr<BitVector>> values);

        static uint32_t eval(const std::shared_ptr<BitVector> &v, std::map<std::shared_ptr<BitVector>, uint32_t> *inmap);

        static uint32_t eval(const std::shared_ptr<BitVector> &v);

        virtual std::shared_ptr<BitVector> readReg(x86::x86_reg reg) = 0;

        virtual std::shared_ptr<BitVector> readReg(std::string reg) = 0;

        virtual bool writeReg(x86::x86_reg reg, std::shared_ptr<BitVector> v) = 0;

        virtual bool writeReg(std::string reg, std::shared_ptr<BitVector> v) = 0;

        virtual std::shared_ptr<BitVector> readMem(std::string memory_address, tana_type::T_SIZE size) = 0;

        virtual bool writeMem(std::string memory_address, tana_type::T_SIZE size, std::shared_ptr<BitVector> v) = 0;

        virtual int run();

        virtual std::vector<std::shared_ptr<BitVector>> getAllOutput() =0;



        std::shared_ptr<BitVector> formula_simplfy(std::shared_ptr<BitVector> v);

        virtual void updateFlags(std::string flag_name, std::shared_ptr<BitVector> cons)
        {
            return;
        }

        virtual void clearFlags(std::string flag_name)
        {
            return;
        }

        virtual std::shared_ptr<BitVector> getFlags(std::string)
        {
            return nullptr;
        }

        virtual void updateConstrains(std::shared_ptr<Constrain> cons)
        {
            return;
        }



        virtual ~SEEngine()= default;

    };

}
