/**
 *  Project:    Implementace překladače imperativního jazyka IFJ23.
 *  File:       @brief Pomocná struktura pro parser implementovaná jako stack
 *  Authors:    @author Jiří Sedlák xsedla2e
*/

#include "rule_stack.h"


RuleStackPtr rule_stack_init() {
    RuleStackPtr stack = (RuleStackPtr) malloc(sizeof(struct RuleStack)); 
    if (!stack) return NULL; // Allocate memory for stack and check it

    // initialize values
    stack->data = NULL;
    stack->data_cap = 0;
    stack->data_pos = -1;
    stack->top = NULL;
    stack->empty = true;

    return stack;
}

bool rule_stack_pop(RuleStackPtr stack) {
    if (stack->empty) return true;

    free(stack->data[stack->data_pos--]);
    
    if (stack->data_pos > -1) stack->top = stack->data[stack->data_pos];
    else {
        stack->top = NULL;
        stack->empty = true;
    }
    return false;
}

bool rule_stack_push(RuleStackPtr stack, int type, bool rule, bool function) {
    // if we hit the allocated capacity for stack data, increase memory by reallocation
    if ((stack->data_pos + 1) >= stack->data_cap){
        RuleStackItemPtr *new_data = realloc(stack->data, (stack->data_cap + RULE_STACK_ALLOC_BLOCK) * sizeof(RuleStackItemPtr));
        if (!new_data) return true;

        // else set old data to new ones and increase data_cap to reflect the change
        stack->data = new_data;
        stack->data_cap += RULE_STACK_ALLOC_BLOCK;
    }

    // add new token to data and correct top pointer
    RuleStackItemPtr new_item = (RuleStackItemPtr) malloc(sizeof(struct RuleStackItem));
    if (!new_item) return true;
    
    new_item->type = type;
    new_item->rule = rule;
    new_item->function = function;
    stack->data[++(stack->data_pos)] = new_item;
    stack->top = new_item;

    stack->empty = false;
    return false;
}

void rule_stack_dispose(RuleStackPtr stack) {
    while (!(stack->empty)) rule_stack_pop(stack);
    free(stack->data);
    free(stack);
}