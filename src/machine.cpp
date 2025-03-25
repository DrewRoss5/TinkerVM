#include <stdio.h>
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <cstring>
#include <unordered_map>
#include <iostream>

#include "../inc/instruction.h"
#include "../inc/machine.h"

void exec_mem(Machine* machine, uint8_t op_code, bool immediate, uint8_t registers, uint64_t extend);
void exec_logic(Machine* machine, uint8_t op_code, bool immediate, uint8_t registers, uint64_t extend);
void exec_jump(Machine* machine, uint8_t op_code, bool immediate, uint8_t registers, uint64_t extend);
void exec_stack(Machine* machine, uint8_t op_code, bool immediate, uint8_t registers, uint64_t extend);
void exec_io(Machine* machine, uint8_t op_code, bool immediate, uint8_t registers, uint64_t extend);
void exec_heap(Machine* machine, uint8_t op_code, bool immediate, uint8_t registers, uint64_t extend);

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
void Machine::split_registers(uint8_t registers, uint8_t& r1, uint8_t& r2){
    r2 = registers & 0x0f;
    r1 = registers & 0xf0;
    r1 >>= 4;
}

Machine::Machine(bool init_default){
    this->registers.fill(0);
    if (init_default){
        this->add_extension(0x00, exec_mem);
        this->add_extension(0x10, exec_logic);
        this->add_extension(0x20, exec_jump);
        this->add_extension(0x30, exec_stack);
        this->add_extension(0x40, exec_io);
        this->add_extension(0x50, exec_heap);
    }
}

Machine::~Machine(){
    size_t label_count = this->labels.size(); 
    for (int i = 0; i < label_count; i++)
        delete this->labels[i];
}

// sets the value of the given register
void Machine::set_register(size_t reg_no, uint64_t val){
    this->registers[reg_no] = val;
}

// returns the value of the given register
uint64_t Machine::get_register(size_t reg_no){
    return this->registers[reg_no];
}

// adds a label to the machine
void Machine::add_label(uint8_t* ptr){
    this->labels.push_back(ptr);
}

// returns the pointer of a label at the given index
uint8_t* Machine::get_label(size_t index){
    if (index >= this->labels.size())
        throw std::runtime_error("invalid label");
    return this->labels[index];
}

// returns the program string at a given index
std::string Machine::get_str(size_t index){
    if (index >= this->prog_strings.size())
        throw std::runtime_error("invalid string index");
    return this->prog_strings[index];
}

// adds an extension and associates it with an operation family
void Machine::add_extension(uint8_t op_family, void(*op)(Machine*, uint8_t, bool, uint8_t, uint64_t)){
    // check if the operation family is already implemented by another extension
    if (this->instruction_map.count(op_family))
        throw std::runtime_error("invalid operation family (in use by another extension)");
    // ensure the operation family can be stored in 3 bits
    if (op_family > 0x80)
        throw std::runtime_error("invalid operation family (out of range)");
    this->instruction_map[op_family] = op;
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
    auto itt = this->instruction_map.find(op_type);
    if (itt == this->instruction_map.end())
        throw std::runtime_error("malformed binary (invalid operation)");
    itt->second(this, op_code, immediate, inst.registers, inst.extend);
}

// executes a memory operation
void exec_mem(Machine* machine, uint8_t op_code, bool immediate, uint8_t registers, uint64_t extend){
    uint8_t reg_1, reg_2;
    uint64_t rhs, tmp, val;
    uint8_t* ptr;
    size_t size;
    std::string str;
    if (immediate){
        reg_1 = registers;
        rhs = extend;
    }
    else {
        machine->split_registers(registers, reg_1, reg_2);
        val = machine->get_register(reg_2);
    }
    switch (op_code){
        case COPY:
            val = machine->get_register(reg_2);
            machine->set_register(reg_1, val);
            break;
        case STORE_WORD:
            // copy rhs to the address stored in r2
            ptr = reinterpret_cast<uint8_t*>(machine->get_register(reg_2));
            std::memcpy(ptr, &rhs, 8);
            break;
        case STORE_BYTE:
            // store the rightmost byte of rhs to the address in r1
            ptr = reinterpret_cast<uint8_t*>(machine->get_register(reg_1));
            rhs &= 0x0f;
            *ptr = rhs;
            break;
        case LOAD_WORD:
            if (immediate)
                machine->set_register(reg_1, rhs);
            else{
                // read in the word pointed to by reg_2
                ptr = reinterpret_cast<uint8_t*>(machine->get_register(reg_2));
                std::memcpy(&val, ptr, 8);
                machine->set_register(reg_1, val);
            }
            break;
        case LOAD_BYTE:
            ptr = reinterpret_cast<uint8_t*>(machine->get_register(reg_2));
            val = (*ptr) & 0x000f;
            break;
        case ALLOC_MEM:
            size = extend;
            machine->add_label(new uint8_t(size));
            break;
        case ALLOC_STR:
            str = machine->get_str(extend);
            // create a char array to the string
            ptr = new uint8_t[str.size()];
            std::copy(str.begin(), str.end(), ptr);
            machine->add_label(ptr);
            break;
        case LOAD_ADDR:
            ptr = machine->get_label(extend - 1);
            tmp = reinterpret_cast<uint64_t>(ptr);
            machine->set_register(registers, tmp);
            break;
    }
}

// executes an arithmatic/logical operation
void exec_logic(Machine* machine, uint8_t op_code, bool immediate, uint8_t registers, uint64_t extend){
    // parse the extent 
    uint8_t src_reg, dst_reg;
    uint64_t rhs, lhs, res;
    machine->split_registers(registers, dst_reg, src_reg);
    lhs = machine->get_register(src_reg);
    // determine if the right hand side is stored in a register or is an immediate value
    if (immediate)
        rhs = extend;
    else
        rhs = machine->get_register(extend);
    // perfrom the given operation and store it to the destination register
    switch (op_code){
        case ADD:
            res = lhs + rhs;
            break;
        case SUB:
            res = lhs - rhs;
            break;
        case MUL:
            res = lhs * rhs;
            break;
        case DIV:
            res = lhs / rhs;
            break;
        case REM:
            res = lhs % rhs;
            break;
        case COMP:
            res = lhs == rhs;
            break;
        case AND:
            res = lhs & rhs;
            break;
        case OR:
            res = lhs | rhs;
            break;
        case XOR:
            res = lhs ^ rhs;
            break;
        case SR:
            res = lhs >> rhs;
            break;
        case SL:
            res = lhs << rhs;
            break;
    }
    machine->set_register(dst_reg, res);
}

// execute a jump operation
void exec_jump(Machine* machine, uint8_t op_code, bool immediate, uint8_t registers, uint64_t extend){
    uint8_t r1,  r2;
    if (op_code == JUMP){
        machine->set_register(PROGRAM_COUNTER, extend);
        return;
    }
    machine->split_registers(registers, r1,r2);
    uint64_t lhs = machine->get_register(r1);
    uint64_t rhs = machine->get_register(r2);
    uint64_t tmp;
    // determine the operation to perform
    switch (op_code)
    {
        case JEQ:
            // jump only if the registers are equal
            if (lhs == rhs)
                machine->set_register(PROGRAM_COUNTER, extend);
            break;
        case JNE:
            // jump only if the registers are not equal
            if (lhs != rhs)
                machine->set_register(PROGRAM_COUNTER, extend);
            break;
        case JGT:
            // jump only if the lhs is greater than rhs
            if (lhs > rhs)
                machine->set_register(PROGRAM_COUNTER, extend);
            break;
        case JLT:
            // jump only if the lhs is greater than rhs
            if (lhs < rhs)
                machine->set_register(PROGRAM_COUNTER, extend);
            break;
        case CAL:
            // store the current return address and jump to the function
            tmp = machine->get_register(PROGRAM_COUNTER);
            machine->set_register(RET_ADDR, tmp);
            machine->set_register(PROGRAM_COUNTER, extend);
            break;
        case RET:
            // jumps to the current return address
            tmp = machine->get_register(RET_ADDR);
            machine->set_register(PROGRAM_COUNTER, tmp);
            machine->set_register(RET_ADDR, machine->get_inst_count());
            break;
    }
}

void exec_stack(Machine* machine, uint8_t op_code, bool immediate, uint8_t registers, uint64_t extend){
    uint8_t reg = registers & 0x0f;
    uint64_t rhs;
    if (immediate)
        rhs = extend;
    else
        rhs = machine->get_register(reg);
    Stack stack = machine->get_stack();
    uint64_t val;
    switch (op_code){
        case PUSH:
            // push the value of the register onto the stack
            stack.push(rhs); 
            break;
        case PUSH_B:
            // push the rightmost byte of the register onto the stack
            stack.push(static_cast<uint8_t>(rhs & 0xff));
            break;
        case POP:
            if (stack.is_empty())
                throw std::runtime_error("no values on the stack to pop!");
            val = stack.pop_type<uint64_t>();
            machine->set_register(reg, val);
            break;
        case POP_B:
            if (stack.is_empty())
                throw std::runtime_error("no values on the stack to pop!");
            machine->set_register(reg, *stack.pop());
            break;
    }
}

// executes an IO operation
void exec_io(Machine* machine, uint8_t op_code, bool immediate, uint8_t registers, uint64_t extend){
    uint8_t reg = registers & 0x0f;
    uint8_t index;
    uint64_t input_int;
    const char* str;
    std::string input;
    switch (op_code){
        case PUT_S:
            str = reinterpret_cast<const char*>(machine->get_register(reg));
            printf("%s", str);
            break;
        case PUT_I:
            printf("%lu", machine->get_register(reg));
            break;
        case GET_S:
            std::getline(std::cin, input);
            machine->add_str(input);
            str = input.c_str();
            machine->set_register(reg, reinterpret_cast<uint64_t>(str));
            break;
        case GET_I:
            std::cin >> input_int;
            machine->set_register(reg, input_int);
            break;
    }
}

void exec_heap(Machine *machine, uint8_t op_code, bool immediate, uint8_t registers, uint64_t extend){
    uint8_t* ptr;
    uint8_t reg, _;
    machine->split_registers(registers, reg, _);
    switch (op_code){
        case HEAP_ALLOC:
            // allocate memory of the specified size and store the pointer in the desitnation register
            ptr = new uint8_t[extend];
            machine->set_register(reg, reinterpret_cast<uint64_t>(ptr));
            break;
        case HEAP_FREE:
            // free the memory in the given register
            ptr = reinterpret_cast<uint8_t*>(machine->get_register(reg));
            delete ptr;
            break;
    }
}