#include <stdlib.h>
#include <stdbool.h>
#include "token.h"

#define STACK_ALLOC_BLOCK 8

typedef struct TokenStackItem {
    bool rule;
    TokenPtr token;
} *TokenStackItemPtr;

typedef struct TokenStack {
    int data_cap;
    int data_pos;
    TokenStackItemPtr top;
    bool empty;
    TokenStackItemPtr *data;
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
bool token_stack_push(TokenStackPtr stack, TokenPtr token, bool rule);

/** Creates new token and pushes it into Token Stack
 *  @param stack TokenStackPtr to add Token to
 *  @param type int type of the new token
 *  @param rule bool if that stack item is rule or not
 *  @returns true if succeded, false otherwise
*/
bool token_stack_push_new(TokenStackPtr stack, int type, bool rule);

/** Dispose and free Token Stack
*/
void token_stack_dispose(TokenStackPtr stack);