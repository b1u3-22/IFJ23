#include <stdbool.h>
#include "global_structs.h"


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
    return (s == '+') || (s == '-') || (s == '*') || (s == '/');
}