//
// Created by Kristopher Pellizzi on 28/03/2020.
//

#include <zconf.h>
#include "bitvector.h"
#include "couple.h"
#include "sbox.h"

#ifndef SIMPLECIPHER_LINEAR_CRYPTANALYSIS_H
#define SIMPLECIPHER_LINEAR_CRYPTANALYSIS_H

#endif //SIMPLECIPHER_LINEAR_CRYPTANALYSIS_H

int8_t BIAS_TABLE[16][16];

int create_bias_table();
u_int8_t read_bias_table();
void print_bias_table();

// Function used to verify if relation represented by "input_mask" => "output_mask" in the bias table holds with input "input"
u_int8_t verify_linear_relation(u_int8_t input_mask, u_int8_t output_mask, u_int8_t input);

void perform_linear_cryptanalysis(u_int64_t number_of_couples, couple** pairs);