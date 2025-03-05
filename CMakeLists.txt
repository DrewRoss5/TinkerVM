cmake_minimum_required(VERSION 3.28)
project(TinkerVM VERSION 0.1.0)

add_executable(tvm
    inc/assembler.h
    inc/instruction.h
    inc/util.hpp
    inc/stack.hpp
    inc/machine.h
    src/assembler.cpp
    src/instruction.cpp
    src/stack.cpp
    src/machine.cpp
    src/main.cpp
)