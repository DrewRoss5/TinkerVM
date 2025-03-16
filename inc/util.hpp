#ifndef TINKER_UTILS
#define TINKER_UTILS

#include <inttypes.h>
#include <cstdlib>

// converts a uint8_t array to another byte size
template <typename T>
T merge_bytes(const uint8_t* data){
    T retval = 0;
    size_t dat_size = sizeof(T);
    for (int i = 0; i < dat_size; i++){
        int shift_val = 8 * (dat_size - i - 1);
        retval += data[i] << shift_val;
    }
    return retval;
}

// converts an unsigned integer type to a given byte array
template <typename T>
void split_bytes(T data, uint8_t* out){
    size_t dat_size = sizeof(T);
    T mask_val = 0xff << (8 * (dat_size - 1));
    for (int i = 0; i < dat_size; i++){
        int shift_val = 8 * (dat_size - i - 1);
        out[i] = (data & mask_val) >> shift_val;
        mask_val >>= 8;
    }
}

#endif