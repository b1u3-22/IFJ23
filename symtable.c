/**
 *  Project:    Implementace překladače imperativního jazyka IFJ23.
 *  File:       @brief Implementace tabulky symbolů pomocí hash tabulky
 *  Authors:    @author Jiří Sedlák xsedla2e
 *              @author Martin Kučera xkucer0s
*/


#include "symtable.h"

// TODO: Write better hash function
int symtable_get_hash(char *id) {
    int hash = 0;
    for (int i = 0; i < (int) strlen(id); i++) hash+= (id[i] >> 1);

    return hash % SYMBTABLE_SIZE;
}

SymTableItemPtr symtable_item_init() {
    SymTableItemPtr item = calloc(1, sizeof(struct SymTableItem));
    if (!item) return NULL; // Alocation and check

    // Set properties
    item->id = NULL;
    item->next = NULL;
    item->isFunction = NULL;
    item->isVar = NULL;
    item->isDefined = NULL;
    item->value = NULL;
    item->paramStack = NULL;
    item->isLiteral = false;
    item->definedAtFuncAssign = false;
    item->isNil = false;

    return item;
}

SymTablePtr symtable_init() {
    SymTablePtr symtable = calloc(SYMBTABLE_SIZE, sizeof(SymTableItemPtr));

    symtable_add_built_in_functions(symtable);

    return symtable;
}

bool symtable_add_item(SymTablePtr symtable, SymTableItemPtr item) {
    int hash = symtable_get_hash(item->id);
    SymTableItemPtr previous = symtable[hash];
    symtable[hash] = item;
    item->next = previous;

    return true;
}

SymTableItemPtr symtable_get_item(SymTablePtr symtable, char *id) {
    SymTableItemPtr item = symtable[symtable_get_hash(id)]; // Get correct "row"
    while (item && strcmp(item->id, id)) item = item->next; // Find item with the same id
    return item;
}

SymTableItemPtr symtable_get_item_lower_depth_same_block(SymTablePtr symtable, char *id, int depth, int block) {
    SymTableItemPtr item = symtable[symtable_get_hash(id)]; // Get correct "row"
    while (item && strcmp(item->id, id)) item = item->next; // Find item with the same id
    
    while (item) {
        if (item->depth == depth && item->block == block && item->isFunction == false)   return item;
        else if (item->depth < depth)   return item;
        else item = item->next;
    }
}

SymTableItemPtr symtable_get_function_item(SymTablePtr symtable, char *id) {
    SymTableItemPtr item = symtable[symtable_get_hash(id)]; // Get correct "row"
    while (item && strcmp(item->id, id)) item = item->next; // Find item with the same id
    
    while (item) {
        if (item->isFunction)   return item;
        else    item = item->next;
    }
}

void symtable_item_dispose(SymTableItemPtr item) {
    if (item->id) free(item->id);
    if (item->value) free(item->value);
    free(item);
}

void symtable_dispose(SymTablePtr symtable) {
    SymTableItemPtr current;
    SymTableItemPtr next;

    // For every column
    for (int column = 0; column < SYMBTABLE_SIZE; column++) {
        current = symtable[column];
        // Delete every item in that row
        while (current) {
            next = current->next;
            symtable_item_dispose(current);
            current = next;
        }
    }
    free(symtable);
}

bool symtable_find_parameter_external_name(ParamStackPtr stack, char* externalName) {
    for (int i = 0; i < stack->data_pos+1; i++) {
        if (!strcmp(stack->data[i]->externalName, externalName)) return true;
    }

    return false;
}

bool symtable_find_parameter_id(ParamStackPtr stack, char* id) {
    for (int i = 0; i < stack->data_pos+1; i++) {
        if (!strcmp(stack->data[i]->id, id)) return true;
    }

    return false;
}

void symtable_add_built_in_functions(SymTablePtr symtable) {
    //readString() -> String?
    SymTableItemPtr functionReadString = symtable_item_init();
    functionReadString->depth = 0;
    functionReadString->block = 0;
    functionReadString->id = "readString";
    functionReadString->isDefined = true;
    functionReadString->isFunction = true;
    functionReadString->isVar = NULL;
    functionReadString->value = NULL;
    functionReadString->type = S_STRINGQ;
    ParamStackPtr paramStackReadString = param_stack_init();
    functionReadString->paramStack = paramStackReadString;
    symtable_add_item(symtable, functionReadString);

    //readInt() -> Int?
    SymTableItemPtr functionReadInt = symtable_item_init();
    functionReadInt->depth = 0;
    functionReadInt->block = 0;
    functionReadInt->id = "readInt";
    functionReadInt->isDefined = true;
    functionReadInt->isFunction = true;
    functionReadInt->isVar = NULL;
    functionReadInt->value = NULL;
    functionReadInt->type = S_INTQ;
    ParamStackPtr paramStackReadInt = param_stack_init();
    functionReadInt->paramStack = paramStackReadInt;
    symtable_add_item(symtable, functionReadInt);

    //readDouble() -> Double?
    SymTableItemPtr functionReadDouble = symtable_item_init();
    functionReadDouble->depth = 0;
    functionReadDouble->block = 0;
    functionReadDouble->id = "readDouble";
    functionReadDouble->isDefined = true;
    functionReadDouble->isFunction = true;
    functionReadDouble->isVar = NULL;
    functionReadDouble->value = NULL;
    functionReadDouble->type = S_DOUBLEQ;
    ParamStackPtr paramStackReadDouble = param_stack_init();
    functionReadDouble->paramStack = paramStackReadDouble;
    symtable_add_item(symtable, functionReadDouble);

    //func write ( term1 , term2 , …, term𝑛 )
    SymTableItemPtr functionWrite = symtable_item_init();
    functionWrite->depth = 0;
    functionWrite->block = 0;
    functionWrite->id = "write";
    functionWrite->isDefined = true;
    functionWrite->isFunction = true;
    functionWrite->isVar = NULL;
    functionWrite->value = NULL;
    functionWrite->type = S_NO_TYPE;
    //ParamStackPtr paramStackReadDouble = param_stack_init();
    //functionWrite->paramStack = paramStackReadDouble;
    symtable_add_item(symtable, functionWrite);

    //Int2Double(_ term : Int) -> Double
    SymTableItemPtr functionInt2Double = symtable_item_init();
    functionInt2Double->depth = 0;
    functionInt2Double->block = 0;
    functionInt2Double->id = "Int2Double";
    functionInt2Double->isDefined = true;
    functionInt2Double->isFunction = true;
    functionInt2Double->isVar = NULL;
    functionInt2Double->value = NULL;
    functionInt2Double->type = S_DOUBLE;

    ParamStackItemPtr item1 = param_stack_item_init();
    item1->externalName = "_";
    item1->id = "term";
    item1->valueType = S_INT;

    ParamStackPtr paramStack1 = param_stack_init();
    param_stack_push(paramStack1, item1);
    functionInt2Double->paramStack = paramStack1;
    symtable_add_item(symtable, functionInt2Double);

    //Double2Int(_ term : Double) -> Int
    SymTableItemPtr functionDouble2Int = symtable_item_init();
    functionDouble2Int->depth = 0;
    functionDouble2Int->block = 0;
    functionDouble2Int->id = "Double2Int";
    functionDouble2Int->isDefined = true;
    functionDouble2Int->isFunction = true;
    functionDouble2Int->isVar = NULL;
    functionDouble2Int->value = NULL;
    functionDouble2Int->type = S_INT;

    ParamStackItemPtr item2 = param_stack_item_init();
    item2->externalName = "_";
    item2->id = "term";
    item2->valueType = S_DOUBLE;

    ParamStackPtr paramStack2 = param_stack_init();
    param_stack_push(paramStack2, item2);
    functionDouble2Int->paramStack = paramStack2;
    symtable_add_item(symtable, functionDouble2Int);

    //length(_ s : String) -> Int
    SymTableItemPtr functionLength = symtable_item_init();
    functionLength->depth = 0;
    functionLength->block = 0;
    functionLength->id = "length";
    functionLength->isDefined = true;
    functionLength->isFunction = true;
    functionLength->isVar = NULL;
    functionLength->value = NULL;
    functionLength->type = S_INT;

    ParamStackItemPtr item3 = param_stack_item_init();
    item3->externalName = "_";
    item3->id = "s";
    item3->valueType = S_STRING;

    ParamStackPtr paramStack3 = param_stack_init();
    param_stack_push(paramStack3, item3);
    functionLength->paramStack = paramStack3;
    symtable_add_item(symtable, functionLength);

    //ord(_ c : String) -> Int
    SymTableItemPtr functionOrd = symtable_item_init();
    functionOrd->depth = 0;
    functionOrd->block = 0;
    functionOrd->id = "ord";
    functionOrd->isDefined = true;
    functionOrd->isFunction = true;
    functionOrd->isVar = NULL;
    functionOrd->value = NULL;
    functionOrd->type = S_INT;

    ParamStackItemPtr item4 = param_stack_item_init();
    item4->externalName = "_";
    item4->id = "c";
    item4->valueType = S_STRING;

    ParamStackPtr paramStack4 = param_stack_init();
    param_stack_push(paramStack4, item4);
    functionOrd->paramStack = paramStack4;
    symtable_add_item(symtable, functionOrd);

    //chr(_ i : Int) -> String
    SymTableItemPtr functionChr = symtable_item_init();
    functionChr->depth = 0;
    functionChr->block = 0;
    functionChr->id = "chr";
    functionChr->isDefined = true;
    functionChr->isFunction = true;
    functionChr->isVar = NULL;
    functionChr->value = NULL;
    functionChr->type = S_STRING;

    ParamStackItemPtr item5 = param_stack_item_init();
    item5->externalName = "_";
    item5->id = "i";
    item5->valueType = S_INT;

    ParamStackPtr paramStack5 = param_stack_init();
    param_stack_push(paramStack5, item5);
    functionChr->paramStack = paramStack5;
    symtable_add_item(symtable, functionChr);

    //substring(of s : String, startingAt i : Int, endingBefore j : Int) -> String?
    SymTableItemPtr functionSubstring = symtable_item_init();
    functionSubstring->depth = 0;
    functionSubstring->block = 0;
    functionSubstring->id = "substring";
    functionSubstring->isDefined = true;
    functionSubstring->isFunction = true;
    functionSubstring->isVar = NULL;
    functionSubstring->value = NULL;
    functionSubstring->type = S_STRINGQ;

    ParamStackItemPtr item6 = param_stack_item_init();
    item6->externalName = "of";
    item6->id = "s";
    item6->valueType = S_STRING;

    ParamStackItemPtr item7 = param_stack_item_init();
    item7->externalName = "startingAt";
    item7->id = "i";
    item7->valueType = S_INT;

    ParamStackItemPtr item8 = param_stack_item_init();
    item8->externalName = "endingBefore";
    item8->id = "j";
    item8->valueType = S_INT;

    ParamStackPtr paramStack6 = param_stack_init();
    param_stack_push(paramStack6, item6);
    param_stack_push(paramStack6, item7);
    param_stack_push(paramStack6, item8);
    functionSubstring->paramStack = paramStack6;
    symtable_add_item(symtable, functionSubstring);
}