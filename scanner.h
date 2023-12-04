/**
 *  Project:    Implementace překladače imperativního jazyka IFJ23.
 *  File:       @brief Hlavičkový soubor pro skener; deklarace stavů (enum), funkcí, pomocných polí
 *  Authors:    @author Nikol Škvařilová xskvar11
*/

#include <stdbool.h>
#include <ctype.h>
#ifndef _TOKEN
#define _TOKEN
#include "token.h"
#endif

#ifndef _SYMTABLE
#define _SYMTABLE
#include "symtable.h"
#endif


typedef enum {
    START,
    INT,
    IDENTIFICATOR,
    _IDENTIFICATOR,
    INT_FLP,
    STR,
    SLASH_IN_STR,
    FLP,
    FLP_E,
    FLP_ES,
    FLPE,
    OP,
    NL,
    DASH,
    QMARK,
    QQMARK,
    ARR,
    END_OF_FILE,
    LESS,
    LARGER,
    LESS_EQ,
    LARGER_EQ,
    EQ,
    EQEQ,
    EXL,
    EXLEQ,
    UNDERSCORE,
    SLASH,
    LINE_COMMENT,
    BLOCK_COMMENT,
    BLOCK_COMMENT_OUT,
    BLOCK_COMMENT_IN,
    TYPEQ,
    C_STR,
    SIMPLE_STR_IN,
    C_STR_IN,
    SIMPLE_STR,
    C_STR_E,
    C_STR_EE,
    COMPLEX_STR,
    U_IN_STR,
    U1,
    U2,
    U3
} scanner_states;

static const char ALLOWED_BACKSLASH_CHARS[] = {'\"', 'n', 't', 'r', '\\', '\0'};  // WARNING: do not remove '\0',
                                                                                // it's used to check the end of the array

static const char ALLOWED_OPERATORS_AND_SPECIAL_SYMBOLS[] = {'+', '*', '(', ')', '{', '}', ',', ':', '\0'}; // WARNING_ do nor remove '\0

static const char KEYWORDS[9][7] = {"if", "else", "func", "return", "let", "var", "nil", "while", "\0"};
static const char TYPES[7][8] = {"Int", "Int?", "String", "String?", "Double", "Double?", "\0"};

void get_next_token(TokenPtr token);
/* Takes token and shoves it back to stdin. Callign get_next_token creates identical token. */
void unget_token(TokenPtr token);