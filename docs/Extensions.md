# Creating Extensions:
## Getting Started:
Currently, creating extension requires the original source code of TinkerVM, to create custom parsing logic for pneumonic instructions, you need to include the `inc/assembler.h` header file, 
and to create custom byte instructions, you need to include the `inc/machine.h` header file.

# Pneumonic Instructions:
Users are able to implement parsing logic for custom TinkerAssembly instructions, each instruction corresponds to exactly one parsing function. These parsing functions must use the following
prototype: <br> `Instruction <func_name>(const std::vector<std::string>& operands)`. 
## Operands:
The operands represent each token (including the instruction itself) in the operation, and are split by spaces.
For instance, a parsing function parsing `add r1 r2 r3` would be passed `{add, r1, r2, r3}` as the value of operands.  
## Creating Instructions:
Every parser function must return an "Instruction" object. The instruction struct has four members that can be set, and all are default intialized to zero. 

<table>
  <tr>
    <th>Member:</th>
    <th>Type:</th>
    <th>Description:</th>
  </tr>
  <tr>
    <td>op_code</td> 
    <td>uint8_t</td>
    <td>
      Stores the unique "id" of each instruction type (e.g, add, sub, copy, etc.). Only the first seven bits of of this should represent the op code itself, with the last bit set
        to one if the instruction is immediate (e.g, addi, loadi, etc.) and zero if it isn't.
    </td>
  </tr>
  <tr>
    <td>registers</td>
    <td>uint8_t</td>
    <td>
      Stores two four-bit register numbers, usually source and destination registers. By convention, the left-hand register correpsonds to the leftmost four bits, and the right-hand register
      corresponds to the rightmost four bits.
    </td>
  </tr>
  <tr>
    <td>extend</td>
    <td>uint64_t</td>
    <td>
      Stores any additional information the instruction needs, such as memory addresses or immediate values.
    </td>
  </tr>
</table>

## Helper Functions:
The following are utility functions for writing parsing fucnctions.
#### `uint8_t Assembler::parse_reg(const std::string& reg):`
Parses a pneumonic register number (for example, r7) and returns the represented number. Throws a `std::runtime_error` if the register is invalid.

#### `uint8_t Assembler::merge_registers(uint8_t r1, uint8_t r2)`:
Merges two registers into one 8-bit value, for conveinenty setting the `registers` value for the returned instruction.

# Bytecode Instructions:
Coming soon.
