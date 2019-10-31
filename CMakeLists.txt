cmake_minimum_required(VERSION 3.10)
project(taintTrace)



set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(JSON_BuildTests OFF CACHE INTERNAL "")


include_directories(lib)

include_directories(external)
include_directories(TaintTrace)
include_directories(SymbolicExecution)
include_directories(LoopDetection)
include_directories(test)
include_directories(BitTaint)
include_directories(StaticSE)
add_subdirectory(external/nlohmann_json)


add_library(TraceLib
        lib/error.hpp
        lib/error.cpp
        lib/x86.cpp
        lib/x86.hpp
        lib/ins_parser.cpp
        lib/ins_parser.hpp
        lib/ins_types.hpp
        lib/ins_types.cpp
        lib/ins_semantics.hpp
        lib/ins_semantics.cpp
        lib/Engine.hpp
        lib/Engine.cpp
        lib/Register.cpp
        lib/Register.hpp
        lib/DynSEEngine.cpp
        lib/DynSEEngine.hpp
        lib/VarMap.hpp
        lib/VarMap.cpp
        lib/Tainter.cpp
        lib/Tainter.hpp
        lib/Loop.cpp
        lib/Loop.hpp
        lib/Function.cpp
        lib/Function.hpp
        lib/Blocks.cpp
        lib/Blocks.hpp
        lib/BitVector.hpp
        lib/BitVector.cpp
        lib/StaticSEEngine.cpp
        lib/StaticSEEngine.hpp
        lib/QIFSEEngine.hpp
        lib/QIFSEEngine.cpp
        lib/Constrains.hpp
        lib/Constrains.cpp
        lib/MonteCarlo.hpp
        lib/MonteCarlo.cpp
        lib/CallStack.hpp
        lib/CallStack.cpp lib/DebugInfo.cpp lib/DebugInfo.hpp
        lib/cmd.cpp lib/cmd.hpp
        lib/ELFInfo.cpp lib/ELFInfo.hpp
        lib/Trace2ELF.cpp lib/Trace2ELF.hpp
        lib/InputParser.hpp
        lib/LibcModel.hpp lib/LibcModel.cpp)


target_link_libraries(TraceLib PRIVATE nlohmann_json::nlohmann_json)


add_executable(taintTrace
        TaintTrace/TaintTrace.cpp)

add_executable(SymbolicExecution
        SymbolicExecution/SymbolicExecution.cpp)


add_executable(LoopDetection
        LoopDetection/LoopDetection.cpp)


add_executable(Ngram
        Ngram/Ngram.cpp)

add_executable(TestBitVector
        test/test_bitvector.cpp)

add_executable(TestX86
        test/test_x86.cpp)

add_executable(TestConstrain
        test/test_constrain.cpp)

add_executable(TestBit
        test/test_bit.cpp)

add_executable(TestQIF
        test/test_qif.cpp)

add_executable(TestQIF1
        test/test_qif1.c)

add_executable(TestQIF2
        test/test_qif2.c)

add_executable(TestRandom
        test/test_random_num.cpp)

add_executable(TestDebugInfo
        test/test_debuginfo.cpp)

add_executable(TestDebugInfoStr
        test/test_debuginfo_str.cpp)

add_executable(TestBvMul
        test/test_bvmul.cpp)

add_executable(BitTaint
        BitTaint/BitTaint.cpp
        BitTaint/BitTaint.hpp
        BitTaint/Dataflow.cpp
        BitTaint/Memory.cpp
        BitTaint/Memory.hpp)

add_executable(StaticSE
        StaticSE/StaticSE.cpp)

add_executable(QIF
        QIF/QIF.cpp)

add_executable(QIF_EXP
        QIF_EXP/QIF_EXP.cpp)

add_executable(TestCMD
        test/test_cmd.cpp)

add_executable(TestELF
        test/test_elf.cpp)

add_executable(TestEval
        test/test_eval.cpp)


add_executable(TestELFSym
        test/test_elf_sym.cpp)


target_link_libraries(taintTrace TraceLib)
target_link_libraries(SymbolicExecution TraceLib)
target_link_libraries(Ngram TraceLib)
target_link_libraries(LoopDetection TraceLib)
target_link_libraries(TestX86 TraceLib)
target_link_libraries(TestBitVector TraceLib)
target_link_libraries(BitTaint TraceLib)
target_link_libraries(StaticSE TraceLib)
target_link_libraries(TestConstrain TraceLib)
target_link_libraries(QIF TraceLib)
target_link_libraries(QIF_EXP TraceLib)
target_link_libraries(TestRandom TraceLib)
target_link_libraries(TestDebugInfo TraceLib)
target_link_libraries(TestDebugInfoStr TraceLib)
target_link_libraries(TestCMD TraceLib)
target_link_libraries(TestELFSym TraceLib)
target_link_libraries(TestBit TraceLib)
target_link_libraries(TestBvMul TraceLib)
target_link_libraries(TestEval TraceLib)





