#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "scanner.h"

/** Reads 1 token worth of data from input.
 * @param token pointer to allocated and initilaized token
*/
void get_next_token(TokenPtr token) { 
    int state = 0; // reflects the current state (position in the FSA)
    bool end = false; // signals that we read a whole token, 

    int c;

    while (true) {        
        c = getchar(); 
        if (c == EOF) break;

        switch (state) {
            case 0:
                if      (is_number(c))      state = 1;
                else if (is_letter(c))      state = 2;
                else if (c == '_')          state = 3;
                else if (is_space(c))       continue;
                else if (is_operator(c))    state = 4;
                else                        end = true;

                break;
            case 1: // INT
                if (!is_number(c)) end = true;
                break;
            case 2: // ID
                if (!is_number(c) && !is_letter(c) && (c != '_')) end = true;
                break;
            case 3: // read '_', expecting letters/numbers/_, becomes ID
                if (is_number(c) || is_letter(c) || (c == '_')) state = 2;
                else end = true; // _ cannot be ID, so error
                break;
        }

        if (end) {
            ungetc(c, stdin); // return back the last read char
            break;
        } else if (state == 4) break;

        token_add_data(token, c);
    }

    if      (state == 1)    token->type = "INT";
    else if (state == 2)    token->type = "ID";
    else if (state == 3)    token->type = "ERR";
    else if (state == 4) {
        switch (c) {
            case '*': token->type = "*"; break;
            case '/': token->type = "/"; break;
            case '+': token->type = "+"; break;
            case '-': token->type = "-"; break; 
        }
    }
}


int main() {
    // basic test usage

    TokenPtr token = token_init();
    get_next_token(token);
    printf("%s %s\n", token->data, token->type);

}