#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#define SYMBTABLE_SIZE 49999 // Has to be a prime number
#define SYMTABLE_ID_ALLOC_BLOCK 5

typedef struct SymTableItem {
        char *id;
        char *type;
        struct SymTableItem *next;
} *SymTableItemPtr;

typedef SymTableItemPtr *SymTablePtr;

int symtable_get_hash(char *id);
SymTableItemPtr symtable_item_init();
SymTablePtr symtable_init();
bool symtable_add_item(SymTablePtr symtable, SymTableItemPtr item);
SymTableItemPtr symtable_get_item(SymTablePtr symtable, char *id);
void symtable_item_dispose(SymTableItemPtr item);
void symtable_dispose(SymTablePtr symtable);