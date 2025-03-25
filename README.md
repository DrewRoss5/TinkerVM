# ⚙️ TinkerVM ⚙️
TinkerVM is a minimalist stack-based open-source virtual machine designed for flexibility and easy modification. It uses a custom bytecode format called tcode to interpret programs. TinkerVM also includes an assembler to provide support for a simple assembly-style language that is directly translated into tcode.

## Project Status:
While TinkerVM is currently in the early stages of development, it is open for exploration and implementation, and feedback is encouraged! Here is the state of the project as of March 9th, 2025:
### Current Features:
- An assembler to support a human-readable version of tcode
- Basic instructions:
    - Memory manipulation
    - Arithmetic and logical instructions
    - Stack Manipulation
    - Jump instructions and functions
    - Simple I/O
### Planned Features (Project Roadmap):
- A framework for implementing custom commands and extensions -- IN PROGRESS
  - Create an interface for implementing custom instructions in the assembler and VM itself ☑️
  - Convert native VM instructions to extensions for maximum customization ☑️
  - Release documentation for creating extensions ☐
- Floating point support
- File I/O support
- System Calls
- A TVM library

# Getting Started:
TinkerVM only relies on the C++ standard library with no external dependencies, and is configured using CMake, so to compile the executable, simply clone this repository and run the following commands:
```
mkdir build
cd build
cmake ..
make
```
This will generate a `tvm` executable which can be used to assemble tinkerassembly and run tcode. 

# Usage:
## Supported commands: 
- `build <input_file> [output_file]`:
  - Assembles the input_file and stores the bytecode to the output file. If no output file is provided the bytecode will be stored in out.tcode
- `run <input_file>`:
  - Executes the provided tcode file.
- `run-debug <input_file>`:
  - Executes the provided tcode file, and displays the values of all registers once the program exits.
