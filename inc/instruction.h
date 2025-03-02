#include <stdint.h>
#include <array>

struct Instruction{
    uint8_t op_code;
    uint8_t registers;
    uint64_t extend;
    std::array<uint8_t, 10> to_bytes();
};