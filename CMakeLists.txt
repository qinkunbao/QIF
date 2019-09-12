cmake_minimum_required(VERSION 3.10)
project(taintTrace)



set(CMAKE_CXX_STANDARD 14)
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
add_subdirectory(nlohmann_json)


add_library(TraceLib
        lib/error.h
        lib/error.cpp
        lib/x86.cpp
        lib/x86.h
        lib/ins_parser.cpp
        lib/ins_parser.h
        lib/ins_types.h
        lib/ins_types.cpp
        lib/ins_semantics.h
        lib/ins_semantics.cpp
        lib/Engine.h
        lib/Engine.cpp
        lib/Register.cpp
        lib/Register.h
        lib/DynSEEngine.cpp
        lib/DynSEEngine.h
        lib/VarMap.h
        lib/VarMap.cpp
        lib/Tainter.cpp
        lib/Tainter.h
        lib/Loop.cpp
        lib/Loop.h
        lib/Function.cpp
        lib/Function.h
        lib/Blocks.cpp
        lib/Blocks.h
        lib/BitVector.h
        lib/BitVector.cpp
        lib/StaticSEEngine.cpp
        lib/StaticSEEngine.h
        lib/QIFSEEngine.h
        lib/QIFSEEngine.cpp
        lib/Constrains.h
        lib/Constrains.cpp
        lib/MonteCarlo.h
        lib/MonteCarlo.cpp
        lib/CallStack.h
        lib/CallStack.cpp lib/DebugInfo.cpp lib/DebugInfo.h
        lib/cmd.cpp lib/cmd.h
        lib/ELFInfo.cpp lib/ELFInfo.h
        lib/Trace2ELF.cpp lib/Trace2ELF.h
        lib/InputParser.h)

target_link_libraries(TraceLib PRIVATE nlohmann_json::nlohmann_json)


add_executable(taintTrace
        TaintTrace/TaintTrace.cpp)

add_executable(SymbolicExecution
        SymbolicExecution/SymbolicExecution.cpp)


add_executable(LoopDetection
        LoopDetection/LoopDetection.cpp)

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

add_executable(BitTaint
        BitTaint/BitTaint.cpp
        BitTaint/BitTaint.h
        BitTaint/Dataflow.cpp
        BitTaint/Memory.cpp
        BitTaint/Memory.h)

add_executable(StaticSE
        StaticSE/StaticSE.cpp)

add_executable(QIF
        QIF/QIF.cpp)

add_executable(TestCMD
        test/test_cmd.cpp)

add_executable(TestELF
        test/test_elf.cpp)


add_executable(TestELFSym
        test/test_elf_sym.cpp)


target_link_libraries(taintTrace TraceLib)
target_link_libraries(SymbolicExecution TraceLib)
target_link_libraries(LoopDetection TraceLib)
target_link_libraries(TestX86 TraceLib)
target_link_libraries(TestBitVector TraceLib)
target_link_libraries(BitTaint TraceLib)
target_link_libraries(StaticSE TraceLib)
target_link_libraries(TestConstrain TraceLib)
target_link_libraries(QIF TraceLib)
target_link_libraries(TestRandom TraceLib)
target_link_libraries(TestCMD TraceLib)
target_link_libraries(TestELFSym TraceLib)

