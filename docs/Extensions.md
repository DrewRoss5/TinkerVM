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
Users are able to implement execution for custom bytecode instructions. These are reasonably straight forward to implement. As with pneumonic instructions, users must write a function that follows a specific prototype; the prototype for bytecode instructions is: `void(*op)(Machine* machine, uint8_t op_code, bool immediate, uint8_t registers, uint64_t extend)`. The following is a breakdown of each of the parameters: 

<table>
  <tr>
    <th>Paramater:</th>
    <th>Description:</th>
  </tr>
<tr>
  <td><code>machine</code></td>
  <td>A pointer to the virtual machine executing the instruction</td>
</tr>
<tr>
  <td><code>op_code</code></td>
  <td>The leftmost seven bits of the `op_code` memeber from the Instruction object being parsed</td>
</tr>
  <tr>
    <td><code>immediate</code></td>
    <td>A bool representing if the final bit of the op-coder is one or zero (one is true, zero is false)</td>
  </tr>
<tr>
  <td><code>registers</code></td>
  <td>Corresponds to the <code>registers</code> member of the <code>Instruction</code> struct.</td>
</tr>
<tr>
  <td><code>extend</code></td>
  <td>Correspond the the `extend` member of the <code>Instruction</code> struct.</td>
</tr>
</table>

Unlike implementing pneumonic instructions, each execution function corresponds to the "family" of instructions, represented in the leftmost 3 bits of the instruction. For instance, `sub` and `add` are both handled by the same execution function, as they both belong to the `0x10` family of instructions. Once you have written your execution logic, call the `add_extension` function on the virtual machine that will run the bytecode in question, passing the instruction family as the first parameter, and the function pointer to your function in the second. (To use our earlier example: `my_machine.add_extension(0x01, &exec_logic);`
