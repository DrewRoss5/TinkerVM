#ifndef MACHINE_H
#define MACHINE_H

#include <array>
#include <string>
#include <vector>
#include <tuple>

#include "../inc/instruction.h"
#include "../inc/stack.hpp"

// stores reserved register names
enum registers{
    PROGRAM_COUNTER,
    ARG_1,
    ARG_2,
    ARG_3,
    ARG_4,
    RET_VAL,
    RET_ADDR,
};

class Machine{
    public:
        Machine(bool init_default = true);
        ~Machine();
        static void split_registers(uint8_t registers, uint8_t& r1, uint8_t& r2);
        uint64_t get_register(size_t reg_no);
        void exec_next();
        void exec_file(const std::string& file_path);
        void exec_inst(const Instruction& inst);
        void set_register(size_t reg_no, uint64_t val);
        void add_extension(uint8_t op_family, void(*op)(Machine*, uint8_t, bool, uint8_t, uint64_t));
        void add_label(uint8_t* ptr);
        void add_str(const std::string& str) {this->prog_strings.push_back(str);}
        uint8_t* get_label(size_t index);
        std::string get_str(size_t index);
        Stack& get_stack() {return this->stack;}
        size_t get_inst_count() {return this->instruction_count;}
    private:
        void read_file(const std::string& file_path);     
        std::array<uint64_t, 16> registers;
        std::vector<uint8_t*> labels;
        std::vector<Instruction> instructions;
        std::vector<std::string> prog_strings;
        std::unordered_map<uint8_t, void(*)(Machine*, uint8_t, bool, uint8_t, uint64_t)> instruction_map;
        Stack stack;
        size_t instruction_count {0};
};

#endif