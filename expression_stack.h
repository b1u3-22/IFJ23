#include <stdlib.h>
#include <stdbool.h>

#ifndef _TOKEN
#define _TOKEN
#include "token.h"
#endif

#define EXPRESSION_STACK_ALLOC_BLOCK 16

typedef struct ExpressionStackItem {
    bool expression;
    int type;
    struct ExpressionStackItem *previous;
} *ExpressionStackItemPtr;

typedef struct ExpressionStack {
    bool empty;
    int data_pos;
    int data_cap;
    ExpressionStackItemPtr top;
    ExpressionStackItemPtr *data;
} *ExpressionStackPtr;

ExpressionStackPtr expression_stack_init();
bool expression_stack_push(ExpressionStackPtr stack, int type, bool expression);
bool expression_stack_pop(ExpressionStackPtr stack);
void expression_stack_dispose(ExpressionStackPtr stack);
