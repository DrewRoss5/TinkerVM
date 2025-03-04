#include <iostream>
#include <iomanip>

#include "../inc/assembler.h"
#include "../inc/stack.hpp"

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
    // simple test of stack functions
    uint32_t num = 666;
    uint32_t n1 = 1024;
    uint16_t n2 = 64;
    Stack stack;
    // test a single push/pop operation
    stack.push(num);
    std::cout << "Testing single operation: " << std::endl;
    std::cout << '\t' << (stack.is_empty() ? "true" : "false") << std::endl; // false
    std::cout << '\t' << stack.size() << std::endl;                         // 5
    std::cout << '\t' << stack.pop_type<uint32_t>() << std::endl;           // 666
    std::cout << '\t' << (stack.is_empty() ? "true" : "false") << std::endl; // true 
    std::cout << '\t' << stack.size() << std::endl;                          // 0
    // test pushing and popping with values of multiple sizes
    std::cout << "Testing mutliple operations" << std::endl;
    stack.push(n1);
    stack.push(n2);
    std::cout << '\t' << stack.size() << std::endl;                         // 8
    std::cout << '\t' << stack.pop_type<uint16_t>() << std::endl;           // 64
    std::cout << '\t' << stack.pop_type<uint32_t>() << std::endl;           // 1024
    std::cout << '\t' << (stack.is_empty() ? "true" : "false") << std::endl; // true 
    std::cout << '\t' << stack.size() << std::endl;                          // 0    
    /*
    Assembler assembler;
    assembler.assemble_file("../examples/test1.tasm", "out1.tcode");
    assembler.assemble_file("../examples/test2.tasm", "out2.tcode");
    */
}