#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <stdint.h>
#include <array>

enum op_types {
    MEM_OP = 0x00, 
    LOGIC_OP = 0x10,
    JUMP_OP = 0x20,
    STACK_OP = 0x30,
    IO_OP = 0x40,
    HEAP_OP = 0x50,
};
enum data_types {
    WORD,
    STRINGZ,
    STRING,
    DATA,
};
enum op_codes{
    COPY,
    STORE_WORD,
    STORE_BYTE,
    LOAD_WORD,
    LOAD_BYTE,
    ALLOC_MEM,
    ALLOC_STR,
    LOAD_ADDR,
    ADD = 0x10,
    SUB,
    MUL,
    DIV,
    REM,
    COMP,
    AND,
    OR, 
    XOR,
    SR, 
    SL,
    JUMP = 0x20,
    JEQ,
    JNE,
    JGT,
    JLT,
    CAL,
    RET,
    PUSH = 0x30,
    PUSH_B,
    POP,
    POP_B,
    PUT_S = 0x40,
    PUT_I,
    GET_S,
    GET_I,
    HEAP_ALLOC = 0x50,
    HEAP_FREE
};


struct Instruction{
    uint8_t op_code;
    uint8_t registers;
    uint64_t extend;
    std::array<uint8_t, 10> to_bytes();
    static Instruction from_bytes(const std::array<uint8_t, 10>& arr);
};

#endif