//
// Created by Kristopher Pellizzi on 29/03/2020.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zconf.h>
#include "list.h"
#include "bitvector.h"

int delete(node** head, void* to_remove){
    if(*head == NULL){
        fprintf(stderr, "The list is empty\n");
        return -1;
    }

    node* current = *head;
    while(current != NULL && memcmp(current->elem, to_remove, current->data_size)){
        current = current->next;
    }

    if(current == NULL){
        fprintf(stderr, "Item not found\n");
        usleep(1 << 15);
        return -1;
    }

    node* previous = current->prev;
    node* following = current->next;

    if(previous != NULL){
        previous->next = current->next;
    }
    else{
        *head = current->next;
    }

    if(following != NULL){
        following->prev = current->prev;
    }

    free(current->elem);
    free(current);

    return 0;
}

int add(node** head, void* to_add, size_t data_size){
    node* new_node = (node*)malloc(sizeof(node));

    new_node->data_size = data_size;
    new_node->elem = to_add;
    new_node->next = NULL;

    if(*head == NULL){
        new_node->prev = NULL;
        *head = new_node;
        return 0;
    }

    node* current = *head;
    while(current->next != NULL){
        current = current->next;
    }
    current->next = new_node;
    new_node->prev = current;
    return 0;
}

void clear(node** head){
    node* current = *head;

    while(current != NULL){
        //free(current->elem);
        node* tmp = current->next;
        free(current);
        current = tmp;
    }

    *head = NULL;
}

// Remove first element in the list and remove it
node* dequeue(node** head){
    node* ret = *head;
    *head = ret->next;
    return ret;
}

int add_if_not_present(node** head, void* to_add, size_t data_size){
    if(*head == NULL){
        return add(head, to_add, data_size);
    }

    node* new_node = (node*) malloc(sizeof(node));
    new_node->elem = to_add;
    new_node -> next = NULL;
    new_node ->data_size = data_size;

    node* current = *head;

    while(current->next != NULL){
        bitvector4* test = (bitvector4*) current->elem;
        bitvector4* test2 = (bitvector4*) to_add;
        if(!memcmp(current->elem, to_add, data_size)){
            return 0;
        }
        current = current->next;
    }

    // Check if last element is equal to the one to be added. If it is, return 0;
    if(!memcmp(current->elem, to_add, data_size)){
        return 0;
    }

    current->next = new_node;
    new_node->prev = current;
    return 1;
}

unsigned long length(node* head){
    unsigned long cont = 0;

    while(head != NULL){
        cont++;
        head = head->next;
    }
    return cont;
}