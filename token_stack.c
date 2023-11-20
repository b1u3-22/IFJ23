#include "token_stack.h"


TokenStackPtr token_stack_init() {
    TokenStackPtr stack = calloc(1, sizeof(struct TokenStack)); 
    if (!stack) return NULL; // Allocate memory for stack and check it

    stack->data = calloc(TOKEN_STACK_ALLOC_BLOCK, sizeof(TokenStackItemPtr)); // Allocate memory for stack data
    if (!stack->data) {
        free(stack); // fail stack if allocation fails
        return NULL;
    }

    // initialize values
    stack->data_cap = TOKEN_STACK_ALLOC_BLOCK;
    stack->data_pos = -1;
    stack->top = NULL;
    stack->empty = true;

    return stack;
}

bool token_stack_pop(TokenStackPtr stack) {
    if (stack->empty) return false;

    free(stack->data[stack->data_pos]->token);
    free(stack->data[stack->data_pos--]);

    if (stack->data_pos > -1) stack->top = stack->data[stack->data_pos];
    else {
        stack->top = NULL;
        stack->empty = true;
    }
    return true;
}

bool token_stack_push(TokenStackPtr stack, TokenPtr token, bool rule) {
    // if we hit the allocated capacity for stack data, increase memory by reallocation
    if (++(stack->data_pos) >= stack->data_cap){
        TokenStackItemPtr *new_data = realloc(stack->data, stack->data_cap + TOKEN_STACK_ALLOC_BLOCK);
        if (!new_data) {
            stack->data_pos--; // if reallocation fails, decrease data_pos to previous value
            return false;
        }

        // else set old data to new ones and increase data_cap to reflect the change
        stack->data = new_data;
        stack->data_cap += TOKEN_ALLOC_BLOCK;
    }

    // add new token to data and correct top pointer
    TokenStackItemPtr new_item = malloc(sizeof(struct TokenStackItem));
    if (!new_item) return false;

    new_item->token = token;
    new_item->rule = rule;

    stack->data[stack->data_pos] = new_item;
    stack->top = stack->data[stack->data_pos];
    stack->empty = false;
    return true;
}

bool token_stack_push_new(TokenStackPtr stack, int type, bool rule) {
    TokenPtr new_token = token_init();
    if (!new_token) return false;
    new_token->type = type;
    token_stack_push(stack, new_token, rule);
    return true;
}

void token_stack_dispose(TokenStackPtr stack) {
    while (!(stack->empty)) token_stack_pop(stack);
    free(stack->data);
    free(stack);
}