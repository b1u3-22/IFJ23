#include "token_stack.h"

TokenStackPtr token_stack_init() {
    TokenStackPtr new_stack = (TokenStackPtr) malloc(sizeof(struct TokenStack));
    if (!new_stack) return NULL;

    new_stack->tokens_cap = 0;
    new_stack->tokens_pos = -1;
    new_stack->tokens = NULL;
}

TokenPtr token_stack_create_token(TokenStackPtr stack) {
    TokenPtr new_token = token_init();
    if (!new_token) return NULL;
    token_stack_push(stack, new_token);
    return new_token;
}

bool token_stack_push(TokenStackPtr stack, TokenPtr token) {
    // if we hit the allocated capacity for stack data, increase memory by reallocation
    if ((stack->tokens_pos + 1) >= stack->tokens_cap){
        TokenPtr *new_tokens = realloc(stack->tokens, (stack->tokens_cap + TOKEN_STACK_ALLOC_BLOCK) * sizeof(TokenPtr));
        if (!new_tokens) return false;

        // else set old data to new ones and increase data_cap to reflect the change
        stack->tokens = new_tokens;
        stack->tokens_cap += TOKEN_STACK_ALLOC_BLOCK;
    }

    stack->tokens[++(stack->tokens_pos)] = token;
    stack->top = token;

    stack->empty = false;
    return true;
}

bool token_stack_pop(TokenStackPtr stack) {
    if (stack->empty) return false;

    token_dispose(stack->tokens[stack->tokens_pos--]);
    if (stack->tokens_pos == -1) {
        stack->empty = true;
        stack->top = NULL;
    }
    else {
        stack->top = stack->tokens[stack->tokens_pos];
    }
    return true;
}

void token_stack_dispose(TokenStackPtr stack) {
    while (!(stack->empty)) {
        token_stack_pop(stack);
    }
    free(stack);
}

bool token_stack_unget(TokenStackPtr stack) {
    if (stack->empty) return false;
    unget_token(stack->top);
    return token_stack_pop(stack);
}

TokenPtr token_stack_get(TokenStackPtr stack) {
    TokenPtr token = token_stack_create_token(stack);
    get_next_token(token);
    return token;
}