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

    return item;
}

SymTablePtr symtable_init() {
    SymTablePtr symtable = calloc(SYMBTABLE_SIZE, sizeof(SymTableItemPtr));

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
    
    while (!item) {
        if (item->depth == depth && item->block == block)   return item;
        else if (item->depth < depth)   return item;
        else item = item->next;
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