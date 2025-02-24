cmake_minimum_required(VERSION 3.28)
project(TinkerVM VERSION 0.1.0)

add_executable(tvm
    inc/assembler.h
    inc/instruction.h
    src/assembler.cpp
    src/main.cpp
)