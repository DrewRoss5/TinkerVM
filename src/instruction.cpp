#include "../inc/instruction.h"
#include "../inc/util.hpp"

std::array<uint8_t, 10> Instruction::to_bytes(){
    std::array<uint8_t, 10> retval;
    // store the op code and register values
    retval[0] = this->op_code;
    retval[1] = this->registers;
    // convert the extend to bytes
    split_bytes(this->extend, &retval[2]);
    return retval;
}

Instruction Instruction::from_bytes(const std::array<uint8_t, 10>& arr){
    Instruction retval;
    // read the opcode and registers
    retval.op_code = arr[0];
    retval.registers = arr[1];
    retval.extend = merge_bytes<uint64_t>(&arr[2]);
    return retval;
}