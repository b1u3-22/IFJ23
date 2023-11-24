/**
 *  Project:    Implementace překladače imperativního jazyka IFJ23.
 *  File:       @brief Hlavičkový soubor pro token stack
 *  Authors:    @author Jiří Sedlák xsedla2e
*/

#include <stdlib.h>
#include <stdbool.h>

#ifndef _TOKEN
#define _TOKEN
#include "token.h"
#endif

#ifndef _TOKEN
#define _TOKEN
#include "scanner.h"
#endif

#define TOKEN_STACK_ALLOC_BLOCK 50

typedef struct TokenStack {
    TokenPtr *tokens;
    int tokens_pos;
    int tokens_cap;
    TokenPtr top;
    bool empty;
} *TokenStackPtr;

TokenStackPtr token_stack_init();
bool token_stack_push(TokenStackPtr stack, TokenPtr token);
TokenPtr token_stack_create_token(TokenStackPtr stack);
bool token_stack_pop(TokenStackPtr stack);
void token_stack_dispose(TokenStackPtr stack);
bool token_stack_unget(TokenStackPtr stack);
TokenPtr token_stack_get(TokenStackPtr stack);