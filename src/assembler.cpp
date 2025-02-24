#include <stdexcept>
#include <iostream>

#include "../inc/assembler.h"

// splits a string by spaces and returns each "word"
std::vector<std::string> split_str(std::string& str){
        std::vector<std::string> retval;
        size_t space_index = str.find(" ");
        while (str.size() && space_index != str.npos){
            retval.push_back(str.substr(0, space_index));
            str = str.substr(space_index);
        }
        if (str.size())
            retval.push_back(str);
        return retval;
}

// parses an operation and returns its 8-bit opcdoe
uint8_t Assembler::parse_op(const std::string& op){
    if (!this->op_map.count(op))
        return NULL_INST;
    // parse the opcode and add the immediate bit
    auto operation = this->op_map[op];
    uint8_t retval = (operation.first << 1);
    retval |= (int) operation.second;
    return retval;
}

// converts a pneumonic register into its corresponding 4bit register code
// raises an error if the code is invalid
uint8_t Assembler::parse_reg(const std::string& reg){
    if (reg[0] != 'r')
        throw std::runtime_error("invalid register: "+reg);
    uint8_t reg_no;
    try{
        reg_no = std::stoi(reg.substr(1));
    }
    catch (std::invalid_argument){
        throw std::runtime_error("invalid register: "+reg);
    }
    if (reg_no < 0 || reg_no > 15)
        throw std::runtime_error("invalid register: "+reg);
    return reg_no;
}

// parses an immediate value, and throws a std::runtime error if it's invalid
uint64_t Assembler::parse_immediate(const std::string& imm){
    try{
        return std::stol(imm);
    }
    catch (std::invalid_argument){
        throw std::runtime_error("invalid immediate value");
    }
}

// combines two four-bit registers into a single eight-bit  constant
uint8_t Assembler::merge_registers(uint8_t r1, uint8_t r2){
    uint8_t retval = r1 << 4;
    return r1 | r2;
}

// converts a pneumonic text insturction to a byte code instruction
// todo: make this handle custom labels
Instruction Assembler::parse_inst(std::string& inst){
    std::vector<std::string> operands = split_str(inst);
    uint8_t op_code = this->parse_op(operands[0]);
    uint8_t op_type = op_code & 0xE0; // left most three bits  
    // determine how to parse the instruction, based on it's type
    Instruction retval;
    switch (op_type){
        case MEM_OP:
            retval = parse_mem(op_code, operands);
            break;
        case LOGIC_OP:
            retval = parse_logic(op_code, operands);
            break;
        case STACK_OP:
            retval = parse_stack(op_code, operands);
            break;
        case IO_OP:
            retval = parse_io(op_code, operands);
            break;
    }
    return retval;

}

// parses a memory operation
Instruction Assembler::parse_mem(uint8_t op_code, const std::vector<std::string>& operands){
    if (operands.size() != 3)
        throw std::runtime_error("invalid instruction");
    uint8_t reg_byte = parse_reg(operands[0]);
    
    uint8_t reg2;
    uint64_t extend = 0x0;
    // the last bit is a one if rhs is an immediate, otherwise it's a 0
    switch (op_code & 0x01){
        case 0:
            // store the second register in the register byte
            reg2 = parse_reg(operands[2]);
            reg_byte = merge_registers(reg_byte, reg2);
            break;
        case 1:
            extend = parse_immediate(operands[2]);
            break;
    }
    // construct the instruction
    Instruction retval;
    retval.op_code = op_code;
    retval.registers = reg_byte;
    retval.extend = extend;
    return retval;
}

// parses a logical/arithmetic expression 
Instruction Assembler::parse_logic(uint8_t op_code, const std::vector<std::string>& operands){
    // ensure four opperands are included (opcode, dst, lhs, rhs)
    if (operands.size() != 4)
        throw std::runtime_error("invalid instruction");
    // the first two operands must be registers
    uint8_t reg1 = parse_reg(operands[1]);
    uint8_t reg2 = parse_reg(operands[2]);
    uint8_t reg_byte = merge_registers(reg1, reg2);
    uint64_t extend;
     // the last bit is a one if rhs is an immediate, otherwise it's a 0
    switch (op_code & 0x01){
        case 0:
            extend = parse_reg(operands[3]);
            break;
        case 1:
            extend = parse_immediate(operands[3]);
            break;
    }
    Instruction retval;
    retval.op_code = op_code;
    retval.registers = reg_byte;
    retval.extend = extend;
    return retval;
}

// parses a stack instruction
Instruction Assembler::parse_stack(uint8_t op_code, const std::vector<std::string>& operands){
    if (operands.size() != 2)
        throw std::runtime_error("invalid instruction");
    Instruction retval;
    retval.op_code = op_code;
    // we ignore the instruction bit when comparing operation codes
    switch (op_code & 0xfe){
        case STACK_PUSH:
            // determine if we are parsing pushing an immediate or register value
            if (op_code & 0x01 == 1)
                retval.extend = parse_immediate(operands[1]);
            else
                retval.extend = parse_reg(operands[1]);
            break;
        case STACK_POP:
            retval.registers = parse_reg(operands[1]);
        break;
    }
    return retval;
}