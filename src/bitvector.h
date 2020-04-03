//
// Created by Kristopher Pellizzi on 27/03/2020.
//

#include <stdlib.h>

#ifndef SIMPLECIPHER_BITVECTOR_H
#define SIMPLECIPHER_BITVECTOR_H

#endif //SIMPLECIPHER_BITVECTOR_H

struct bitvector4{
    int repr[4];
} typedef bitvector4;

struct bitvector16 {
    int repr[16];
} typedef bitvector16 ;

struct bitvector80{
    int repr[80];
} typedef bitvector80;

struct bitvector{
    int* repr;
    unsigned int size;
} typedef bitvector;

struct bitvector80_value{
    u_int64_t MSB;
    u_int16_t LSB;
} typedef bitvector80_value;

bitvector4* get_bitvector4(u_int8_t val);
u_int8_t get_val4(bitvector4* vector);
void print_bitvector4(bitvector4* vector);

bitvector16* get_bitvector16(u_int16_t val);
u_int16_t get_val16(bitvector16* vector);
void print_bitvector16(bitvector16* vector);

bitvector80* get_bitvector80(u_int64_t key_MSB, u_int16_t key_LSB);
bitvector80_value* get_val80(bitvector80* vector);
void print_bitvector80(bitvector80* vector);

bitvector* get_bitvector(u_int64_t val, unsigned int max_bits);
u_int64_t get_val(bitvector* vector);
void print_bitvector(bitvector* vector);