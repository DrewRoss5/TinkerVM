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
    assembler.assemble_file("../examples/binoptest.tasm", "logic.tcode");
    vm.exec_file("logic.tcode");
    std::cout << "Arithmatic/logic test" << std::endl;
    std::cout << "\t6 + 3: " <<(int) vm.get_register(9) << std::endl;
    std::cout << "\t6 - 3: " << vm.get_register(10) << std::endl;
    std::cout << "\t6 * 3: " << vm.get_register(11) << std::endl;
    std::cout << "\t6 / 3: " << vm.get_register(12) << std::endl;
    std::cout << "\t6 and 3: " << vm.get_register(13) << std::endl;
    std::cout << "\t6 * 10: " << vm.get_register(14) << std::endl;
    return 0;
}