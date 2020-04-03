//
// Created by Kristopher Pellizzi on 29/03/2020.
//

#ifndef SIMPLECIPHER_LIST_H
#define SIMPLECIPHER_LIST_H

#endif //SIMPLECIPHER_LIST_H


struct node{
    size_t data_size;
    void* elem;
    struct node* next;
    struct node* prev;
} typedef node;

int delete(node** head, void* to_remove);
node* dequeue(node** head);
int add(node** head, void* to_add, size_t data_size);
void clear(node** head);