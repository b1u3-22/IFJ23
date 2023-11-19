#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define ALLOC_BLOCK 16

typedef enum {
    ID,
    TYPE,
    VALUE,
    IF,
    WHILE,
    L_BRAC,
    R_BRAC,
    L_CBRAC,
    R_CBRAC,
    NEWLINE,
    VAR,
    LET,
    NIL,
    PLUS,
    MINUS,
    DIV,
    E_MARK,
    Q_MARK,
    QQ_MARK,
    ERROR,
    ARROW
} token_types;

typedef struct Token {
    char *data;
    int data_len;
    int data_allocd;
    token_types type;
} *TokenPtr;

/** Initialize token
 * @returns true if succeded, false otherwise
*/
TokenPtr token_init();

/** Add new char to token data
 * @param c new char
 * @param token pointer to token to which c should be added
 * @returns true if succeded, false otherwise
*/
bool token_add_data(TokenPtr token, char c);

/** Delete token and free all memory */
void token_dispose(TokenPtr token);