#include <stdlib.h>
#include <stdbool.h>

#define PARAM_STACK_ALLOC_BLOCK 20

typedef struct ParamStackItem {
    char* id;
    char* externalName;
    int valueType;
} *ParamStackItemPtr;

typedef struct ParamStack {
    int data_cap;
    int data_pos;
    bool empty;
    ParamStackItemPtr top;
    ParamStackItemPtr *data;
} *ParamStackPtr;

/** Initialize param
    @returns ParamStackPtr if succeded NULL otherwise
*/
ParamStackPtr param_stack_init();

/** Pop last item from stack
    @param stack ParamStackPtr from which to pop
    @returns true if succeded, false otherwise
*/
bool param_stack_pop(ParamStackPtr stack);

/** Push new item into Param Stack
 *  @param stack ParamStackPtr to add Token to
 *  @param item ParamStackItemPtr that should be added to stack
 *  @returns false if succeeded, true otherwise
*/
bool param_stack_push(ParamStackPtr stack, ParamStackItemPtr item);

/** Dispose and free Param Stack
*/
void param_stack_dispose(ParamStackPtr stack);

/** Initialize paramItem
    @returns ParamStackItemPtr if succeded NULL otherwise
*/
ParamStackItemPtr param_stack_item_init();

/** Dispose and free Param Stack Item
*/
void param_stack_item_dispose(ParamStackItemPtr item);