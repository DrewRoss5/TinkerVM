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

void print_registers(Machine& vm){
    for (int i = 0; i < 16; i++)
        std::cout << "R" << i << ": " << vm.get_register(i) << "\n";
    std::cout << std::endl;
}

int main(){
    Assembler assembler;
    Machine vm;
    assembler.assemble_file("../examples/functest.tasm", "func.tcode");
    vm.exec_file("func.tcode");
    print_registers(vm);
    return 0;
}