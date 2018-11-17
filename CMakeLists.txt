cmake_minimum_required(VERSION 3.10)
project(taintTrace)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


include_directories(shared)
include_directories(TaintTrace)
include_directories(CryptoHunt)
include_directories(SymbolicExecution)
include_directories(DetectMagicNumber)
include_directories(LoopDetection)
include_directories(test)
include_directories(BitTaint)


add_library(TraceLib
        shared/x86.cpp
        shared/x86.h
        shared/ins_parser.cpp
        shared/ins_parser.h
        shared/ins_types.h
        shared/ins_types.cpp
        shared/Engine.h
        shared/Engine.cpp
        shared/Register.cpp
        shared/Register.h
        shared/SEEngine.cpp
        shared/SEEngine.h
        shared/VarMap.h
        shared/VarMap.cpp
        shared/Tainter.cpp
        shared/Tainter.h
        shared/Loop.cpp
        shared/Loop.h
        shared/Function.cpp
        shared/Function.h)

add_executable(taintTrace
        TaintTrace/TaintTrace.cpp)

add_executable(CryptoHunt
        CryptoHunt/CryptoHunt.cpp)


add_executable(SymbolicExecution
        SymbolicExecution/SymbolicExecution.cpp)

add_executable(DetectMagicNumber
        DetectMagicNumber/DetectMagicNumber.cpp)

add_executable(LoopDetection
        LoopDetection/LoopDetection.cpp)

add_executable(TestX86
        test/test_x86.cpp)

add_executable(BitTaint
        BitTaint/BitTaint.cpp
        BitTaint/BitTaint.h
        BitTaint/Dataflow.cpp
        BitTaint/Memory.cpp
        BitTaint/Memory.h)


target_link_libraries(taintTrace TraceLib)
target_link_libraries(CryptoHunt TraceLib)
target_link_libraries(SymbolicExecution TraceLib)
target_link_libraries(DetectMagicNumber TraceLib)
target_link_libraries(LoopDetection TraceLib)
target_link_libraries(TestX86 TraceLib)
target_link_libraries(BitTaint TraceLib)
