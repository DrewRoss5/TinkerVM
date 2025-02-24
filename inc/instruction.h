#include <stdint.h>

struct Instruction{
    uint8_t op_code;
    uint8_t registers;
    uint64_t extend;
};