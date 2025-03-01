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
        tmp = "loada r8 my_label";
        assembler.parse_inst(tmp);
    }
    catch (std::runtime_error e){
        std::cout << "Error message: "  << e.what() << std::endl;
    }
    tmp = "my_label: .word";
    assembler.parse_inst(tmp);
    tmp = "other_label: .data 16";
    assembler.parse_inst(tmp);
    tmp = "loada r7 other_label";
    // should output 0a070000000000000001
    print_inst(assembler.parse_inst(tmp)); 
}