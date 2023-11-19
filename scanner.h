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

const char ALLOWED_BACKSLASH_CHARS[] = {'\"', 'u', 'n', 't', 'r', '\\', '\0'};  // WARNING: do not remove '\0',
                                                                                // it's used to check the end of the array

const char ALLOWED_OPERATORS_AND_SPECIAL_SYMBOLS[] = {'+', '*', '/', '(', ')', '{', '}', '!', ',', ':', '=', '\0'}; // WARNING_ do nor remove '\0

void get_next_token(TokenPtr token);

/* Using the ASCII codes, check whether provided char is a number 0-9.
    parameters:
        - char s - the char you want to check
    return value:
        - true if s is a number
        - false if s is not a number
*/
bool is_number(char s) {
    return ((s >= '0') && (s <= '9'));
}

/* Using the ASCII codes, check whether provided char is a letter A-Z, a-z.
    parameters:
        - char s - the char you want to check
    return value:
        - true if s is a letter
        - false if s is not a letter
*/
bool is_letter(char s) {
    return (((s >= 'A') && (s <= 'Z')) || ((s >= 'a') && (s <= 'z')));
}

/* Check whether provided char is a blank space (space, tab, etc.).
    parameters:
        - char s - the char you want to check
    return value:
        - true if s is a space
        - false if s is not a space*/
bool is_space(char s) {
    return isspace(s);
}

bool is_operator(char s) {
    int i = 0;
    while (ALLOWED_OPERATORS_AND_SPECIAL_SYMBOLS[i]) {
        if (s == ALLOWED_OPERATORS_AND_SPECIAL_SYMBOLS[i]) return true;
        i++;
    }
    return false;
}