#ifndef POW_H
#define POW_H

#include <vector>
#include <string>

#include "../inc/instruction.h"
#include "../inc/assembler.h"
#include "../inc/machine.h"

Instruction parse_square(const std::vector<std::string>& operands);
Instruction parse_pow(const std::vector<std::string>& operands);
void init_pow_assembler(Assembler& assembler);
void init_pow_machine(Machine& machine);

#endif