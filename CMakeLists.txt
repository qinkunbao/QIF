cmake_minimum_required(VERSION 3.10)
project(taintTrace)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


include_directories(shared)
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


add_executable(TestX86
        test/test_x86.cpp)

add_executable(BitTaint
        BitTaint/BitTaint.cpp
        BitTaint/BitTaint.h
        BitTaint/Dataflow.cpp
        BitTaint/Memory.cpp
        BitTaint/Memory.h)


target_link_libraries(TestX86 TraceLib)
target_link_libraries(BitTaint TraceLib)
