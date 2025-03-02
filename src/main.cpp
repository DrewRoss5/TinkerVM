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
    inst = assembler.assemble_inst("loadi r9 65281");
    std::array<uint8_t, 10> arr = inst.to_bytes();
    for (int i = 0; i < 10; i++)
        std::cout << to_hex(arr[i]);
    std::cout << std::endl;
    print_inst(assembler.assemble_inst("loadi r9 65281"));


}