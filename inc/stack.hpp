
#ifndef TINKER_STACK_H
#define TINKER_STACK_H

#include <cstdlib>
#include <inttypes.h>

#include "../inc/util.hpp"

class Stack{
    public:
        Stack();
        ~Stack();
        void push(uint8_t* data, uint8_t data_size);
        template <typename T>
        void push(T data);
        uint8_t* pop();
        template <typename T>
        T pop_type();
        bool is_empty() {return (this->stack_ptr == this->init_ptr);}
        size_t size() {return 1000000 - this->capacity;}
    private:
        uint8_t* stack_ptr {nullptr};
        uint8_t* init_ptr {nullptr};
        size_t capacity {1000000};
};

template <typename T>
void Stack::push(T data){
    uint8_t* bytes = new uint8_t[sizeof(T)];
    split_bytes(data, bytes);
    this->push(bytes, sizeof(T));
    delete bytes;
}

template <typename T>
T Stack::pop_type(){
    uint8_t* data = this->pop();
    return merge_bytes<T>(data);
}

#endif