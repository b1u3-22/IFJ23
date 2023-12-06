#include "gen_stack.h"
#include <stdlib.h>

GenStackPtr gen_stack_init() {
    GenStackPtr stack = (GenStackPtr) malloc(sizeof(struct GenStack)); 
    if (!stack) return NULL; // Allocate memory for stack and check it

    // initialize values
    stack->data = NULL;
    stack->data_cap = 0;
    stack->data_pos = -1;
    stack->top = NULL;
    stack->empty = true;

    return stack;
}

bool gen_stack_pop(GenStackPtr stack) {
    if (stack->empty) return false;

    free(stack->data[stack->data_pos--]);
    
    if (stack->data_pos > -1) stack->top = stack->data[stack->data_pos];
    else {
        stack->top = NULL;
        stack->empty = true;
    }
    return true;
}

bool gen_stack_push(GenStackPtr stack, TokenPtr token, bool op) {
    // if we hit the allocated capacity for stack data, increase memory by reallocation
    if ((stack->data_pos + 1) >= stack->data_cap){
        GenStackItemPtr *new_data = realloc(stack->data, (stack->data_cap + GEN_STACK_ALLOC_BLOCK) * sizeof(GenStackItemPtr));
        if (!new_data) return true;

        // else set old data to new ones and increase data_cap to reflect the change
        stack->data = new_data;
        stack->data_cap += GEN_STACK_ALLOC_BLOCK;
    }

    GenStackItemPtr new_item = malloc(sizeof(struct GenStackItem));
    if (!new_item) return true;

    new_item->token = token;
    new_item->op = op;

    // add new token to data and correct top pointer
    stack->data[++(stack->data_pos)] = new_item;
    stack->top = new_item;

    stack->empty = false;
    return false;
}

void gen_stack_dispose(GenStackPtr stack) {
    while (!(stack->empty)) gen_stack_pop(stack);
    free(stack->data);
    free(stack);
}