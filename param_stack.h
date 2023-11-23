#include <stdlib.h>
#include <stdbool.h>

#define PARAM_STACK_ALLOC_BLOCK 20

typedef struct ParamStackItem {
    int type;
    bool rule;
    bool function;
} *ParamStackItemPtr;

typedef struct RuleStack {
    int data_cap;
    int data_pos;
    bool empty;
    ParamStackItemPtr top;
    ParamStackItemPtr *data;
} *ParamStackPtr;

/** Initialize token
    @returns ParamStackPtr if succeded NULL otherwise
*/
ParamStackPtr param_stack_init();

/** Pop last item from stack
    @param stack ParamStackPtr from which to pop
    @returns true if succeded, false otherwise
*/
bool param_stack_pop(ParamStackPtr stack);

/** Push new Token into Token Stack
 *  @param stack ParamStackPtr to add Token to
 *  @param token TokenPtr that should be added to stack
 *  @returns false if succeeded, true otherwise
*/
bool param_stack_push(ParamStackPtr stack, int type, bool Param, bool function);

/** Dispose and free Token Stack
*/
void param_stack_dispose(ParamStackPtr stack);