#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <vector>
#include <string>
#include <unordered_map>

#include "instruction.h"

#define NULL_INST 255


std::vector<std::string> split_str(const std::string& str);
std::string parse_str_lit(std::string& str_lit, bool null_terminate);
 
class Assembler{
    public:
        Assembler() {}
        void assemble_file(const std::string& in_path, const std::string& outpath);
        Instruction assemble_inst(const std::string& inst);
    private:
        uint8_t parse_op(const std::string& op);
        uint8_t parse_reg(const std::string& reg);
        uint8_t merge_registers(uint8_t r1, uint8_t r2);
        uint64_t parse_immediate(const std::string& imm);
        uint64_t parse_jmp_label(const std::string& label);
        uint64_t parse_data_label(const std::string& label);
        Instruction parse_label(const std::string& label);
        Instruction parse_mem(uint8_t op_code, const std::vector<std::string>& operands);
        Instruction parse_logic(uint8_t op_code, const std::vector<std::string>& operands);
        Instruction parse_stack(uint8_t op_code, const std::vector<std::string>& operands);
        Instruction parse_jump(uint8_t op_code, const std::vector<std::string>& operands);
        Instruction parse_io(uint8_t op_code, const std::vector<std::string>& operands);
        void scan_prog_labels(std::vector<std::string>& lines);
        size_t next_label {0};
        size_t line_no {0};
        size_t instruction_count{0};
        std::unordered_map<std::string, size_t> data_labels;
        std::unordered_map<std::string, size_t> program_labels;
        std::vector<std::string> program_strs;
        std::vector<Instruction> instructions;
        /* this associates each pneumonic with a bytecode instruction, the first element of
           the tuple represents the op-code and the second part represents the immediate flag 
           1 for immediate operations, 0 for not*/
        std::unordered_map<std::string, std::pair<uint8_t, bool> > op_map{
            {"copy",    {0x00, 0}},
            {"stow",    {0x01, 0}},
            {"stowi",   {0x01, 1}},
            {"stob",    {0x02, 0}},
            {"stobi",   {0x02, 1}},
            {"loadw",   {0x03, 0}},
            {"loadi",   {0x03, 1}},
            {"loadb",   {0x04, 0}},
            {"loada",   {0x07, 0}},
            {"add",     {0x10, 0}},
            {"addi",    {0x10, 1}},
            {"sub",     {0x11, 0}},
            {"subi",    {0x11, 1}},
            {"mul",     {0x12, 0}},
            {"muli",    {0x12, 1}},
            {"div",     {0x13, 0}},
            {"divi",    {0x13, 1}},
            {"rem",     {0x14, 0}},
            {"remi",    {0x14, 1}},
            {"comp",    {0x15, 0}},
            {"and",     {0x16, 0}},
            {"andi",    {0x16, 1}},
            {"or",      {0x17, 0}},
            {"ori",     {0x17, 1}},
            {"xor",     {0x18, 0}},
            {"xori",    {0x18, 1}},
            {"sr",      {0x19, 1}},
            {"sl",      {0x1a, 1}},
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
        std::unordered_map<std::string, int> type_map{
            {".word", WORD},
            {".string", STRING},
            {".stringz", STRINGZ},
            {".data", DATA}
        };
};

#endif