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
    assembler.assemble_file("../examples/memtest.tasm", "memtest.tcode");
    vm.exec_file("memtest.tcode");
    std::cout << "Memory test. This should print '10' twice, followed by '1234'" << std::endl;
    std::cout << '\t' << vm.get_register(7) << std::endl;
    std::cout << '\t' << vm.get_register(8) << std::endl;
    std::cout << '\t' << vm.get_register(12) << std::endl;
    return 0;
}