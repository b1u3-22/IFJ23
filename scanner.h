#include <stdbool.h>
#include <ctype.h>
#ifndef _TOKEN
#define _TOKEN
#include "token.h"
#endif

typedef enum {
    START,
    INT,
    IDENTIFICATOR,
    _IDENTIFICATOR,
    INT_FLP,
    STR,
    STR_B,
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
    END_OF_FILE
} scanner_states;

static const char ALLOWED_BACKSLASH_CHARS[] = {'\"', 'u', 'n', 't', 'r', '\\', '\0'};  // WARNING: do not remove '\0',
                                                                                // it's used to check the end of the array

static const char ALLOWED_OPERATORS_AND_SPECIAL_SYMBOLS[] = {'+', '*', '/', '(', ')', '{', '}', '!', ',', ':', '=', '\0'}; // WARNING_ do nor remove '\0

static const char KEYWORDS[9][7] = {"if", "else", "func", "return", "let", "var", "nil", "while", "\0"};
static const char TYPES[7][8] = {"Int", "Int?", "String", "String?", "Double", "Double?", "\0"};

void get_next_token(TokenPtr token);
void unget_token(TokenPtr token);