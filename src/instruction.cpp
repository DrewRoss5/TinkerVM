#include "../inc/instruction.h"

std::array<uint8_t, 10> Instruction::to_bytes(){
    std::array<uint8_t, 10> retval;
    // store the op code and register values
    retval[0] = this->op_code;
    retval[1] = this->registers;
    // convert the extend to bytes
    uint64_t extend = this->extend;
    uint64_t mask_val = 0xff00000000000000;
    for (int i = 2; i < 10; i++){
        int shift_val = 8 * (9 - i);
        retval[i] = (extend & mask_val) >> shift_val;
        mask_val >>= 8;
    }
    return retval;
}

Instruction Instruction::from_bytes(const std::array<uint8_t, 10>& arr){
    Instruction retval;
    // read the opcode and registers
    retval.op_code = arr[0];
    retval.registers = arr[1];
    retval.extend = 0;
    // read the extend from bytes
    for (int i = 2; i < 10; i++){
        int shift_val = 8 * (9 - i);
        retval.extend += arr[i] << shift_val;
    } 
    return retval;
}