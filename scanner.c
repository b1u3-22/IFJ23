/**
 *  Project:    Implementace překladače imperativního jazyka IFJ23.
 *  File:       @brief Implementace stavového automatu pro parsování vstupu; výroba tokenů.
 *  Authors:    @author Nikol Škvařilová xskvar11
*/


#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "scanner.h"


void unget_token(TokenPtr token) {
    ungetc(' ', stdin);
    
    for (int i = token->data_len; i > -1; i--) {
        if (token->data[i] == '\0') continue;
        ungetc(token->data[i], stdin);
    }
}

/* Using the ASCII codes, check whether provided char is a number 0-9.
    parameters:
        - char s - the char you want to check
    return value:
        - true if s is a number
        - false if s is not a number
*/
static bool is_number(char s) {
    return ((s >= '0') && (s <= '9'));
}

/* Using the ASCII codes, check whether provided char is a letter A-Z, a-z.
    parameters:
        - char s - the char you want to check
    return value:
        - true if s is a letter
        - false if s is not a letter
*/
static bool is_letter(char s) {
    return (((s >= 'A') && (s <= 'Z')) || ((s >= 'a') && (s <= 'z')));
}

/* Check whether provided char is a blank space (space, tab, etc.).
    parameters:
        - char s - the char you want to check
    return value:
        - true if s is a space
        - false if s is not a space*/
static bool is_space(char s) {
    return isspace(s);
}

static bool is_operator(char s) {
    int i = 0;
    while (ALLOWED_OPERATORS_AND_SPECIAL_SYMBOLS[i]) {
        if (s == ALLOWED_OPERATORS_AND_SPECIAL_SYMBOLS[i]) return true;
        i++;
    }
    return false;
}

int is_keyword(char* s) {
    int i = 0;
    while (strcmp(KEYWORDS[i], "\0")) {
        if (!strcmp(s, KEYWORDS[i])) {
            return i;
            }
        i++;
    }
    return -1; 
}

int is_type(char* s) {
    int i = 0;
    while (strcmp(TYPES[i], "\0")) {
        if (!strcmp(s, TYPES[i])) return i;
        i++;
    }
    return -1;
}

void parse_int(int c, scanner_states* state, bool* end) {
    if (c == '.') *state = INT_FLP;
    else if ((c == 'e') || (c == 'E')) *state = FLP_E;
    else if (!is_number(c)) *end = true;
}

void parse_id(char c, scanner_states* state, bool* true_end, bool* end) {
    if (c == '?') {
        *state = TYPEQ;
        *true_end = true;
    } else if (!is_number(c) && !is_letter(c) && (c != '_')) *end = true;
}

void parse_underscore(int c, scanner_states* state, bool* end) {
    if (is_number(c) || is_letter(c) || (c == '_')) *state = IDENTIFICATOR;
    else *end = true;
}

void parse_int_flp(int c, scanner_states* state, bool* true_end) {
    if (is_number(c)) *state = FLP;
    else *true_end = true;
}

void parse_flp(int c, scanner_states* state, bool* end) {
    if ((c == 'e') || (c == 'E')) *state = FLP_E;
    else if (!is_number(c)) *end = true;
}

void parse_flp_e(int c, scanner_states* state, bool* true_end) {
    if ((c == '+') || (c == '-')) *state = FLP_ES;
    else if (is_number(c)) *state = FLPE;
    else *true_end = true;
}

void parse_flp_es(int c, scanner_states* state, bool* true_end) {
    if (is_number(c)) *state = FLPE;
    else *true_end = true;
}

void parse_flpe(int c, bool* end) {
    if (!is_number(c)) *end = true;
}

void parse_str(int c, scanner_states* state, bool* true_end) {
    if (c == '\"') *state = C_STR;
    else if (c == '\n') *true_end = true;
    else if (c == '\\') *state = SLASH_IN_STR;
    else *state = SIMPLE_STR_IN;
}

void parse_dash(int c, scanner_states* state, bool* true_end, bool* end) {
    if (c == '>') {
        *true_end = true;
        *state = ARR;
    } else *end = true;
}

void parse_qmark(int c, scanner_states* state, bool* true_end) {
    if (c == '?') {
        *true_end = true;
        *state = QQMARK;
    } else *true_end = true;
}

void parse_exl(int c, scanner_states* state, bool* true_end, bool* end) {
    if (c == '=') {
        *state = EXLEQ;
        *true_end = true;
    } else {
        *end = true;
    }
}

void parse_eq(int c, scanner_states* state, bool* true_end, bool* end) {
    if (c == '=') {
        *state = EQEQ;
        *true_end = true;
    } else {
        *end = true;
    }
}

void parse_less(int c, scanner_states* state, bool* true_end, bool* end) {
    if (c == '=') {
        *state = LESS_EQ;
        *true_end = true;
    } else {
        *end = true;
    }
}

void parse_larger(int c, scanner_states* state, bool* true_end, bool* end) {
    if (c == '=') {
        *state = LARGER_EQ;
        *true_end = true;
    } else {
        *end = true;
    }
}

void parse_slash(int c, scanner_states* state, bool* end, int* cb) {
    if (c == '/') *state = LINE_COMMENT;
    else if (c == '*') {
        *state = BLOCK_COMMENT;
        *cb = *cb + 1;
    }
    else {
        *end = true;
    }
}

void parse_line_comment(int c, scanner_states* state, TokenPtr token) {
    if (c == '\n') {
        *state = START;
        token_clear(token);
    }
}

void parse_block_comment(int c, scanner_states* state) {
    if (c == '*') *state = BLOCK_COMMENT_OUT;
    else if (c == '/') * state = BLOCK_COMMENT_IN;
}

void parse_block_comment_in(int c, scanner_states* state, int* cb) {
    if (c == '*') {
        *cb = *cb + 1;
    }
    *state = BLOCK_COMMENT;
} 

void parse_block_comment_out(int c, scanner_states* state, TokenPtr token, int* cb) {
    if (c == '/') {
        *cb = *cb - 1;
        if (*cb == 0) {
            token_clear(token);
            *state = START;
        }
    } else {
        *state = BLOCK_COMMENT;
    }
}

void parse_c_str(int c, scanner_states* state, bool* end) {
    if (c == '\"') *state = C_STR_IN;
    else {
        *state = SIMPLE_STR;
        *end = true;
    }
}

void parse_simple_str_in(int c, scanner_states* state, bool* true_end, bool* end) {
    if (c == '\"') {
        *state = SIMPLE_STR;
        *true_end = true;
    } else if (c == '\n') {
        *end = true;
    } else if (c == '\\') *state = SLASH_IN_STR;
}

void parse_c_str_in(int c, scanner_states* state) {
    if (c == '\"') *state = C_STR_E;
}

void parse_c_str_e(int c, scanner_states* state) {
    if (c == '\"') *state = C_STR_EE;
    else *state = C_STR_IN;
}

void parse_c_str_ee(int c, scanner_states* state, bool *true_end) {
    if (c == '\"') {
        *state = COMPLEX_STR;
        *true_end = true;
    } else *state = C_STR_IN;
}

void parse_slash_in_str(int c, scanner_states* state, bool* end) {
    int i = 0;
    while (ALLOWED_BACKSLASH_CHARS[i]) {
        if (ALLOWED_BACKSLASH_CHARS[i] == c) {
            *state = SIMPLE_STR_IN;
            return;
        }
        i++;
    }
    if (c == 'u') {
        *state = U_IN_STR;
        return;
    }

    *end = true;
}

void parse_u_in_str(int c, scanner_states* state, bool* end) {
    if (c == '{') *state = U1;
    else *end = true;
}

void parse_u1(int c, scanner_states* state, bool* end) {
    if (((c >= '0') && (c <= '9')) || ((c >= 'a') && (c <= 'f')) || ((c >= 'A') && (c <= 'F'))) *state = U2;
    else *end = true;
}

void parse_u2(int c, scanner_states* state, bool* end) {
    if (((c >= '0') && (c <= '9')) || ((c >= 'a') && (c <= 'f')) || ((c >= 'A') && (c <= 'F'))) *state = U3;
    else *end = true;
}

void parse_u3(int c, scanner_states* state, bool* end) {
    if (c == '}') *state = SIMPLE_STR_IN;
    else *end = true;
}


void get_token_type(scanner_states state, char c, TokenPtr token) {
    if (state == IDENTIFICATOR) {
        int i = is_keyword(token->data);
        if (i > -1) {
            switch (i) {
                case 0: token->type = IF;
                        break;
                case 1: token->type = ELSE;
                        break;
                case 2: token->type = FUNC;
                        break;
                case 3: token->type = RETURN;
                        break;
                case 4: token->type = LET;
                        break;
                case 5: token->type = VAR;
                        break;
                case 6: {
                    token->value_type = S_NO_TYPE;
                    token->type = VALUE;
                    break;
                }
                case 7: token->type = WHILE;
                        break;
                default: token->type = ERROR;
            }
            return;
        } 
        i = is_type(token->data);
        if ((i > -1) && (i < 6)) {
            token->value_type = i;
            token->type = TYPE;
        } else token->type = ID;
        
    } 
    else if (state == OP) {
        switch (c) {
            case '(':   token->type = L_BRAC;
                        break;
            case ')': token->type = R_BRAC;
                        break;
            case '{': token->type = L_CBRAC;
                        break;
            case '}': token->type = R_CBRAC;
                        break;
            case '+': token->type = PLUS;
                        break;
            case ':': token->type = D_DOT;
                        break;
            case '*': token->type = MULT;
                        break;
            case ',': token->type = COMMA;
                        break;
            default : token->type = ERROR;
        }
    } 
    else if ((state == FLP) || (state == FLPE) || (state == SIMPLE_STR) || (state == COMPLEX_STR) || (state == INT)) {
        if ((state == FLP) || (state == FLPE)) {
            token->value_type = S_DOUBLE;
        } else if ((state == SIMPLE_STR) || (state == COMPLEX_STR)) {
            token->value_type = S_STRING;
        } else if (state == INT) {
            token->value_type = S_INT;
        }
        token->type = VALUE;
    } else if (state == TYPEQ) {
        int i = is_type(token->data);
        if ((i == 1) || (i == 3) || (i == 5)) {
            token->value_type = i;
            token->type = TYPE;
        } else {
            token->type = ERROR;
            token->value_type = S_NO_TYPE;
        }
    }
    else if (state == NL)          token->type = NEWLINE;
    else if (state == QMARK)       token->type = ERROR; // single question mark is not valid
    else if (state == QQMARK)      token->type = QQ_MARK;
    else if (state == ARR)         token->type = ARROW; 
    else if (state == DASH)        token->type = MINUS;
    else if (state == END_OF_FILE) token->type = END;
    else if (state == EXL)         token->type = EXL_MARK;
    else if (state == EXLEQ)       token->type = EXL_EQ_MARK;
    else if (state == EQ)          token->type = EQUALS;
    else if (state == EQEQ)        token->type = EQUALS_EQUALS;
    else if (state == LARGER)      token->type = LARGER_THAN;
    else if (state == LARGER_EQ)   token->type = LARGER_EQUALS;
    else if (state == LESS)        token->type = SMALLER_THAN;
    else if (state == LESS_EQ)     token->type = SMALLER_EQUALS;
    else if (state == UNDERSCORE)  token->type = UNDERSCR;
    else if (state == SLASH)       token->type = DIV;
    else                            token->type = ERROR;
}





void get_next_token(TokenPtr token) { 
    scanner_states state = START; // reflects the current state (position in the FSA)
    bool end = false; // signals that we read a whole token and accidentaly read another char which we need to put back
    bool true_end = false; // we read the whole token and there is no need to put anything back
    int c; // read char from stdin
    int cb = 0; // number of comment blocksˇ
    token->value_type = S_NO_TYPE;

    while (true) {        
        c = getchar(); 
        if (c == EOF) break;

        switch (state) {
            case START:
                if      (is_number(c))      state = INT;
                else if (is_letter(c))      state = IDENTIFICATOR;
                else if (c == '_')          state = UNDERSCORE;
                else if (is_space(c))       continue;
                else if (c == '-')          state = DASH;
                else if (c == '?')          state = QMARK;
                else if (c == '\"')         state = STR;
                else if (c == '!')          state = EXL;
                else if (c == '=')          state = EQ;
                else if (c == '<')          state = LESS;
                else if (c == '>')          state = LARGER;
                else if (c == '/')          state = SLASH;
                else if (is_operator(c)) {
                    state = OP;
                    true_end = true;
                } 
                else if (c == '\n') {
                    state = NL;
                    true_end = true;
                } 
                else                        true_end = true;
                break;
            case IDENTIFICATOR: // IDEN
                parse_id(c, &state, &true_end, &end);
                break;
            case UNDERSCORE: // read '_', expecting letters/numbers/_, becomes IDEN
                parse_underscore(c, &state, &end);
                break;
            case INT:
                parse_int(c, &state, &end);
                break;
            case INT_FLP: // read INT, now got the floating part, expecting one numberc,
                parse_int_flp(c, &state, &true_end);
                break;
            case FLP: // got one number after dot, now just load all the numbers after that
                parse_flp(c, &state, &end);
                break;
            case FLP_E: // read INT, now got e
                parse_flp_e(c, &state, &true_end);
                break;
            case FLP_ES: // read sign (+,-) after e, expecting a number
                parse_flp_es(c, &state, &true_end);
                break;
            case FLPE: // numbers after e
                parse_flpe(c, &end);
                break;
            case STR: // got double quotation mark
                parse_str(c, &state, &true_end);
                break;
            case DASH:
                parse_dash(c, &state, &true_end, &end);
                break;
            case QMARK:
                parse_qmark(c, &state, &true_end);
                break;
            case EXL:
                parse_exl(c, &state, &true_end, &end);
                break;
            case EQ:
                parse_eq(c, &state, &true_end, &end);
                break;
            case LESS:
                parse_less(c, &state, &true_end, &end);
                break;
            case LARGER:
                parse_larger(c, &state, &true_end, &end);
                break;
            case SLASH:
                parse_slash(c, &state, &end, &cb);
                break;
            case LINE_COMMENT:
                parse_line_comment(c, &state, token);
                continue;
            case BLOCK_COMMENT:
                parse_block_comment(c, &state);
                continue;
            case BLOCK_COMMENT_OUT:
                parse_block_comment_out(c, &state, token, &cb);
                continue;
            case BLOCK_COMMENT_IN:
                parse_block_comment_in(c, &state, &cb);
                continue;
            case C_STR:
                parse_c_str(c, &state, &end);
                break;
            case SIMPLE_STR_IN:
                parse_simple_str_in(c, &state, &true_end, &end);
                break;
            case C_STR_IN:
                parse_c_str_in(c, &state);
                break;
            case C_STR_E:
                parse_c_str_e(c, &state);
                break;
            case C_STR_EE:
                parse_c_str_ee(c, &state, &true_end);
                break;
            case SLASH_IN_STR:
                parse_slash_in_str(c, &state, &end);
                break;
            case U_IN_STR:
                parse_u_in_str(c, &state, &end);
                break;
            case U1:
                parse_u1(c, &state, &end);
                break;
            case U2:
                parse_u2(c, &state, &end);
                break;
            case U3:
                parse_u3(c, &state, &end);
                break;
            default: break;
        }
        if (end) {
            ungetc(c, stdin); // return back the last read char
            break;
        }
        token_add_data(token, c);
        if (true_end) {
            break;
        }
    }

    token_add_data(token, '\0');

    if (c == EOF) {
        if (state == START) {
            token->type = END;
            return;
        } else {
            ungetc(EOF, stdin);
        }
    }
    get_token_type(state, c, token);
}

int main() {
    TokenPtr token = token_init();
    get_next_token(token);
        
    while (token->type != END) {
        printf("TYPE: %d, DATA: %s, SPECIAL TYPE: %d\n", token->type, token->data, token->value_type);
        token_clear(token);    
        get_next_token(token);
        
    }
} 