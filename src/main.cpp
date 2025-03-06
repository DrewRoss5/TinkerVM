#include <iostream>
#include <iomanip>

#include "../inc/assembler.h"
#include "../inc/stack.hpp"
#include "../inc/machine.h"

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
    Machine vm;
    assembler.assemble_file("../examples/jumptest.tasm", "jump.tcode");
    vm.exec_file("jump.tcode");
    std::cout << "Testing jump operations, r7 should be 0, and r9 should be 50";
    for (int i = 0; i < 16; i++)
        std::cout << "\n\tr" << i << ": " << vm.get_register(i);
    std::cout << std::endl;
    return 0;
}