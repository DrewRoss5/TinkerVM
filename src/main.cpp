#include <iostream>
#include <iomanip>
#include "../inc/assembler.h"


int main(){
    Assembler assemble;
    uint8_t r1 = assemble.parse_reg("r11");
    uint8_t r2 = assemble.parse_reg("r5");
    std::cout << "R1: " << (int) r1 << std::endl;
    std::cout << "R2: " << (int) r2 << std::endl;
    std::cout << "Registers: " << (int) assemble.merge_registers(r1, r2) << std::endl;
}