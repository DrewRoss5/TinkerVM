#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <stdint.h>
#include <array>

struct Instruction{
    uint8_t op_code;
    uint8_t registers;
    uint64_t extend;
    std::array<uint8_t, 10> to_bytes();
    static Instruction from_bytes(const std::array<uint8_t, 10>& arr);
};

#endif