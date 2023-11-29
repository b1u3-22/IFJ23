/**
 *  Project:    Implementace překladače imperativního jazyka IFJ23.
 *  File:       @brief Hlavičkový soubor pro rule_stack
 *  Authors:    @author Jiří Sedlák xsedla2e
*/

#include <stdlib.h>
#include <stdbool.h>

#define RULE_STACK_ALLOC_BLOCK 20

typedef struct RuleStackItem {
    int type;
    bool rule;
    bool function;
} *RuleStackItemPtr;

typedef struct RuleStack {
    int data_cap;
    int data_pos;
    bool empty;
    RuleStackItemPtr top;
    RuleStackItemPtr *data;
} *RuleStackPtr;

/** Initialize token
    @returns RuleStackPtr if succeded NULL otherwise
*/
RuleStackPtr rule_stack_init();

/** Pop last item from stack
    @param stack RuleStackPtr from which to pop
    @returns true if succeded, false otherwise
*/
bool rule_stack_pop(RuleStackPtr stack);

/** Push new Token into Token Stack
 *  @param stack RuleStackPtr to add Token to
 *  @param token TokenPtr that should be added to stack
 *  @returns false if succeeded, true otherwise
*/
bool rule_stack_push(RuleStackPtr stack, int type, bool rule, bool function);

/** Dispose and free Token Stack
*/
void rule_stack_dispose(RuleStackPtr stack);