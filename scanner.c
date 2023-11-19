#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "scanner.h"

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
    } else end = true;
}

void parse_qmark(int c, scanner_states* state, bool* true_end, bool* end) {
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

token_types get_token_type(scanner_states* state, char c) {
    if (*state == IDENTIFICATOR) return ID;
    else if ((*state == FLP) || (*state == FLPE) || (*state == STR) || (*state == INT)) return VALUE;
    else if (*state == NL) return NEWLINE;
    else if (*state == OP) {
        switch (c) {
            case '(': return L_BRAC;
            case ')': return R_BRAC;
            case '{': return L_CBRAC;
            case '}': return R_CBRAC;
            case '+': return PLUS;
            case '/': return DIV;
            case '!': return E_MARK;
            default : return ERROR;
        }
    } else if (*state == QMARK) return ERROR; // single question mark is not valid
    else if (*state == QQMARK) return QQ_MARK;
    else if (*state == ARR) return ARROW; 
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
                parse_dash(&c, &state, &true_end, &end);
                break;
            case QMARK:
                parse_qmark(&c, &state, &true_end, &end);
                break;
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


    token->type = get_token_type(&state, c);
}

int main() {
    // basic test usage
    TokenPtr token = token_init();
    get_next_token(token);
}