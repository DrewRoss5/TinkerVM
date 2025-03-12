# TinkerVM Assembly
The TinkerVM assembly language (TASM) is a human readable version of Tcode, which is natively run by TVM.  Despite being human-readable, TASM is not intended as a stand-alone language for developing software, but rather it is intended to demonstrate TVM and serve as a potential compilation target for languages that may use TinkerVM as runtime environment.
# Registers
TinkerVM has 16 registers, which are referenced by the names
r0-r15
The purposes of these registers is as follows:
<table>
<tr>
<th>Register Number/Range</th>
<th>Purpose</th>
</tr>
<tr>
<td>r0</td>
<td>Program Counter, stores the index of the next instruction to execute.</td>
</tr>
<tr>
<td>r1-r4</td>
<td>Argument registers, store arguments for function calls (functions requiring more than four arguments should use the stack to pass arguments.</td>
</tr>
<tr>
<td>r5</td>
<td>Return Value, stores the value returned from a function</td>
</tr>
<tr>
<td>r6</td>
<td>Return address, stores the index of the instruction that the `ret` operation will jump to. This is automatically set with the `call` operation</td>
</tr>
<tr>
<td>r7-r15</td>
<td>General Purpose</td>
</tr>
</table>

# Operations
Currently, there are five basic types operations in TinkerVM
<ul>
	<li>Memory Manipulation</li>
	<li>Logical/Arithmetic operations</li>
	<li>Jump operations</li>
	<li>Stack operations</li>
	<li>I/O operations</li>
</ul>

#### Note:
Operations that support immediate values may replace their rightmost register with an immediate (literal) value, immediate operations are followed by an `i`, for instance:,
`add` adds two registers where `addi` adds the value of a register and an immediate value.
### Memory Manipulation:
#### `copy <r0> <r1>`:
- Copies the value of `r1` to `r0`

#### `stow/stob <r0> <r1>`:
- Stores the value of `r1` to the address stored in `r0`
- `stow` stores a 64-bit word, and `stob` stores a byte
- Supports immediate values

#### `loadw/loadb <r0> <r1>`
- Stores the value in the address pointed to by `r1` to `r0`
- `loadw` loads a word, `loadb` loads a byte
- Supports immediate values

#### `loada <r0> label`
- Stores the memory address of the given data label to `r0`, allowing it to be used by the above operations.

### Arithmetic/Logical Operations:
All logical and arithmetic operations use the same syntax:
`<operation> <r0> <r1> <rhs>`
Where `r0` is set to the result of `operation` on `r1` and `rhs`
For instance, `add r9 r7 r8`
Stores `r7` + `r8` to `r9`. RHS can be either a register or an immediate value.
#### Supported Operations
- `add`
- `sub`
- `mul`
- `div`
- `rem` (modulo operation)
- `and`
- `or`
- `xor`
- `sl` (bitwise shift left. Does not support immediate values)
- `sr` (bitwise shift right. Does not support immediate values)
- `comp` (comparison, evaluates to `r1 == rhs`)

### Jump Operations:
#### `j <label>`
- Jumps to `label`
#### `jeq <r1> <r2> <label>`
- Jumps to `label` if `r1` is equal to `r2`
#### `jne <r1> <r2> <label>`
- Jumps to `label` if `r1` is not equal to `r2`
#### `jgt <r1> <r2> <label>`
- Jumps to `label` if `r1` is greater than `r2`
#### `jlt <r1> <r2> <label>`
- Jumps to `label` if `r1` is less than `r2`
#### `call <label>`
- Jumps to `label` and sets the return address register to the current place in the program
- This is used to implement functions
#### `ret`
- Jumps to the position stored in the reeturn address register

### Stack Operations:
#### `push <r0>/pushb <r0>`:
- Pushes the value of r0 to the program stack
- `push` pushes a 64-bit word and `pushb` pushes a byte
- Supports immediate values

#### `pop <r0>/popb <r0>`:
- Pops the top value off of the program stack and stores it to `r0`
- `pop` pops a 64 bit word
- `popb` pops a single byte

### I/O Operations:
#### `puts <r0>`
- Prints the string at the address stored in `r0`
#### `puti <r0>`
- Prints the 64-bit integer value of `r0`
#### `gets <r0>`
- Reads a string from stdin and stores the address of the read string to r0
#### `geti <r0>`
- Reads a 64-bit integer from stdin and stores it to `r0`
