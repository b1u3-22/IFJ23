#include <stdbool.h>

#ifndef _TOKEN
#define _TOKEN
#include "token.h"
#endif

#define GEN_STACK_ALLOC_BLOCK 16

typedef struct GenStackItem {
    TokenPtr token;
    bool op;
} *GenStackItemPtr;

typedef struct GenStack {
    GenStackItemPtr *data;
    int data_pos;
    int data_cap;
    GenStackItemPtr top;
    bool empty;
} *GenStackPtr;


GenStackPtr gen_stack_init();

bool gen_stack_pop(GenStackPtr stack);

bool gen_stack_push(GenStackPtr stack, TokenPtr token, bool op);

void gen_stack_dispose(GenStackPtr stack);