#include <stdlib.h>
#include <stdbool.h>
#include "token.h"

#define STACK_ALLOC_BLOCK 8

typedef struct TokenStack {
    int data_cap;
    int data_pos;
    TokenPtr top;
    bool empty;
    TokenPtr *data;
} *TokenStackPtr;

/** Initialize token
    @returns TokenStackPtr if succeded NULL otherwise
*/
TokenStackPtr token_stack_init();

/** Pop last item from stack
    @param stack TokenStackPtr from which to pop
    @returns true if succeded, false otherwise
*/
bool token_stack_pop(TokenStackPtr stack);

/** Push new Token into Token Stack
 *  @param stack TokenStackPtr to add Token to
 *  @param token TokenPtr that should be added to stack
 *  @returns true if succeded, false otherwise
*/
bool token_stack_push(TokenStackPtr stack, TokenPtr token);

/** Dispose and free Token Stack
*/
void token_stack_dispose(TokenStackPtr stack);