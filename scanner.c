#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "scanner.h"
#include <stdio.h>

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

void parse_id(char c, bool* end) {
    if (!is_number(c) && !is_letter(c) && (c != '_')) *end = true;
}

void parse__id(int c, scanner_states* state, bool* true_end) {
    if (is_number(c) || is_letter(c) || (c == '_')) *state = IDENTIFICATOR;
    else *true_end = true; // _ cannot be IDEN, so error
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
    if (c == '\"') *true_end = true;
    else if (c == '\\') *state = STR_B;
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

void parse_str_b(int c, scanner_states* state, bool* true_end) {
    int i = 0;
    while (ALLOWED_BACKSLASH_CHARS[i]) {
        if (c == ALLOWED_BACKSLASH_CHARS[i++]) {
            *state = STR;
            return;
        }
    }
    *true_end = true;
}

token_types get_token_type(scanner_states* state, char c, char* data) {
    if (*state == IDENTIFICATOR) {
        int i = is_keyword(data);
        if (i > -1) {
            switch (i) {
                case 0: return IF;
                case 1: return ELSE;
                case 2: return FUNC;
                case 3: return RETURN;
                case 4: return LET;
                case 5: return VAR;
                case 6: return NIL;
                case 7: return WHILE;
                default: return ERROR;
            }
        } 
        i = is_type(data);
        if (i > -1) return TYPE;
        return ID;
    }
    else if ((*state == FLP) || (*state == FLPE) || (*state == STR) || (*state == INT)) return VALUE;
    else if (*state == NL)          return NEWLINE;
    else if (*state == OP) {
        switch (c) {
            case '(': return L_BRAC;
            case ')': return R_BRAC;
            case '{': return L_CBRAC;
            case '}': return R_CBRAC;
            case '+': return PLUS;
            case '/': return DIV;
            case '!': return E_MARK;
            case '=': return EQUALS;
            case ':': return D_DOT;
            case '*': return MULT;
            case ',': return COMMA;
            default : return ERROR;
        }
    } else if (*state == QMARK)     return ERROR; // single question mark is not valid
    else if (*state == QQMARK)      return QQ_MARK;
    else if (*state == ARR)         return ARROW; 
    else if (*state == DASH)        return MINUS;
    else if (*state == END_OF_FILE) return END;
    return ERROR;
}





void get_next_token(TokenPtr token) { 
    scanner_states state = START; // reflects the current state (position in the FSA)
    bool end = false; // signals that we read a whole token and accidentaly read another char which we need to put back
    bool true_end = false; // we read the whole token and there is no need to put anything back
    int c; // read char from stdin

    while (true) {        
        c = getchar(); 
        if (c == EOF) break;

        switch (state) {
            case START:
                if      (is_number(c))      state = INT;
                else if (is_letter(c))      state = IDENTIFICATOR;
                else if (c == '_')          state = _IDENTIFICATOR;
                else if (is_space(c))       continue;
                else if (is_operator(c)) {
                    state = OP;
                    true_end = true;
                } else if (c == '-')        state = DASH;
                else if (c == '?')          state = QMARK;
                else if (c == '\n') {
                    state = NL;
                    true_end = true;
                } else if (c == '\"')       state = STR;
                else                        true_end = true;
                break;
            case IDENTIFICATOR: // IDEN
                parse_id(c, &end);
                break;
            case _IDENTIFICATOR: // read '_', expecting letters/numbers/_, becomes IDEN
                parse__id(c, &state, &true_end);
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
                parse_str(c, &state, &end);
                break;
            case STR_B: // got backslash
                parse_str_b(c, &state, &true_end);
                break;
            case DASH:
                parse_dash(c, &state, &true_end, &end);
                break;
            case QMARK:
                parse_qmark(c, &state, &true_end);
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

    if (c == EOF) state = END_OF_FILE;

    token_add_data(token, '\0');
    token->type = get_token_type(&state, c, token->data);
}

// int main() {
//     // basic test usage
    
//     for (int i = 0; i < 3; i++) {
//         TokenPtr token = token_init();
//         get_next_token(token);
//         printf("TYPE: %d, DATA: %s\n", token->type, token->data);
//     }
// }