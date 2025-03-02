#include <iostream>
#include <iomanip>
#include "../inc/assembler.h"

template <typename T>
std::string to_hex(T in){
    std::stringstream ss;
    ss << std::setw(sizeof(T)*2) << std::setfill('0') << std::hex << (int) in;
    return ss.str();
}

void print_inst(const Instruction& inst){
    std::cout << to_hex(inst.op_code) << to_hex(inst.registers) << to_hex(inst.extend) << std::endl; 
}

int main(){
    Assembler assembler;
    Instruction inst; 
    // test a memory exprssions
    print_inst(assembler.parse_inst("copy r9 r10"));            // "009a0000000000000000"
    print_inst(assembler.parse_inst("loadi r9 255"));           // "050900000000000000ff"
    // test logical/arithmetic expresions
    print_inst(assembler.parse_inst("or r11 r12 r13"));         // "38bc000000000000000d"
    print_inst(assembler.parse_inst("addi r9 r9 16"));          // "21990000000000000010"
    // declare a label to jump to
    assembler.parse_inst("lab_1:");
    // test jump commands
    print_inst(assembler.parse_inst("j lab_1"));                  // "40000000000000000004"
    print_inst(assembler.parse_inst("jeq r12 r13 lab_1"));        // "42cd0000000000000004"


}