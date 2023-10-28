#include <stdlib.h>
#include <stdio.h>
#include "scanner.h"

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

void get_next_token(TokenPtr token) {
    int symbol;
    int pos = 0;
    int state = 0;
    bool OK = false;
    bool end = false;
    char* str = malloc(sizeof(char) * ALLOC_BLOCK);
    int str_lim = ALLOC_BLOCK;

    while (true) {
        symbol = getchar();

        switch (state) {
            case 0:
                // the init state
                OK = true;
                if (is_number(symbol)) {
                    state = 1;
                } else if (is_letter(symbol)) {
                    state = 2;
                } else if (symbol == '_') {
                    state = 3;
                } else {
                    end = true;
                }
                break;

            case 1:
                if (is_number(symbol)) {
                    OK = true;
                } else {
                    end = true;
                }
                break;
            case 2:
                if (is_number(symbol) || is_letter(symbol) || (symbol == '_')) {
                    OK = true;
                } else {
                    end = true;
                }
                break;
            case 3:
                if (is_number(symbol) || is_letter(symbol)) {
                    OK = true;
                    state = 2;
                } else {
                    end = true;
                }
                break;
        }

        if (end) {
            break;
        }
        if (OK) {
            str[pos++] = symbol;
        }
        OK = false;

        if (pos == str_lim - 1) {
            char* res = realloc(str, str_lim + ALLOC_BLOCK);
            if (!res) {
                // ERROR REALLOCATING
                continue;
            }
            str_lim += ALLOC_BLOCK;
        }

    }
    if (state == 1) {
        token->type = "INTEGER";
    } else if (state == 2) {
        token->type = "ID";
    }

    token->data = str;

}


int main() {
    // basic test usage

    TokenPtr token = (TokenPtr) malloc(sizeof(struct Token));
    init_token(token);

    get_next_token(token);
    printf("type: %s, data: %s\n", token->type, token->data);
}