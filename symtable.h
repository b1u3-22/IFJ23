#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#ifndef _PARAM_STACK
#define _PARAM_STACK
#include "param_stack.h"
#endif

#ifndef _TOKEN_STACK
#define _TOKEN_STACK
#include "token_stack.h"
#endif

#define SYMBTABLE_SIZE 49999 // Has to be a prime number
#define SYMTABLE_ID_ALLOC_BLOCK 5

enum Types {
    S_INT,
    S_INTQ,
    S_STRING,
    S_STRINGQ,
    S_DOUBLE,
    S_DOUBLEQ,
    S_NO_TYPE
};

typedef struct SymTableItem {
    char *id;                   // identifier
    int type;                   // data type
    bool isFunction;
    bool isVar;
    bool isDefined;
    struct SymTableItem *next;
    char *value;
    int depth;
    int block;
    TokenStackPtr paramStack;
} *SymTableItemPtr;

typedef SymTableItemPtr *SymTablePtr;

/** Function to get hash from id
 *  TODO: Write a better hash function :) 
*/
int symtable_get_hash(char *id);

/** Initializes SymTableItem
 *  @return SymTableItemPtr if succeded, NULL otherwise 
*/
SymTableItemPtr symtable_item_init();

/** Initializes SymTablePtr
 *  @returns SymTablePtr if succeded, NULL otherwise
*/
SymTablePtr symtable_init();

/** Adds new item to symtable
 *  This function adds the new item before any other items that
 *  have the same hash
 *  @param symtable SymTablePtr to which the item should be added
 *  @param item SymTableItemPtr that should be added to symtable
 *  @returns true if succeded, false otherwise
*/
bool symtable_add_item(SymTablePtr symtable, SymTableItemPtr item);

/** Gets item from symtable by id
 *  @returns SymTableItemPtr if item is found, NULL otherwise
*/
SymTableItemPtr symtable_get_item(SymTablePtr symtable, char *id);

/**
 * Gets item from symtable by id with same depth and block or with lower depth
 * @returns SymTatbleItemPtr if item is found, NULL otherwise
*/
SymTableItemPtr symtable_get_item_lower_depth_same_block(SymTablePtr symtable, char *id, int depth, int block);

/**
 * Gets item from symtable with atribute isFunction = true
 * @returns SymTableItemPtr if item is found, NULL otherwise
*/
SymTableItemPtr symtable_get_function_item(SymTablePtr symtable, char *id);

/** Correctly disposes item and it's properties
 *  @param item SymTableItemPtr that should be disposed  
*/
void symtable_item_dispose(SymTableItemPtr item);

/** Deletes ALL items from symtable and symtable itself
 *  @param symtable SymTablePtr that should be disposed
*/
void symtable_dispose(SymTablePtr symtable);