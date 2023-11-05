#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "scanner.h"

/* Reads characters from standard input, processes them, allocates space for new 
    data and links them to token parameter.
    parameters:
        - TokenPtr token - allocated and initialized token
    return value:
        - void*/
void get_next_token(TokenPtr token) {
    int symbol;
    int state = 0;
    bool OK = false;
    bool end = false;
    int pos = 0;
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
                } else if (is_space(symbol)) {
                    continue;
                } else if (is_operator(symbol)) {
                    switch (symbol) {
                        case ''
                    }
                } else {
                    end = true;
                }
                break;

            case 1:
                // reading an integer
                if (is_number(symbol)) {
                    OK = true;
                } else {
                    end = true;
                }
                break;
            case 2:
                // reading an identificator
                if (is_number(symbol) || is_letter(symbol) || (symbol == '_')) {
                    OK = true;
                } else {
                    end = true;
                }
                break;
            case 3:
                // read '_', expecting letters/numbers, becomes an identificator
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


    
    get_next_token(token);
    printf("type: %s, data: %s\n", token->type, token->data);
    get_next_token(token);
    printf("type: %s, data: %s\n", token->type, token->data);
}