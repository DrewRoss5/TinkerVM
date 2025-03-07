#include <stdexcept>
#include <cstring>
#include "../inc/stack.hpp"

Stack::Stack(){
    this->stack_ptr = new uint8_t[1000000];
    this->init_ptr = stack_ptr;
}

Stack::~Stack(){
    delete this->init_ptr;
    this->stack_ptr = nullptr;
    this->init_ptr = nullptr;
}

// pushes a new value on to the stack
void Stack::push(uint8_t* data, uint8_t data_size){
    // store the new data to the stack
    if (this->capacity < data_size)
        throw std::runtime_error("stack overflow");
    std::memcpy(this->stack_ptr, data, data_size);
    this->capacity -= data_size + 1;
    // store the allocated size as metadata
    stack_ptr += data_size;
    *stack_ptr = data_size;
    stack_ptr++;
}

// pops a value off the stack
uint8_t* Stack::pop(){
    if (this->is_empty())
        throw std::runtime_error("cannot pop a value from an empty stack");
    // read the size of the last allocated value
    stack_ptr--;
    uint8_t data_size = *(this->stack_ptr);
    // get the value of the last allocated value
    this->capacity += data_size + 1;
    stack_ptr -= data_size;
    return stack_ptr;
}