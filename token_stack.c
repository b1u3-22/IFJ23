#include "token_stack.h"


TokenStackPtr token_stack_init() {
    TokenStackPtr stack = calloc(1, sizeof(struct TokenStack)); 
    if (!stack) return NULL; // Allocate memory for stack and check it

    stack->data = calloc(STACK_ALLOC_BLOCK, sizeof(TokenPtr)); // Allocate memory for stack data
    if (!stack->data) {
        free(stack); // fail stack if allocation fails
        return NULL;
    }

    // initialize values
    stack->data_cap = STACK_ALLOC_BLOCK;
    stack->data_pos = -1;
    stack->top = NULL;
    stack->empty = true;

    return stack;
}

bool token_stack_pop(TokenStackPtr stack) {
    if (stack->empty) return false;

    // if stack isn't empty, set top pointer to last token 
    if (--(stack->data_pos) >= 0) {
        stack->top = stack->data[stack->data_pos];
    }

    // if we poped the last item, remove top pointer and correct empty bool
    else {
        stack->top = NULL;
        stack->empty = true;
    }

    return true;
}

bool token_stack_push(TokenStackPtr stack, TokenPtr token) {
    // if we hit the allocated capacity for stack data, increase memory by reallocation
    if (++(stack->data_pos) >= stack->data_cap){
        TokenPtr *new_data = realloc(stack->data, stack->data_cap + STACK_ALLOC_BLOCK);
        if (!new_data) {
            stack->data_pos--; // if reallocation fails, decrease data_pos to previous value
            return false;
        }

        // else set old data to new ones and increase data_cap to reflect the change
        stack->data = new_data;
        stack->data_cap += STACK_ALLOC_BLOCK;
    }

    // add new token to data and correct top pointer
    stack->data[stack->data_pos] = token;
    stack->top = stack->data[stack->data_pos];
    stack->empty = false;
    return true;
}

void token_stack_dispose(TokenStackPtr stack) {
    free(stack->data);
    free(stack);
}