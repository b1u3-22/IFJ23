#include "expression_stack.h"

ExpressionStackPtr expression_stack_init() {
    ExpressionStackPtr stack = malloc(sizeof(struct ExpressionStack));
    if (!stack) return NULL;

    stack->data = calloc(EXPRESSION_STACK_ALLOC_BLOCK, sizeof(ExpressionStackItemPtr));
    if (!stack->data) {
        free(stack);
        return NULL;
    }

    stack->data_cap = EXPRESSION_STACK_ALLOC_BLOCK;
    stack->data_pos = -1;
    stack->top = NULL;
    stack->empty = true;
    return stack;
}

bool expression_stack_push(ExpressionStackPtr stack, int type, bool expression) {
    ExpressionStackItemPtr new_item = malloc(sizeof(struct ExpressionStackItem));
    if (!new_item) return false;

    new_item->type = type;
    new_item->expression = expression;
    new_item->previous = stack->top;

    stack->data_pos++;

    if (stack->data_pos >= stack->data_cap) {
        ExpressionStackItemPtr *new_data = realloc(stack->data, stack->data_cap + EXPRESSION_STACK_ALLOC_BLOCK);
        if (!new_data) {
            free(new_item);
            return false;
        }
        stack->data = new_data;
    }

    stack->data_cap += EXPRESSION_STACK_ALLOC_BLOCK;
    stack->data[stack->data_pos] = new_item;
    stack->top = stack->data[stack->data_pos];
    stack->empty = false;

    return true;
}

bool expression_stack_pop(ExpressionStackPtr stack) {
    if (stack->empty) return false;

    free(stack->data[stack->data_pos--]);

    if (stack->data_pos < 0) {
        stack->empty = true;
        stack->top = NULL;
    }

    else {
        stack->top = stack->data[stack->data_pos];
    }

    return true;
}

void expression_stack_dispose(ExpressionStackPtr stack) {
    while (!(stack->empty)) {
        expression_stack_pop(stack);
    }

    free(stack);
}
