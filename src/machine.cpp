#include <stdio.h>
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <cstring>
#include <unordered_map>
#include <iostream>

#include "../inc/instruction.h"
#include "../inc/machine.h"

// reads a string literal wrapped in quotations from a file
std::string read_str(std::ifstream& file){
    std::unordered_map<char, char> escapes{
        {'n', '\n'},
        {'t', '\t'},
        {'b', '\b'},
        {'v', '\v'},
        {'f', '\f'},
        {'r', '\r'}
    };
    std::string retval;
    std::string str;
    char curr_char;
    while (true){
        file >> std::noskipws >> curr_char;
        if (curr_char == '"')
            break;
        else if (curr_char == '\\'){
            file >> std::noskipws >> curr_char;
            if (file.eof())
                throw std::runtime_error("malformed binary (invalid string)");
            if (escapes.count(curr_char))
                curr_char = escapes[curr_char];
        }
        retval.push_back(curr_char);
    }
    if (curr_char != '"')
        throw std::runtime_error("malformed binary (invalid string)");
    return retval;   
}



// splits a merged register into two seperate values
void split_registers(uint8_t registers, uint8_t& r1, uint8_t& r2){
    r2 = registers & 0x0f;
    r1 = registers & 0xf0;
    r1 >>= 4;
}

Machine::Machine(){
    this->registers.fill(0);
}

Machine::~Machine(){
    size_t label_count = this->labels.size(); 
    for (int i = 0; i < label_count; i++)
        delete this->labels[i];
}

// returns the value of the given register
uint64_t Machine::get_register(size_t reg_no){
    return this->registers[reg_no];
}

// reads all the instructions from a bytecode file
void Machine::read_file(const std::string& file_path){
    std::ifstream file(file_path);
    file >> std::noskipws;
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
        if (file.eof())
            break;
        inst = Instruction::from_bytes(inst_bytes);
        this->instructions.push_back(inst);
        this->instruction_count++;
    }
    // set the return value to exit the program if called outside of a function
    this->registers[RET_ADDR] = this->instruction_count + 1;
}

// reads all instructions from a tcode file and runs the program
void Machine::exec_file(const std::string& file_path){
    this->read_file(file_path);
    // keep executing the program until we run out of instructions
    while (this->registers[PROGRAM_COUNTER] < this->instruction_count)
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
    // run the appropriate operation
    uint8_t op_type = (op_code & 0xE0) >> 1;
    op_code &= 0xfe;
    op_code >>= 1;
    switch (op_type){
        case MEM_OP:
            exec_mem(op_code, immediate, inst.registers, inst.extend);
            break;
        case LOGIC_OP:
            exec_logic(op_code, immediate, inst.registers, inst.extend);
            break;
        case JUMP_OP:
            exec_jump(op_code, immediate, inst.registers, inst.extend);
            break;
        case STACK_OP:
            exec_stack(op_code, immediate, inst.registers, inst.extend);
            break;
        case IO_OP:
            exec_io(op_code, immediate, inst.registers, inst.extend);
            break;
        default:
            throw std::runtime_error("malformed binary (invalid operation)");
            break;
    }
}

// executes a memory operation
void Machine::exec_mem(uint8_t op_code, bool immediate, uint8_t registers, uint64_t extend){
    uint8_t reg_1, reg_2, val;
    uint64_t rhs, tmp;
    uint8_t* ptr;
    size_t size;
    std::string str;
    if (immediate){
        reg_1 = registers;
        rhs = extend;
    }
    else {
        split_registers(registers, reg_1, reg_2);
        rhs = this->registers[reg_2];
    }
    switch (op_code){
        case COPY:
            this->registers[reg_1] = this->registers[reg_2];
            break;
        case STORE_WORD:
            // copy rhs to the address stored in r2
            ptr = reinterpret_cast<uint8_t*>(this->registers[reg_2]);
            std::memcpy(ptr, &rhs, 8);
            break;
        case STORE_BYTE:
            // store the rightmost byte of rhs to the address in r1
            ptr = reinterpret_cast<uint8_t*>(this->registers[reg_1]);
            rhs &= 0x000f;
            *ptr = rhs;
            break;
        case LOAD_WORD:
            if (immediate)
                this->registers[reg_1] = rhs;
            else{
                // read in the word pointed to by reg_2
                ptr = reinterpret_cast<uint8_t*>(this->registers[reg_2]);
                std::memcpy(&(this->registers[reg_1]), ptr, 8);
            }
            break;
        case LOAD_BYTE:
            ptr = reinterpret_cast<uint8_t*>(this->registers[reg_2]);
            val = (*ptr) & 0x000f;
            break;
        case ALLOC_MEM:
            size = extend;
            labels.push_back(new uint8_t[size]);
            break;
        case ALLOC_STR:
            str = prog_strings[extend];
            // create a char array to the string
            ptr = new uint8_t[str.size()];
            std::copy(str.begin(), str.end(), ptr);
            labels.push_back(ptr);
            break;
        case LOAD_ADDR:
            tmp = reinterpret_cast<uint64_t>(this->labels[extend - 1]);
            this->registers[reg_2] = tmp;
            break;
        default:
            break;
    }
}

// executes an arithmatic/logical operation
void Machine::exec_logic(uint8_t op_code, bool immediate, uint8_t registers, uint64_t extend){
    // parse the extent 
    uint8_t src_reg, dst_reg;
    uint64_t rhs;
    split_registers(registers, dst_reg, src_reg);
    // determine if the right hand side is stored in a register or is an immediate value
    if (immediate)
        rhs = extend;
    else
        rhs = this->registers[extend];
    // perfrom the given operation and store it to the destination register
    switch (op_code){
        case ADD:
            this->registers[dst_reg] = this->registers[src_reg] + rhs;
            break;
        case SUB:
            this->registers[dst_reg] = this->registers[src_reg] - rhs;
            break;
        case MUL:
            this->registers[dst_reg] = this->registers[src_reg] * rhs;
            break;
        case DIV:
            this->registers[dst_reg] = this->registers[src_reg] / rhs;
            break;
        case REM:
            this->registers[dst_reg] = this->registers[src_reg] % rhs;
            break;
        case COMP:
            this->registers[dst_reg] = (this->registers[src_reg] == rhs);
        case AND:
            this->registers[dst_reg] = this->registers[src_reg] & rhs;
            break;
        case OR:
            this->registers[dst_reg] = this->registers[src_reg] | rhs;
            break;
        case XOR:
            this->registers[dst_reg] = this->registers[src_reg] ^ rhs;
            break;
        case SR:
            this->registers[dst_reg] = this->registers[src_reg] >> rhs;
            break;
        case SL:
            this->registers[dst_reg] = this->registers[src_reg] << rhs;
            break;
    }
}

// execute a jump operation
void Machine::exec_jump(uint8_t op_code, bool immediate, uint8_t registers, uint64_t extend){
    uint8_t lhs,  rhs;
    split_registers(registers, lhs, rhs);
    // determine the operation to perform
    switch (op_code)
    {
        case JUMP:
            this->registers[PROGRAM_COUNTER] = extend;
            break;
        case JEQ:
            // jump only if the registers are equal
            if (this->registers[lhs] == this->registers[rhs])
                this->registers[PROGRAM_COUNTER] = extend;
            break;
        case JNE:
            // jump only if the registers are not equal
            if (this->registers[lhs] != this->registers[rhs])
                this->registers[PROGRAM_COUNTER] = extend;
            break;
        case JGT:
            // jump only if the lhs is greater than rhs
            if (this->registers[lhs] > this->registers[rhs])
                this->registers[PROGRAM_COUNTER] = extend;
            break;
        case JLT:
            // jump only if the lhs is greater than rhs
            if (this->registers[lhs] > this->registers[rhs])
                this->registers[PROGRAM_COUNTER] = extend;
            break;
        case CAL:
            // store the current return address and jump to the function
            this->registers[RET_ADDR] = this->registers[PROGRAM_COUNTER];
            this->registers[PROGRAM_COUNTER] = extend;
            break;
        case RET:
            // jumps to the current return address
            this->registers[PROGRAM_COUNTER] = this->registers[RET_ADDR];
            this->registers[RET_ADDR] = this->instruction_count + 1;
            break;
    }
}

void Machine::exec_stack(uint8_t op_code, bool immediate, uint8_t registers, uint64_t extend){
    uint8_t reg = registers & 0x0f;
    uint64_t rhs;
    if (immediate)
        rhs = extend;
    else
        rhs = this->registers[reg];
    switch (op_code){
        case PUSH:
            // push the value of the register onto the stack
            this->stack.push(rhs); 
            break;
        case PUSH_B:
            // push the rightmost byte of the register onto the stack
            this->stack.push(static_cast<uint8_t>(rhs & 0xff));
            break;
        case POP:
            if (this->stack.is_empty())
                throw std::runtime_error("no values on the stack to pop!");
            this->registers[reg] = stack.pop_type<uint64_t>();
            break;
        case POP_B:
            if (this->stack.is_empty())
                throw std::runtime_error("no values on the stack to pop!");
            this->registers[reg] = *stack.pop();
            break;
        default:
            break;
    }
}

// executes an IO operation
void Machine::exec_io(uint8_t op_code, bool immediate, uint8_t registers, uint64_t extend){
    uint8_t reg = registers & 0x0f;
    uint8_t index;
    uint64_t input_int;
    const char* str;
    std::string input;
    switch (op_code){
        case PUT_S:
            str = reinterpret_cast<const char*>(this->registers[reg]);
            printf("%s", str);
            break;
        case PUT_I:
            printf("%lu", this->registers[reg]);
            break;
        case GET_S:
            std::getline(std::cin, input);
            this->prog_strings.push_back(input);
            str = (prog_strings.end() - 1)->c_str();
            this->registers[reg] = reinterpret_cast<uint64_t>(str);
            break;
        case GET_I:
            std::cin >> input_int;
            this->registers[reg] = input_int;
            break;
    }
}