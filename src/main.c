#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "list.h"
#include "linear_cryptanalysis.h"

u_int8_t SBOX[16] = {14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7};

void revert_sbox(){
    for(int i = 0; i < 16; i++){
        REVERSE_SBOX[SBOX[i]] = i;
    }
}

void print_sbox(u_int8_t * table){
    for(int i = 0; i < 16; i++){
        printf("%d\t-->\t%d\n", i, table[i]);
    }
}

u_int16_t add_round_key(u_int16_t state, u_int16_t key){
    return key ^ state;
}

u_int16_t sbox(u_int16_t state){
    u_int16_t ret = 0;
    u_int8_t s[4];

    for(int i = 0; i < 4; i++){
        int shift_val = 16 - (i + 1) * 4;
        s[i] = (state >> shift_val) % 16;
        s[i] = SBOX[s[i]];
        ret += s[i] << shift_val;
    }

    return ret;
}

u_int16_t reverse_sbox(u_int16_t state){
    u_int16_t ret = 0;
    u_int8_t s[4];

    for(int i = 0; i < 4; i++){
        int shift_val = 16 - (i + 1) * 4;
        s[i] = (state >> shift_val) % 16;
        s[i] = REVERSE_SBOX[s[i]];
        ret += s[i] << shift_val;
    }

    return ret;
}

u_int16_t build_state(bitvector16* vector){
    return get_val16(vector);
}

u_int16_t permutation(u_int16_t state){
    bitvector16* bitvector_state = get_bitvector16(state);

    int permuted[4][4];

    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            permuted[i][j] = 0;
        }
    }

    for(int j = 0; j < 4; j++){
        for(int i = 0; i < 4; i++){
            if(permuted[i][j]){
                continue;
            }
            permuted[i][j] = permuted [j][i] = 1;
            int tmp = bitvector_state->repr[(j * 4) + i];
            bitvector_state->repr[(j * 4) + i] = bitvector_state->repr[(i * 4) + j];
            bitvector_state->repr[(i * 4) + j] = tmp;
        }
    }

    u_int16_t ret = build_state(bitvector_state);
    free(bitvector_state);
    bitvector_state = 0;
    return ret;
}

u_int16_t encryption_round(u_int16_t state, u_int16_t key){
    return permutation(sbox(add_round_key(key, state)));
}

u_int16_t decryption_round(u_int16_t state, u_int16_t key){
    return add_round_key(reverse_sbox(permutation(state)), key);
}

u_int16_t encrypt_block(u_int16_t ptx_block, bitvector80* key_bitvector){
    u_int16_t state = ptx_block;
    bitvector80_value* key_value = get_val80(key_bitvector);

    int mod_mask = 1 << 16;
    for(int i = 0; i < 3; i++){
        state = encryption_round(state, (key_value->MSB >> (64 - 16 * (i + 1))) % mod_mask);
    }

    state = add_round_key(sbox(add_round_key(state, key_value->MSB % mod_mask)), key_value->LSB);
    free(key_value);
    key_value = 0;
    return state;
}

u_int16_t decrypt_block(u_int16_t ctx_block, bitvector80* key_bitvector){
    u_int16_t state = ctx_block;
    bitvector80_value* key_value = get_val80(key_bitvector);
    int mod_mask = 1 << 16;

    state = add_round_key(reverse_sbox(add_round_key(state, key_value->LSB)), key_value->MSB % mod_mask);

    for(int i = 0; i < 3; i++){
        state = decryption_round(state, (key_value->MSB >> (16 * (i + 1))) % mod_mask);
    }

    return state;
}

int tests() {
    // DEBUG SECTION

    printf("SBOX:\n");
    print_sbox(SBOX);
    printf("REVERSE_SBOX:\n");
    print_sbox(REVERSE_SBOX);

    printf("Test encryption primitive...\n");
    int ptx = 54;
    for(int i = 0; i < 10; i++) {
        printf("Plaintext: %d\tCiphertext: %d\n", ptx, encrypt_block(ptx, get_bitvector80(0, 3)));
    }

    printf("Test bitvector80_val...\n");
    bitvector80* vector = get_bitvector80(0, 14);
    bitvector80_value* val = get_val80(vector);
    printf("MSB: %llu\tLSB:%d\n", val->MSB, val->LSB);

    printf("Test permutation reverse...\n");
    u_int16_t test = 1235;
    printf("0: %d\t1: %d\t2: %d\n", test, permutation(test), permutation(permutation(test)));

    printf("Test shifts...\n");
    test = 14 << 12;
    print_bitvector16(get_bitvector16(test));
    test += 14 << 8;
    print_bitvector16(get_bitvector16(test));
    test += 14 << 4;
    test += 4 << 0;
    print_bitvector16(get_bitvector16(test));

    printf("Test sbox reverse...\n");
    test = 1;
    printf("0: %d\t1: %d\t2: %d\n", test, sbox(test), reverse_sbox(sbox(test)));
    print_bitvector16(get_bitvector16(1));
    print_bitvector16(get_bitvector16(sbox(1)));
    print_bitvector16(get_bitvector16(10495));

    printf("Test encryption and decryption...\n");
    u_int16_t enc_round = encryption_round(ptx, 14);
    printf("encryption round: %d\tdecryption round: %d\n", enc_round, decryption_round(enc_round, 14));
    u_int16_t ctx = encrypt_block(ptx, get_bitvector80(0, 3));
    printf("Plaintext: %d\tComputed ciphertext: %d\n", ptx, ctx);
    printf("Ciphertext: %d\tComputed plaintext: %d\n", ctx, decrypt_block(ctx, get_bitvector80(0, 3)));

    // END OF DEBUG SECTION


    return 0;
}

void print_int_list(node* current){
    if(current == NULL){
        printf("Empty list...\n");
        return;
    }
    printf("%d",  *((int*) current->elem));
    current = current->next;

    while(current != NULL){
        printf(" -> %d", *((int*) current->elem));
        current = current->next;
    }

    printf("\n");
}

int test_list(){
    node* int_list = NULL;

    for(int i = 0; i < 100; i++){
        int* to_add = (int*)malloc(sizeof(int));
        *to_add = i;
        add(&int_list, to_add, sizeof(int));
    }

    print_int_list(int_list);

    int* to_remove = (int*)malloc(sizeof(int));
    *to_remove = 3;
    delete(&int_list, to_remove);
    delete(&int_list, to_remove);
    *to_remove = 65;
    delete(&int_list, to_remove);
    print_int_list(int_list);
    *to_remove = 0;
    delete(&int_list, to_remove);
    print_int_list(int_list);
    clear(&int_list);
    print_int_list(int_list);

    for(int i = 10; i >= 0; i--){
        int* to_add = (int*)malloc(sizeof(int));
        *to_add = i;
        add(&int_list,to_add, sizeof(int));
    }
    print_int_list(int_list);
    free(to_remove);
    clear(&int_list);
    return 0;
}

int test_linear_cryptanalysis(){
    int num =  20000;
    couple** pairs = (couple**)malloc(sizeof(couple*) * num);

    srand(time(0));
    u_int64_t pow = 1 << 16;
    u_int64_t MSB = 0;
    for(int i = 0; i < 4; i++){
        MSB += (u_int64_t )(rand() % pow) << (16 * i);
    }
    u_int16_t LSB = rand() % pow;
    printf("MSB: %lu\tLSB: %d\n", MSB, LSB);
    bitvector80* key_bitvector = get_bitvector80(MSB, LSB);
    printf("First: ");
    bitvector80_value* key = get_val80(key_bitvector);
    printf("MSB: %lu\tLSB: %d\n", key->MSB, key->LSB);
    printf("Encrypting with key: ");
    print_bitvector80(key_bitvector);

    for(int i = 0; i < num; i++){
        u_int16_t ptx = rand() % pow;
        u_int16_t ctx = encrypt_block(ptx, key_bitvector);
        couple* pair = (couple*)malloc(sizeof(couple));
        pair->x = ptx;
        pair->y = ctx;
        pairs[i] = pair;
    }

    printf("Reading bias table...\n");
    if(read_bias_table()){
        fprintf(stderr, "Error while reading the table...\n");
        abort();
    }

    perform_linear_cryptanalysis(num, pairs);
    return 0;
}


int main(){
    revert_sbox();

    //tests();
    //test_list();
    test_linear_cryptanalysis();

    return 0;
}

