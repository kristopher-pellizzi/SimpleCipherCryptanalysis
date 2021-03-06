#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "bitvector.h"

// Given a value between 0 and 15, return a bitvector structure representing its binary encoding
bitvector4* get_bitvector4(u_int8_t val){
    bitvector4* vector = (bitvector4*) malloc(sizeof(bitvector4));
    int pow = 1 << 4;

    if(val >= pow){
        char* error = "Argument must be lower than %d\n";

        fprintf(stderr, error, pow);
        return 0;
    }

    for(int i = 3; i >= 0 ; i--){
        pow = 1 << i;

        vector->repr[i] = val / pow;
        val = val % pow;
    }

    return vector;
}

// Given a bitvector with 4 bits as argument, return the value it is encoding
u_int8_t get_val4(bitvector4* vector){
    u_int8_t ret = 0;

    for(int i = 3; i >= 0; i--){
        ret += vector->repr[i] << i;
    }

    return ret;
}

void print_bitvector4(bitvector4* vector){
    for(int i = 3; i >= 0; i--){
        printf("%d", vector->repr[i]);
    }
    printf("\n");
}

// Given a value between 0 and 65535, return a bitvector structure representing its binary encoding
bitvector16* get_bitvector16(u_int16_t val){
    bitvector16* vector;

    vector = (bitvector16*)malloc(sizeof(bitvector16));

    for(int i = 15; i >= 0; i--){
        unsigned int pow = 1 << i;

        vector->repr[i] = val / pow;
        val = val % pow;
    }

    return vector;
}

// Given a bitvector with 16 bits as argument, return the value it is encoding
u_int16_t get_val16(bitvector16* vector){
    u_int16_t ret = 0;

    for(int i = 15; i >= 0; i--){
        ret += (vector->repr[i] << i);
    }
    return ret;
}

void print_bitvector16(bitvector16* vector){
    for(int i = 15; i >= 0; i--){
        printf("%d", vector->repr[i]);
    }
    printf("\n");
}

// Given a value 'key_MSB' between 0 and (2^64 - 1) and a value 'key_LSB' between 0 and 65535,
// return a bitvector with 80 bits, whose 64 Most Significant Bits encode 'key_MSB',
// while the last 16 Least Significant Bits encode 'key_LSB'
bitvector80* get_bitvector80(u_int64_t key_MSB, u_int16_t key_LSB){
    bitvector80* vector;

    vector = (bitvector80*)malloc(sizeof(bitvector80));

    int diff = 79 - 63;

    for(int i = 63; i >= 0; i--){
        u_int64_t pow = (u_int64_t) 1 << i;

        vector->repr[i + 16] = key_MSB / pow;
        key_MSB = key_MSB % pow;
    }

    for(int i = 15; i >= 0; i--){
        int pow = 1 << i;

        vector->repr[i] = key_LSB / pow;
        key_LSB = key_LSB % pow;
    }

    return vector;
}

// Given a bitvector with 80 bits, return a structure containing a value 'MSB' between 0 and (2^64 - 1) and a
// value 'LSB' between 0 and 65535. 'MSB' encoding is given by the 64 Most Significant Bits of the bitvector;
// 'LSB' encoding is given by the last 16 Least Significatn Bits of the bitvector.
bitvector80_value* get_val80(bitvector80* vector){
    bitvector80_value* ret = (bitvector80_value*)malloc(sizeof(bitvector80_value));

    ret->LSB = ret->MSB = 0;
    for(int i = 63; i >= 0; i--){
        ret->MSB += ((u_int64_t)vector->repr[i + 16] << i);
    }

    for(int i = 15; i >= 0; i--){
        ret->LSB += (vector->repr[i] << i);
    }

    return ret;
}

void print_bitvector80(bitvector80* vector){
    for(int i = 79; i >= 0; i--){
        printf("%d", vector->repr[i]);
        if(i % 4 == 0){
            printf("\t");
        }
    }
    printf("\n");
}

// Given a value between 0 and 2^64 and a maximum number of bits, return a generic bitvector with 'max_bits'
// bits encoding the value. If the value can't be represented with 'max_bits' bits, report error and abort execution
bitvector* get_bitvector(u_int64_t val, unsigned int max_bits){
    if(max_bits > 64){
        fprintf(stderr, "Generic bitvectors with more than 64 bits not implemented yet\n");
        abort();
    }

    if(max_bits < 64 && val >= (1 << max_bits)){
        fprintf(stderr, "The value to be converted can't be represented with %u bits\n", max_bits);
    }

    bitvector* vector = (bitvector*)malloc(sizeof(bitvector));
    vector->repr = (int*)malloc(sizeof(int) * max_bits);
    vector->size = max_bits;
    for(int i = max_bits - 1; i >=0; i--){
        uint64_t pow = (u_int64_t ) 1 << i;
        vector->repr[i] = val / pow;
        val = val % pow;
    }

    return vector;
}

// Given a generic bitvector (with at most 64 bits), return the value it is encoding
u_int64_t get_val(bitvector* vector){
    unsigned int size = vector->size;
    u_int64_t ret = 0;

    for(unsigned int i = 0; i < size; i++){
        ret += vector->repr[i] * ((u_int64_t) 1 << i);
    }

    return ret;
}

void print_bitvector(bitvector* vector){
    for(int i = vector->size - 1; i >= 0; i--){
        printf("%d", vector->repr[i]);
    }
    printf("\n");
}