/**
 *  Project:    Implementace překladače imperativního jazyka IFJ23.
 *  File:       @brief Hlavičkový soubor pro token
 *  Authors:    @author Jiří Sedlák xsedla2e
 *              @author Nikol Škvařilová xskvar11
*/


#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define TOKEN_ALLOC_BLOCK 16

typedef enum {
    ID,
    VALUE,
    TYPE,
    WHILE,
    IF,
    ELSE,
    FUNC,
    RETURN,
    LET,
    VAR,
    EQUALS,
    D_DOT,
    PLUS,
    MINUS,
    MULT,
    DIV,
    EXL_MARK,
    QQ_MARK,
    EXL_EQ_MARK,
    EQUALS_EQUALS,
    LARGER_THAN,
    LARGER_EQUALS,
    SMALLER_THAN,
    SMALLER_EQUALS,
    L_BRAC,
    R_BRAC,
    L_CBRAC,
    R_CBRAC,
    COMMA,
    ARROW,
    NIL,
    UNDERSCR,
    NEWLINE,
    END,
    ERROR
} token_types;

typedef struct Token {
    char *data;
    int data_len;
    int data_allocd;
    int type;
    int value_type;
} *TokenPtr;

/** Initialize token
 * @returns TokenPtr if succeded, NULL otherwise
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

/**Resets token to state right after inicialization*/
void token_clear(TokenPtr token);