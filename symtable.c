#include "symtable.h"

// TODO: Write better hash function
int symtable_get_hash(char *id) {
    int hash = 0;
    for (int i = 0; i < (int) strlen(id); i++) hash+= (id[i] >> 1);

    return hash % SYMBTABLE_SIZE;
}

SymTableItemPtr symtable_item_init() {
    SymTableItemPtr item = calloc(1, sizeof(struct SymTableItem));
    if (!item) return NULL;

    item->id = NULL;
    item->type = NULL;
    item->next = NULL;

    return item;
}

SymTablePtr symtable_init() {
    SymTablePtr symtable = calloc(SYMBTABLE_SIZE, sizeof(SymTableItemPtr));

    return symtable;
}

bool symtable_add_item(SymTablePtr symtable, SymTableItemPtr item) {
    int hash = symtable_get_hash(item->id);
    if (!symtable[hash]) {
        symtable[hash] = item;
        return true;
    }

    SymTableItemPtr previous = symtable[hash];
    symtable[hash] = item;
    item->next = previous;
    return true;
}

SymTableItemPtr symtable_get_item(SymTablePtr symtable, char *id) {
    SymTableItemPtr item = symtable[symtable_get_hash(id)];
    while (item && strcmp(item->id, id)) item = item->next;
    return item;
}

void symtable_item_dispose(SymTableItemPtr item) {
    if (item->id) free(item->id);
    if (item->type) free(item->type);
    free(item);
}

void symtable_dispose(SymTablePtr symtable) {
    SymTableItemPtr current;
    SymTableItemPtr next;

    for (int column = 0; column < SYMBTABLE_SIZE; column++) {
        current = symtable[column];
        while (current) {
            next = current->next;
            symtable_item_dispose(current);
            current = next;
        }
    }
    free(symtable);
}