#include <stdexcept>
#include <sstream>
#include <iostream>
#include <fstream>
#include <cstring>

#include "../inc/assembler.h"

#define INSTRUCTION_BYTES 10

// splits a string by spaces and returns each "word"
std::vector<std::string> split_str(const std::string& str){
        std::string str_copy = str;
        std::vector<std::string> retval;
        size_t space_index = str.find(" ");
        while (str_copy.size() && space_index != str_copy.npos){
            retval.push_back(str_copy.substr(0, space_index));
            str_copy = str_copy.substr(space_index+1);
            space_index = str_copy.find(" ");
        }
        if (str_copy.size())
            retval.push_back(str_copy);
        return retval;
}

// parses a string literal, and optionally appends null termination
std::string parse_str_lit(std::string& str_lit, bool null_terminate){
    if (str_lit[0] != '"')
        throw std::runtime_error("invalid string literal: no opening quotation");
    size_t pos = 1;
    size_t str_len = str_lit.size();
    bool closed =  false;
    std::string out;
    while (pos < str_len){
        char chr = str_lit[pos];
        if (chr == '"'){
            closed = true;
            break;
        }
        out.push_back(chr);
        pos++;
    }
    if (!closed)
        throw std::runtime_error("invalid string literal: no closing quotation");
    if (null_terminate)
        out.push_back('\0');
    return out;
}


// parses an operation and returns its 8-bit opcdoe
uint8_t Assembler::parse_op(const std::string& op){
    auto op_itt = op_map.find(op);
    if (op_itt == op_map.end()){
        // determine this if this a label or invalid statement
        if (op[op.size() - 1] == ':')
            return NULL_INST;
        throw std::runtime_error("invalid operation");
    }
    // parse the opcode and add the immediate bit
    auto operation = op_itt->second;
    uint8_t retval = operation.first;
    retval <<= 1;
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
// parses a label declaration
Instruction Assembler::parse_label(const std::string& label){
    // remove the colon from the end of the label
    std::vector<std::string> operands = split_str(label);
    Instruction retval;
    std::string label_name = operands[0];
    label_name = label_name.substr(0, label_name.size() - 1);
    // if there is no size specificiation, we assume that this is a program label
    if (operands.size() == 1){
        this->program_labels[label_name] = this->line_no;
        retval.op_code = NULL_INST;
        this->line_no++;
        return retval;
    }
    // generate an instruction to allocate space for the data label
    auto label_itt = type_map.find(operands[1]);
    if (label_itt  == type_map.end())
        throw std::runtime_error("invalid data type in label declaration");
    int label_type = label_itt->second;
    // determine the space to allocate based on the label type
    std::string str_lit;
    bool null_terminate;
    uint64_t mem_size;
    switch (label_type){
        case WORD:
            if (operands.size() != 2)
                throw std::runtime_error("invalid label declaration");
            retval.op_code = ALLOC_MEM << 1;
            retval.extend = 64;
            this->next_label++;
            break;
        case STRING:
        case STRINGZ:
            if (operands.size() < 3)
                throw std::runtime_error("invalid label declaration");
            // append all operands after the thrid to the third
            for (int i = 3; i < operands.size(); i++){
                operands[2] += operands[i];
            }
            null_terminate = (label_type == STRINGZ);
            str_lit = parse_str_lit(operands[2], null_terminate);
            
            // parse the string and create the instruction
            this->program_strs.push_back(str_lit);
            retval.op_code = ALLOC_STR << 1;
            retval.extend = program_strs.size() - 1;
            this->next_label++;
            break;
        case DATA:
            if (operands.size() != 3)
                throw std::runtime_error("invalid label declaration");
            mem_size = parse_immediate(operands[2]);
            retval.op_code = ALLOC_MEM << 1;
            retval.extend = mem_size;
            break;
    }
    this->line_no++;
    this->data_labels[label_name] = this->next_label;
    return retval;
}

uint64_t Assembler::parse_jmp_label(const std::string& label){
    auto itt = this->program_labels.find(label);
    if (itt == this->program_labels.end())
        throw std::runtime_error("invalid jump destination");
    return itt->second;
}

uint64_t Assembler::parse_data_label(const std::string& label){
    auto itt = this->data_labels.find(label);
    if (itt == this->data_labels.end())
        throw std::runtime_error("use of uneclared label: " + label);
    return itt->second;
}

// combines two four-bit registers into a single eight-bit  constant
uint8_t Assembler::merge_registers(uint8_t r1, uint8_t r2){
    uint8_t retval = r1 << 4;
    return retval ^ r2;
}

// converts an tinker assembly file to a byte code file to be exewcuted
void Assembler::assemble_file(const std::string& in_path, const std::string& out_path){
    // read the input file
    std::ifstream in(in_path);
    if (!in.good())
        throw std::runtime_error("Error: failed to read the input file");
    std::vector<Instruction> instructions;
    std::string buf;
    while (std::getline(in, buf)){
        Instruction inst = assemble_inst(buf);
        instructions.push_back(inst);
    }
    in.close();
    // store all program strings to the file
    std::ofstream out(out_path, std::ios::binary);
    size_t string_count = this->program_strs.size();
    for (int i = 0; i < string_count; i++)
        out << '"' << program_strs[i] << '"';
    out << '\0';
    // store the bytecode to the output file
    size_t instruction_count = instructions.size();
    for (int i = 0; i < instruction_count; i++){
        uint8_t* byte_buf = instructions[i].to_bytes().data();
        out.write(reinterpret_cast<const char*>(byte_buf), INSTRUCTION_BYTES);
    }
    out.close();
}

// converts a pneumonic text insturction to a byte code instruction
Instruction Assembler::assemble_inst(const std::string& inst){
    try{
        std::vector<std::string> operands = split_str(inst);
        uint8_t op_code = this->parse_op(operands[0]);
        // if this is an unrecognized operation, treat it as a label
        if (op_code == NULL_INST)
            return this->parse_label(inst);
        // determine how to parse the instruction, based on it's type
        uint8_t op_type = (op_code & 0xE0) >> 1; // left most three bits  
        Instruction retval;
        switch (op_type){
            case MEM_OP:
                retval = parse_mem(op_code, operands);
                break;
            case LOGIC_OP:
                retval = parse_logic(op_code, operands);
                break;
            case JUMP_OP:
                retval = parse_jump(op_code, operands);
                break;
            case STACK_OP:
                retval = parse_stack(op_code, operands);
                break;
            case IO_OP:
                retval = parse_io(op_code, operands);
                break;
        }
        this->line_no++;
        return retval;
    }
    catch (std::runtime_error e){
        std::stringstream error_msg;
        error_msg << "Syntax error on line " << this->line_no + 1 << ": " << e.what();
        throw std::runtime_error(error_msg.str());
    }
}

// parses a memory operation
Instruction Assembler::parse_mem(uint8_t op_code, const std::vector<std::string>& operands){
    Instruction retval;
    retval.op_code = op_code;
    if (operands.size() != 3)
            throw std::runtime_error("invalid instruction");
    // check if this is load address, which has different parsing logic than the other memory commands
    if ((op_code >> 1) == LOAD_ADDR){
        retval.registers = parse_reg(operands[1]);
        retval.extend = parse_data_label(operands[2]);
        return retval;
    }
    // parse the typical 3 operand parse memory command
    uint8_t reg_byte = parse_reg(operands[1]);
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
    retval.registers = reg_byte;
    retval.extend = extend;
    return retval;
}

// parses a logical/arithmetic expression 
Instruction Assembler::parse_logic(uint8_t op_code, const std::vector<std::string>& operands){
    // ensure four opperands are included (opcode, dst, lhs, rhs)
    if (operands.size() != 4)
        throw std::runtime_error("invalid instruction. Operation expects four operands");
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
    uint8_t op_type = (op_code & 0xfe) >> 1;
    switch (op_type){
        case STACK_PUSH:
            // determine if we are parsing pushing an immediate or register value
            if (op_type){
                retval.registers = 0;
                retval.extend = parse_immediate(operands[1]);
            }
            else{
                retval.registers = parse_reg(operands[1]);
                retval.extend = 0;
            }
            break;
        case STACK_POP:
            retval.registers = parse_reg(operands[1]);
            retval.extend = 0;
            break;
    }
    return retval;
}

// parses a jump or function call instruction
Instruction Assembler::parse_jump(uint8_t op_code, const std::vector<std::string>& operands){
    Instruction retval;
    retval.op_code = op_code; 
    uint8_t r1, r2;
    switch ((op_code & 0xfe) >> 1){
        case JUMP:
        case CAL:
            if (operands.size() != 2)
                throw std::runtime_error("invalid instruction");
            retval.extend = parse_jmp_label(operands[1]);
            retval.registers = 0;
            break;
        case JEQ:
        case JNE:
        case JLT:
        case JGT:
            // should have the opcode, two registers, and a destination
            if (operands.size() != 4)
                throw std::runtime_error("invalid instruction");
            r1 = parse_reg(operands[1]);
            r2 = parse_reg(operands[2]);
            retval.registers = merge_registers(r1, r2);
            retval.extend = parse_jmp_label(operands[3]);
            break;
        case RET:
            break;
    }
    return retval;
}

// parses an IO operation
Instruction Assembler::parse_io(uint8_t op_code, const std::vector<std::string>& operands){
    // all IO operations take a single register, so this parsing logic is universal
    Instruction retval;
    retval.op_code = op_code;
    retval.extend = 0;
    if (operands.size() != 2)
        throw std::runtime_error("invalid io operation");
    uint8_t dst_reg = parse_reg(operands[1]);
    retval.registers = dst_reg;
    return retval;
}
