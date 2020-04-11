//
// Created by Kristopher Pellizzi on 28/03/2020.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "list.h"
#include "linear_cryptanalysis.h"

char* TABLE_NAME = "bias_table";
u_int8_t IS_OUTPUT_SELECTED[4] = {0};
node* OUTPUT_LISTS[4];
node* PROBABLE_KEYS[4];

int create_bias_table(){
    FILE* table_file = fopen(TABLE_NAME, "w");

    for(int i = 0; i < 16; i++){
        for(int j = 0; j < 16; j++){
            BIAS_TABLE[i][j] = 0;

            for(int k = 0; k < 16; k++){
                BIAS_TABLE[i][j] += verify_linear_relation(i, j, k);
            }

            BIAS_TABLE[i][j] -= 8;
            fprintf(table_file, "%d\t", BIAS_TABLE[i][j]);
        }
        fprintf(table_file, "\n");
    }

    fclose(table_file);
    return 0;
}

u_int8_t read_bias_table(){
    FILE* table_file = fopen(TABLE_NAME, "r");

    if(!table_file){
        fprintf(stderr, "File %s does not exist. It will be created now...\n", TABLE_NAME);
        create_bias_table();
        usleep(1 << 20);
        print_bias_table();
        return 0;
    }

    for (int i = 0; i < 16; i++){
        for (int j = 0; j < 16; j++){
            fscanf(table_file, "%d", &BIAS_TABLE[i][j]);
            printf("%d\t", BIAS_TABLE[i][j]);
        }
        printf("\n");
    }

    fclose(table_file);
    return 0;
}

void print_bias_table(){
    for(int i = 0; i < 16; i++){
        for(int j = 0; j < 16; j++){
            printf("%d\t", BIAS_TABLE[i][j]);
        }
        printf("\n");
    }
}

u_int8_t verify_linear_relation(u_int8_t input_mask, u_int8_t output_mask, u_int8_t input){
    bitvector4* masked_input = get_bitvector4(input_mask & input);
    bitvector4* masked_output = get_bitvector4(output_mask & SBOX[input]);

    u_int8_t xored_input = 0, xored_output = 0;

    for(int i = 0; i < 4; i++){
        xored_input ^= masked_input->repr[i];
        xored_output ^= masked_output->repr[i];
    }

    free(masked_input);
    free(masked_output);
    masked_input = 0;
    masked_output = 0;

    return xored_input == xored_output ? 1 : 0;

}

u_int8_t count_outputs(u_int8_t output_index){
    bitvector4* output_bitvector = get_bitvector4(output_index);
    u_int8_t ret = 0;

    for(int i = 0; i < 4; i++){
        ret += output_bitvector->repr[i];
    }

    free(output_bitvector);
    return ret;
}

int output_already_selected(int output_index){
    bitvector4* output_vector = get_bitvector4(output_index);
    int to_be_discarded = 1;

    for(int i = 0; i < 4; i++){
        if(output_vector->repr[i] == 1) {
            to_be_discarded = to_be_discarded && (output_vector->repr[i] == IS_OUTPUT_SELECTED[i]);
        }
    }
    return to_be_discarded;
}

int all_output_selected(){
    for(int i = 0; i < 4; i++){
        if(!IS_OUTPUT_SELECTED[i])
            return 0;
    }
    return 1;
}

couple* select_relation(){
    couple* ret = (couple*)malloc(sizeof(couple));

    if(all_output_selected()){
        return NULL;
    }

    if(OUTPUT_LISTS[0] == NULL) {
        for (int i = 1; i < 16; i++) {
            int *to_add = (int *) malloc(sizeof(int));
            *to_add = i;
            add(&OUTPUT_LISTS[count_outputs(i) - 1], to_add, sizeof(int));
        }
    }

    int8_t current_bias = -1;
    node* current_node = OUTPUT_LISTS[0];
    int8_t max_bias = BIAS_TABLE[0][0];
    int8_t number_of_outputs = 1;

    while(current_node != NULL) {
        int output_index = *((int *) current_node->elem);
        for (int i = 1; i < 16; i++) {
            int8_t bias_value = abs(BIAS_TABLE[i][output_index]);
            if(bias_value >= max_bias / 2) {
                if (!output_already_selected(output_index) && bias_value > current_bias) {
                    current_bias = bias_value;
                    ret->x = i;
                    ret->y = output_index;
                }
            }
        }

        current_node = current_node->next;

        if (current_node == NULL && current_bias == -1 && number_of_outputs == 1) {
            current_node = OUTPUT_LISTS[1];
            number_of_outputs++;
        }
    }

    if(current_bias == -1){
        return NULL;
    }

    bitvector4* output_vector = get_bitvector4(ret->y);
    for(int i = 0; i < 4; i++){
        IS_OUTPUT_SELECTED[i] = output_vector->repr[i] ? 1 : IS_OUTPUT_SELECTED[i];
    }
    return ret;
}

void print_state(uint8_t* arr){
    for(int i = 0; i < 16; i++){
        printf("%d", arr[i]);
    }
}

couple* seek_best_relation_by_input(uint8_t input_index){
    couple* ret = (couple*)malloc(sizeof(couple));
    node* current_node = OUTPUT_LISTS[0];
    int current_bias = -1;
    uint8_t number_of_outputs = 1;

    while(current_node != NULL){
        int output_index = *((int*)current_node->elem);
        int bias_val = abs(BIAS_TABLE[input_index][output_index]);

        if(bias_val > current_bias){
            current_bias = bias_val;
            ret->y = output_index;
        }

        current_node = current_node->next;

        if(current_node == NULL && number_of_outputs <2){
            current_node = OUTPUT_LISTS[1];
            number_of_outputs++;
        }

    }

    ret->x = input_index;
    return ret;
}

int add_to_list(uint8_t* input_bits, node** head){
    int ret = 0;
    bitvector4* sbox_input = (bitvector4*)malloc(sizeof(bitvector4));

    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            sbox_input->repr[3 - j] = input_bits[j + i * 4];
        }
        uint8_t input_val = get_val4(sbox_input);
        if(input_val > 0) {
            couple* to_add = (couple*)malloc(sizeof(couple));

            to_add->x = i;
            to_add->y = input_val;
            add(head, to_add, sizeof(couple));
            ret ++;
        }
    }
    free(sbox_input);

    return ret;
}

int test_round_key(u_int16_t ptx, u_int16_t ctx, u_int16_t round_key, u_int64_t key_MSB){
    // NOTE: we assume to know the first 64 bits of the key. In reality, we should brute force them or continue with the linear cryptanalysis
    // to further reduce the brute force space of the previous round keys. This is only a small demo.

    u_int16_t state = ptx;
    u_int16_t inverse_state = ctx;

    // Apply 3 encryption rounds + 1 round key addition using the first 64 bits of the key (assumed to be known)
    int mod_mask = 1 << 16;
    for(int i = 0; i < 3; i++){
        state = encryption_round(state, (key_MSB >> (64 - 16 * (i + 1))) % mod_mask);
    }
    state = add_round_key(state, key_MSB % mod_mask);

    // Starting from the ctx, apply the last round key addition and invert the sbox
    inverse_state = reverse_sbox(add_round_key(inverse_state, round_key));

    // Compare the 2 computed states. If they are equal, the key round is the correct one
    return state == inverse_state ? 1 : 0;

}

int next_combination(node** sbox_list){
    int change = 1;

    for(int i = 3; i >= 0; i--){
        if(change) {
            sbox_list[i] = sbox_list[i]->next;
            change = 0;

            if (sbox_list[i] == NULL) {
                if (i == 0) {
                    return 0;
                }

                sbox_list[i] = PROBABLE_KEYS[i];
                change = 1;
            }
        }
    }
    return 1;
}

int perform_linear_cryptanalysis(u_int64_t number_of_couples, couple** pairs, u_int64_t key_MSB){
    node* local_probable_keys[4];

    for(int i = 0; i < 4; i++){
        OUTPUT_LISTS[i] = NULL;
        PROBABLE_KEYS[i] = NULL;
        IS_OUTPUT_SELECTED[i] = 0;
    }

    // U[i][j] contains the j-th bit in input to sboxes of the i-th round
    // V[i][j] contains the j-th bit at output of sboxes of the i-th round

    // Analyze i relations
    for(int i = 0; i < 20; i++) {
        u_int8_t U[4][16] = {{0}};
        u_int8_t V[4][16] = {{0}};
        couple *selected_relation = select_relation();

        if(selected_relation == NULL){
            fprintf(stderr, "No more relations needed\n");
            break;
        }

        printf("The selected relation is %d => %d, which has a bias of %d\n", selected_relation->x,
               selected_relation->y, BIAS_TABLE[selected_relation->x][selected_relation->y]);


        double total_bias = 0.5;
        node* sbox_list = NULL;

        //int8_t considered_round = 0;
        int8_t considered_sbox = 0;

        int considered_round = 0;
        int round_boxes = 1;
        int items_added = 0;
        while(considered_round < 3) {
            bitvector4 *input_bits = get_bitvector4(selected_relation->x);
            bitvector4 *output_bits = get_bitvector4(selected_relation->y);

            total_bias *= 2 * BIAS_TABLE[selected_relation->x][selected_relation->y] / (float) 16;
            for (int j = 0; j < 4; j++) {
                u_int8_t round_bit = j + (4 * considered_sbox);

                if (input_bits->repr[3 - j] == 1) {
                    U[considered_round][round_bit] = 1;
                }

                int permuted_bit_pos = considered_sbox;
                int next_sbox = j;
                if (output_bits->repr[3 - j] == 1) {
                    V[considered_round][round_bit] = 1;
                    U[considered_round + 1][permuted_bit_pos + 4 * next_sbox] = 1;
                }

            }

            items_added += add_to_list(U[considered_round + 1], &sbox_list);
            round_boxes--;

            if(!round_boxes){
                round_boxes = items_added;
                considered_round++;
                items_added = 0;
            }

            couple* next_elem = (couple*) dequeue(&sbox_list)->elem;
            considered_sbox = next_elem->x;
            int input_index = next_elem->y;
            selected_relation = seek_best_relation_by_input(input_index);
            // printf("For now total bias is %.4f \n", total_bias);
        }

        printf("Path: \n");
        print_state(U[0]);
        printf(" -> ");
        print_state(V[0]);
        printf("\n");
        print_state(U[1]);
        printf(" -> ");
        print_state(V[1]);
        printf("\n");
        print_state(U[2]);
        printf(" -> ");
        print_state(V[2]);
        printf("\n");
        print_state(U[3]);
        printf("\n\n");

        printf("Relation ");
        print_state(U[0]);
        printf(" -> ");
        print_state(U[3]);
        printf(" holds with a theoretical bias of %.4f\n", total_bias);



        int8_t key_bits_number = 0;
        for(int j = 0; j < 16; j++){
            key_bits_number += U[3][j];
        }

        key_bits_number *= 4;

        bitvector16* key_vector;

        double previous_diff = -1;
        int most_probable_subkey = -1;
        u_int8_t output_sboxes_touched[4] = {0};

        for(int k = 0; k < 4; k++){
            local_probable_keys[k] = NULL;
        }

        for(int j = 0; j < (1 << key_bits_number); j++){
            u_int64_t counter = 0;
            key_vector = get_bitvector16(0);
            int considered_bit = 0;
            bitvector* subkey_bitvector = get_bitvector(j, key_bits_number);

            for(int k = 0; k < 4; k++){
                output_sboxes_touched[k] = 0;
            }

            // Check which sboxes of the last round are touched by the selected relation
            for(int k = 0; k < 16; k++){
                if(U[3][k]){
                    u_int8_t sbox_index = k / 4;
                    output_sboxes_touched[sbox_index] = 1;
                }

            }

            // Distribute subkey bits in the correct position in the whole key bitvector representation
            for(int k = 0; k < 4; k++){
                if(output_sboxes_touched[k]){
                    for(int q = 3; q >= 0; q--) {
                        key_vector->repr[4 * (3 - k) + q] = subkey_bitvector->repr[key_bits_number - 1 - considered_bit];
                        considered_bit++;
                    }
                }
            }

            printf("Key vector: ");
            print_bitvector16(key_vector);

            u_int16_t key_val = get_val16(key_vector);

            for(int q = 0; q < number_of_couples; q++){
                u_int16_t V1 = key_val ^ pairs[q]->y;
                u_int16_t sbox_input = reverse_sbox(V1);
                bitvector16* ptx = get_bitvector16(pairs[q]->x);
                bitvector16* ctx = get_bitvector16(sbox_input);

                int xor_res = 0;
                for(int k = 0; k < 16; k++){
                    xor_res ^= U[0][k] * ptx->repr[15 - k];
                    xor_res ^= U[3][k] * ctx->repr[15 - k];
                }
                free(ptx);
                free(ctx);
                if(xor_res == 0){
                    counter++;
                }
            }

            double bias = ((float) counter / number_of_couples) - 0.5;
            printf("SUBKEY: %d\tBIAS: %.30f\n", j, bias);
            double delta = fabs(1.0 / number_of_couples) ;
            int add_to_lists = 0;

            if(fabs(bias) > previous_diff){
                for(int k = 0; k < 4; k++){
                    if(output_sboxes_touched[k])
                        clear(&local_probable_keys[k]);
                }
                previous_diff = fabs(bias);
                most_probable_subkey = j;
                add_to_lists = 1;
            }
            else if (fabs(bias) >= previous_diff - delta && fabs(bias) <= previous_diff + delta){
                add_to_lists = 1;
            }

            if(add_to_lists){
                for(int k = 0; k < 4; k++){
                    if(output_sboxes_touched[k]){
                        bitvector4* sbox_subkey = get_bitvector4(0);

                        for(int q = 0; q < 4; q++){
                            sbox_subkey->repr[q] = key_vector->repr[(3 - k) * 4 + q];
                        }

                        add_if_not_present(&local_probable_keys[k], sbox_subkey, sizeof(bitvector4));
                    }
                }
            }
        }

        for(int k = 0; k < 4; k++) {
            node* tmp = local_probable_keys[k];
            while(tmp != NULL){
                add_if_not_present(&PROBABLE_KEYS[k], tmp->elem, tmp->data_size);
                tmp = tmp -> next;
            }
        }

        int considered_bit = 0;
        bitvector* key_bitvector = get_bitvector(most_probable_subkey, key_bits_number);
        print_bitvector(key_bitvector);
        key_vector = get_bitvector16(0);

        for(int k = 0; k < 4; k++){
            if(output_sboxes_touched[k]){
                for(int q = 0; q < 4; q++) {
                    // printf("k = %d\n", k);
                    key_vector->repr[4 * (3 - k) + q] = key_bitvector->repr[considered_bit];
                    considered_bit++;
                }

            }
        }

        /*printf("sbox_input: ");
        print_state(U[3]);
        printf("\tkey: ");
        print_bitvector16(key_vector);
        printf("Key bits: %d\n", key_bits_number);*/

        /*print_state(U[0]);
        printf(" --> ");
        print_state(V[0]);
        printf("\n");
        print_state(U[1]);
        printf(" --> ");
        print_state(V[1]);
        printf("\n");
        print_state(U[2]);
        printf(" --> ");
        print_state(V[2]);
        printf("\n");
        print_state(U[3]);
        printf(" --> ");
        print_state(V[3]);
        printf("\n");*/



    }

    /*printf("LISTS: \n");
    for(int k = 0; k < 4; k++){
        printf("LIST %d\n", k);
        node* current = PROBABLE_KEYS[k];

        while(current != NULL){
            bitvector4* vector = (bitvector4*) current->elem;
            print_bitvector4(vector);
            current = current -> next;
        }

    }*/

    // Test all combinations of found subkeys
    bitvector16* key_vector = get_bitvector16(0);
    node* subkeys[4];

    for(int k = 0; k < 4; k++) {
        subkeys[k] = PROBABLE_KEYS[k];
    }

    int cont = 1;

    while(cont) {
        for (int k = 0; k < 4; k++) {
            for (int q = 0; q < 4; q++) {
                bitvector4 *current_subkey = (bitvector4 *) subkeys[k]->elem;
                key_vector->repr[(3 - k) * 4 + q] = current_subkey->repr[q];
            }
        }

        u_int16_t key_val = get_val16(key_vector);
        if (test_round_key(pairs[0]->x, pairs[0]->y, key_val, key_MSB)) {
            printf("THE LAST ROUND KEY IS %u -> ", key_val);
            print_bitvector16(key_vector);
            printf("\n");
            return 1;
        };
        cont = next_combination(subkeys);
    }
    printf("SUBKEY NOT FOUND\n");
    return 0;
}