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
    std::string tmp;
    try{
        assembler.parse_inst("loada r7 other_label");
    }
    catch (std::runtime_error e){
        std::cout << "Error message: "  << e.what() << std::endl;
    }
    assembler.parse_inst("my_label: .word");
    assembler.parse_inst("other_label: .data 16");
    // should output 0a070000000000000001
    print_inst(assembler.parse_inst("loada r7 other_label")); 
}