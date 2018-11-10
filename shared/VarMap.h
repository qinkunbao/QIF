#pragma once

#include <iostream>
#include <list>
#include <set>
#include <map>
#include <vector>
#include <queue>
#include <memory>
#include <iterator>
#include "ins_types.h"
#include "SEEngine.h"


namespace tana {
    namespace varmap {

        typedef std::pair<std::set<uint32_t>, std::set<uint32_t>> PartMap;
        typedef std::pair<std::map<uint32_t, uint32_t>, std::map<uint32_t, uint32_t>> FullMap;

        struct BitValue {
            std::shared_ptr<Value> symbol;
            t_type::index bit_index;

            BitValue();

            BitValue(std::shared_ptr<Value> v, t_type::index n);
        };

        class BitMatrix {
        private:
            std::vector<std::vector<bool> > m;

        public:
            BitMatrix() {}

            BitMatrix(std::vector<std::vector<bool> > mx);

            BitMatrix(uint32_t row, uint32_t col);

            void initIdentityMatrix(uint32_t n);

            void show() const;

            std::vector<bool> getRow(uint32_t row_index) const;

            std::vector<uint32_t> getRowVector() const;

            std::vector<uint32_t> getColVector() const;

            uint32_t getRowNum() const;

            uint32_t getColNum() const;

            void randomizeAll();

            void setCol(uint32_t col, bool b);

            void setRow(std::vector<bool> bit_vector, uint32_t row_index);

            void setmValue(uint32_t row, uint32_t col, bool v);
        };

        void randomizeMappedVar(varmap::BitMatrix *input_m1, varmap::BitMatrix *input_m2,
                                std::map<uint32_t, uint32_t> *mappedvar);

        void setIdentityMatrix(BitMatrix *input_m1, BitMatrix *input_m2, std::map<uint32_t, uint32_t> *mappedvar);

        std::vector<std::shared_ptr<Value> > getInputVector(std::shared_ptr<Value> formula);

        std::set<std::shared_ptr<Value> > getInputs(std::shared_ptr<Value> output);

        bool varmapAndoutputCVC(SEEngine *se1, std::shared_ptr<Value> v1, SEEngine *se2, std::shared_ptr<Value> v2);

        void setOutputMatrix(BitMatrix *inmput_m, std::shared_ptr<Value> formula,
        		             std::vector<std::shared_ptr<Value> > *input_v,
                             std::vector<std::shared_ptr<Value> > *output_v,
                             SEEngine *se, BitMatrix *output_m);

        std::vector<bool> var2bv(std::map<std::shared_ptr<Value>, uint32_t> *varm,
        		                 std::vector<std::shared_ptr<Value>> *vv);

        std::map<std::shared_ptr<Value>, uint32_t> bv2var(std::vector<bool> bv,
        		                                          std::vector<std::shared_ptr<Value>> *vv);

        void updateUnmap(std::vector<PartMap> *unmap, std::vector<uint32_t> *vec1, std::vector<uint32_t> *vec2,
                         std::map<uint32_t, uint32_t> *mappedvar);

        void bitVarmap(std::shared_ptr<Value> formula1, std::shared_ptr<Value> formula2,
        		       SEEngine *se1, SEEngine *se2,
                       std::vector<std::shared_ptr<Value>> *input_v1,
                       std::vector<std::shared_ptr<Value>> *input_v2,
                       std::vector<PartMap> unmap_input, std::vector<PartMap> unmap_output,
                       std::map<uint32_t, uint32_t> map_input,
                       std::map<uint32_t, uint32_t> map_output,
                       std::list<FullMap> *result);

		bool fuzzFormula(std::shared_ptr<Value> v1, std::shared_ptr<Value> v2, SEEngine *se1, SEEngine *se2);

		bool checkFormula(std::shared_ptr<Value> v1, std::shared_ptr<Value> v2, SEEngine *se1, SEEngine *se2);

		std::map<std::shared_ptr<Value>, uint32_t> input2val(std::vector<uint32_t> input,
		                                                     std::vector<std::shared_ptr<Value>> *vv);

        std::map<int, int> matchFunction(SEEngine *se1, SEEngine *se2,
                                         std::vector<std::shared_ptr<Value>> v1s,
                                         std::vector<std::shared_ptr<Value>> v2s);

		void printV(std::shared_ptr<Value> v, std::stringstream& ss);

		void printV(std::shared_ptr<Value> v, std::stringstream& ss, uint32_t& length);

    }

}