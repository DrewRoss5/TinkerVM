#include <array>
#include <string>
#include <vector>

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
        Machine();
        void exec_next();
        void exec_file(const std::string& file_path);
        void exec_inst(const Instruction& inst);
    private:
        void read_file(const std::string& file_path);     
        void exec_mem(uint8_t op_code, bool immediate, uint64_t extend);
        void exec_logic(uint8_t op_code, bool immediate, uint64_t extend);
        void exec_stack(uint8_t op_code, bool immediate, uint64_t extend);
        void exec_io(uint8_t op_code, bool immediate, uint64_t extend);
        std::array<uint64_t, 16> registers;
        std::vector<Instruction> instructions;
        std::vector<std::string> prog_strings;
        Stack stack;
        size_t instruction_count {0};
};