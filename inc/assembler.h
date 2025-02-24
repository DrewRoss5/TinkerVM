#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <vector>
#include <string>
#include <unordered_map>

#include "instruction.h"

#define NULL_INST 255

enum op_types {MEM_OP = 0x00, 
               LOGIC_OP = 0x10,
               JUMP_OP = 0x20,
               STACK_OP = 0x30,
               IO_OP = 0x40
            };

std::vector<std::string> split_str(std::string& str);

class Assembler{
    public:
        Assembler() {}
        uint8_t parse_op(const std::string& op);
        uint8_t parse_reg(const std::string& reg);
        uint8_t merge_registers(uint8_t r1, uint8_t r2);
        Instruction parse_inst(std::string& inst);
        Instruction parse_mem(uint8_t op_code, const std::vector<std::string>& operands);
        Instruction parse_logic(uint8_t op_code, const std::vector<std::string>& operands);
        Instruction parse_stack(uint8_t op_code, const std::vector<std::string>& operands);
        Instruction parse_io(uint8_t op_code, const std::vector<std::string>& operands);
    private:
        /* this associates each pneumonic with a bytecode instruction, the first element of
           the tuple represents the op-code and the second part represents the immediate flag 
           1 for immediate operations, 0 for not*/
        std::unordered_map<std::string, std::pair<uint8_t, bool> > op_map{
            {"copy",    {0x00, 0}},
            {"store",   {0x01, 0}},
            {"stori",   {0x01, 1}},
            {"load",    {0x02, 0}},
            {"loadi",   {0x02, 1}},
            {"add",     {0x10, 0}},
            {"addi",    {0x10, 1}},
            {"sub",     {0x11, 0}},
            {"subi",    {0x11, 1}},
            {"mul",     {0x12, 0}},
            {"muli",    {0x12, 1}},
            {"div",     {0x13, 0}},
            {"divi",    {0x13, 1}},
            {"rem",     {0x16, 0}},
            {"remi",    {0x16, 1}},
            {"cmp",     {0x17, 0}},
            {"and",     {0x18, 0}},
            {"andi",    {0x18, 1}},
            {"or",      {0x19, 0}},
            {"ori",     {0x19, 1}},
            {"xor",     {0x1a, 0}},
            {"xori",    {0x1a, 1}},
            {"sr",      {0x1b, 1}},
            {"sl",      {0x1c, 1}},
            {"j",       {0x20, 0}},
            {"jeq",     {0x21, 0}},
            {"jne",     {0x22, 0}},
            {"jgt",     {0x23, 0}},
            {"jlt",     {0x24, 0}},
            {"call",    {0x25, 0}},
            {"ret",     {0x26, 0}},
            {"push",    {0x30, 0}},
            {"pushi",   {0x30, 1}},
            {"pop",     {0x31, 0}},
            {"puts",    {0x40, 0}},
            {"puti",    {0x41, 0}},
            {"gets",    {0x42, 0}},
            {"geti",    {0x43, 0}}
        };
};

#endif