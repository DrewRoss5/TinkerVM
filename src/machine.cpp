#include <stdexcept>
#include <fstream>

#include "../inc/assembler.h"
#include "../inc/machine.h"

std::string read_str(std::ifstream& file){
    std::string retval;
    char curr_char;
    while (!file.eof()){
        file.get(curr_char);
        if (curr_char == '"')
            break;
        retval.push_back(curr_char);
    }
    if (curr_char != '"')
        throw std::runtime_error("malformed binary (invalid string)");
    return retval;   
}


Machine::Machine(){
    this->registers.fill(0);
}

void Machine::read_file(const std::string& file_path){
    std::ifstream file(file_path, std::ios::binary);
    if (!file.good())
        throw std::runtime_error("failed to read the binary");
    char curr_char;
    file.get(curr_char);
    while (curr_char != 0x0 && !file.eof()){
        std::string tmp = read_str(file);
        this->prog_strings.push_back(tmp);
        file.get(curr_char);
    }
    // read each instruction
    std::array<uint8_t, 10> inst_bytes;
    Instruction inst;
    while (!file.eof()){
        file.read(reinterpret_cast<char*>(&inst_bytes[0]), 10);
        inst = Instruction::from_bytes(inst_bytes);
        this->instructions.push_back(inst);
        this->instruction_count++;
    }
}

// reads all instructions from a tcode file and runs the program
void Machine::exec_file(const std::string& file_path){
    this->read_file(file_path);
    // keep executing the program until we run out of instructions
    while (this->registers[PROGRAM_COUNTER] != this->instruction_count)
        this->exec_next();
}

// executes the instruction that the PC currently points to and increments the PC
void Machine::exec_next(){
    size_t inst_no = this->registers[PROGRAM_COUNTER];
    this->exec_inst(this->instructions[inst_no]);
    this->registers[PROGRAM_COUNTER]++;
}

// executes a provided command
void Machine::exec_inst(const Instruction& inst){
    // parse the op code
    uint8_t op_code = inst.op_code;
    bool immediate = inst.op_code & 0x01;
    op_code &= 0xfe;
    op_code >> 1;
    // run the appropriate operation
    /*
    uint8_t op_type = (op_code & 0xE0) >> 1;
    switch (op_type){
        case MEM_OP:
            exec_mem(op_code, immediate, inst.extend);
            break;
        case LOGIC_OP:
            exec_logic(op_code, immediate, inst.extend);
            break;
        case STACK_OP:
            exec_stack(op_code, immediate, inst.extend);
            break;
        case IO_OP:
            exec_stack(op_code, immediate, inst.extend);
            break;
        default:
            throw std::runtime_error("malformed binary (invalid operation)");
            break;
    }
    */
}