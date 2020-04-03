//
// Created by Kristopher Pellizzi on 28/03/2020.
//
#include <stdio.h>
#include <stdlib.h>

#ifndef SIMPLECIPHER_SBOX_H
#define SIMPLECIPHER_SBOX_H

#endif //SIMPLECIPHER_SBOX_H

extern u_int8_t SBOX[16];
u_int8_t REVERSE_SBOX[16];

void revert_sbox();
void print_sbox(u_int8_t * table);
u_int16_t sbox(u_int16_t state);
u_int16_t reverse_sbox(u_int16_t state);