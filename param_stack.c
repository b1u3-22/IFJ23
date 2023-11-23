#include "Param_stack.h"


ParamStackPtr param_stack_init() {
    ParamStackPtr stack = (ParamStackPtr) malloc(sizeof(struct ParamStack)); 
    if (!stack) return NULL; // Allocate memory for stack and check it

    // initialize values
    stack->data = NULL;
    stack->data_cap = 0;
    stack->data_pos = -1;
    stack->top = NULL;
    stack->empty = true;

    return stack;
}

bool param_stack_pop(ParamStackPtr stack) {
    if (stack->empty) return false;

    free(stack->data[stack->data_pos--]);
    
    if (stack->data_pos > -1) stack->top = stack->data[stack->data_pos];
    else {
        stack->top = NULL;
        stack->empty = true;
    }
    return true;
}

bool param_stack_push(ParamStackPtr stack, int type, bool Param, bool function) {
    // if we hit the allocated capacity for stack data, increase memory by reallocation
    if ((stack->data_pos + 1) >= stack->data_cap){
        ParamStackItemPtr *new_data = realloc(stack->data, (stack->data_cap + PARAM_STACK_ALLOC_BLOCK) * sizeof(ParamStackItemPtr));
        if (!new_data) return true;

        // else set old data to new ones and increase data_cap to reflect the change
        stack->data = new_data;
        stack->data_cap += PARAM_STACK_ALLOC_BLOCK;
    }

    // add new token to data and correct top pointer
    ParamStackItemPtr new_item = (ParamStackItemPtr) malloc(sizeof(struct ParamStackItem));
    if (!new_item) return true;
    
    new_item->type = type;
    new_item->Param = Param;
    new_item->function = function;
    stack->data[++(stack->data_pos)] = new_item;
    stack->top = new_item;

    stack->empty = false;
    return false;
}

void param_stack_dispose(ParamStackPtr stack) {
    while (!(stack->empty)) param_stack_pop(stack);
    free(stack->data);
    free(stack);
}