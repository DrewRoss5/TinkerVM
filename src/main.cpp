#include <iostream>
#include <iomanip>
#include <unordered_map>

#include "../inc/assembler.h"
#include "../inc/machine.h"
#include "../extensions/pow.h"

enum Command{
    NULL_CMD,
    HELP,
    BUILD,
    RUN,
    DEBUG,
};

void print_error(const std::string& err_msg);
Command parse_command(const std::string& command);
void print_help();
int assemble_prog(const std::string& in, const std::string& out);
int exec_prog(const std::string& in, bool debug);

int main(int argc, char** argv){
    if (argc == 1){
        print_help();
        return 0;
    }
    Command cmd = parse_command(argv[1]);
    std::string in, out;
    bool debug;
    switch (cmd){
        case NULL_CMD:
            print_error("Unrecognized command: " + cmd);
            print_help();
            return -1;
        case HELP:
            print_help();
            return 0;
        case BUILD:
            if (argc < 3 || argc > 4){
                print_error("this command expects between one and two arguments. Use 'tvm help' for more information");
                return 1;
            }
            in = argv[2];
            out = "out.tcode";
            if (argc == 4){
                out = argv[3];
                if (out.size() < 7 || out.substr(out.size() - 6) != ".tcode")
                    out.append(".tcode");
            }
            return assemble_prog(in, out);
        case RUN:
        case DEBUG:
            if (argc != 3)
                print_error("this command only accepts one argument. Use 'tvm help' for more information");
            in = argv[2];
            debug = (cmd == DEBUG);
            return exec_prog(in, debug);
    }
    return 0;
}

void print_error(const std::string& err_msg) {
    std::cout << "\033[31mError:\033[0m " << err_msg << std::endl;
}

Command parse_command(const std::string& command){
    std::unordered_map<std::string, Command> options{
        {"help", HELP},
        {"build", BUILD},
        {"run", RUN},
        {"run-debug", DEBUG}
    };
    auto cmd_itt = options.find(command);
    if (cmd_itt == options.end())
        return NULL_CMD;
    return cmd_itt->second;
}

void print_help(){
    std::string names[] = {"help", "build", "run",  "run-debug"};
    std::string args[] = {"", "<input_file> [output_file]", "<input_file>", "<input_file>"};
    std::string descriptions[] = {
        "displays this menu",
        "assembles the input_file and stores the bytecode to the output file. If no output file is provided the bytecode will be stored in out.tcode",
        "executes the provided tcode file",
        "executes the provided tcode file and displays the values of all registers at completion" 
    };
    std::cout << "Program options" << std::endl;
    for (int i = 0; i < 4; i++)
        std::cout << "\t" << std::left << std::setw(15) << names[i] << std::setw(35) << args[i] << descriptions[i] << "\n";
    std::cout << "Visit https://github.com/DrewRoss5/TinkerVM for more information" << std::endl;
}

int assemble_prog(const std::string& in, const std::string& out){
    try{
        Assembler assembler; 
        init_pow_assembler(assembler);
        assembler.assemble_file(in, out);
        std::cout << "Built " << out << " succesfully." << std::endl;
    }
    catch (std::runtime_error err){
        print_error(err.what());
        return -1;
    }
    return 0;
}

int exec_prog(const std::string& in, bool debug){
    Machine vm;
    init_pow_machine(vm);
    try{
        vm.exec_file(in);
    }
    catch (std::runtime_error err){
        print_error(err.what());
        return -1;
    }
    // print the value of each register if we're in debug mode
    if (debug){
        std::cout << "Registers:";
        for (int i = 0; i < 16; i++)
            std::cout << "\n\tR" << i << ": " << vm.get_register(i);
        std::cout << std::endl;
    }
    return 0;
}