#include <vector>
#include <string>
#include <stdexcept>
#include <cmath>

#include "pow.h"
#include "../inc/assembler.h"
#include "../inc/machine.h"

#define SQUARE  0x60
#define POW     0x61

// parses an instruction to square the value of a register
Instruction parse_square(const std::vector<std::string>& operands){
    Instruction retval;
    retval.op_code = SQUARE << 1;
    if (operands.size() != 3)
        throw std::runtime_error("square only accepts two operands");
    uint8_t r1 = Assembler::parse_reg(operands[1]);
    uint8_t r2 = Assembler::parse_reg(operands[2]);
    retval.registers = Assembler::merge_registers(r1, r2);
    return retval;
}

// parses an instruction to raise one register to the power of another
Instruction parse_pow(const std::vector<std::string>& operands){
    Instruction retval;
    retval.op_code = POW << 1;
    if (operands.size() != 4)
        throw std::runtime_error("pow only accepts three operands");
    uint8_t dst = Assembler::parse_reg(operands[1]);
    uint8_t lhs = Assembler::parse_reg(operands[2]);
    uint8_t rhs = Assembler::parse_reg(operands[3]);
    retval.registers = Assembler::merge_registers(dst, lhs);
    retval.extend = rhs;
    return retval;
}

// executes an instruction from the pow extension
void exec_pow(Machine* machine, uint8_t op_code, bool immediate, uint8_t registers, uint64_t extend){
    uint8_t lhs, rhs, r1, r2;
    Machine::split_registers(registers, r1, r2);
    switch (op_code){
        case SQUARE:
            lhs = machine->get_register(r2);
            lhs *= lhs;
            machine->set_register(r1, lhs);
            break;
        case POW:
            lhs = machine->get_register(r2);
            rhs = machine->get_register(extend);
            rhs = (int) pow(lhs, rhs);
            machine->set_register(r1, rhs);
            break;      
        default:
            throw std::runtime_error("Malformed machine code");
            break;
    }
}

// initalizes the assembler to accept instructions from the pow extension
void init_pow_assembler(Assembler& assembler){
    assembler.add_extension("square", &parse_square);
    assembler.add_extension("pow", &parse_pow);
}

// initalizes the virtual machine to accept instructions from the pow extension
void init_pow_machine(Machine& machine){
    machine.add_extension(0x60, &exec_pow);
}