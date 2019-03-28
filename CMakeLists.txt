cmake_minimum_required(VERSION 3.10)
project(taintTrace)



set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(nlohmann_json 3.2.0 REQUIRED)

include_directories(lib)
include_directories(TaintTrace)
include_directories(SymbolicExecution)
include_directories(LoopDetection)
include_directories(test)
include_directories(BitTaint)
include_directories(StaticSE)


add_library(TraceLib
        lib/BitVector.h
        lib/BitVector.cpp
        lib/error.h
        lib/error.cpp
        lib/x86.cpp
        lib/x86.h
        lib/ins_parser.cpp
        lib/ins_parser.h
        lib/ins_types.h
        lib/ins_types.cpp
        lib/Engine.h
        lib/Engine.cpp
        lib/Register.cpp
        lib/Register.h
        lib/SEEngine.cpp
        lib/SEEngine.h
        lib/VarMap.h
        lib/VarMap.cpp
        lib/Tainter.cpp
        lib/Tainter.h
        lib/Loop.cpp
        lib/Loop.h
        lib/Function.cpp
        lib/Function.h)

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

add_executable(BitTaint
        BitTaint/BitTaint.cpp
        BitTaint/BitTaint.h
        BitTaint/Dataflow.cpp
        BitTaint/Memory.cpp
        BitTaint/Memory.h)

add_executable(StaticSE
        StaticSE/StaticSE.cpp)


target_link_libraries(taintTrace TraceLib)
target_link_libraries(SymbolicExecution TraceLib)
target_link_libraries(LoopDetection TraceLib)
target_link_libraries(TestX86 TraceLib)
target_link_libraries(TestBitVector TraceLib)
target_link_libraries(BitTaint TraceLib)
target_link_libraries(StaticSE TraceLib)