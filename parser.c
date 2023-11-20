#include "parser.h"

int parse() {
    TokenPtr token = token_init();
    TokenPtr expression_helper_token;
    if (!token) return 99; // If token initialization fails, return error code 99 
    TokenStackPtr token_stack = token_stack_init();
    if (!token_stack) return 99; // Token stack initialization failed, return code 99
    TokenPtr body_token = token_init();
    if (!body_token) return 99;
    body_token->type = R_G_BODY;
    token_stack_push(token_stack, body_token, true);
    get_next_token(token);
    int rule_to_apply = 0;
    int return_code = 0;
    bool new_line = true;

    while (token->type != END || !(token_stack->empty)) {
        //printf("Token type: %d, Token stack top type: %d\n", token->type, token_stack->top->token->type);
        if (token->type == NEWLINE) {
            new_line = true;
            continue;
        }

        else if (token->type == L_CBRAC) {
            new_line = true;
        }

        else if (token->type <= R_EOL && token->type > R_EOL_B) {
            if (!new_line) {
                return_code = 2;
                error_skip(token_stack, token);
            }
        }

        else if (token->type <= R_EOL && token->type <= R_EOL_B && token_stack->top->rule && (token_stack->top->token->type == R_BODY || token_stack->top->token->type == R_G_BODY)) {
            if (!new_line) {
                return_code = 2;
                error_skip(token_stack, token);
            }
        }

        if (token_stack->top->rule) {
            rule_to_apply = ll_table[token_stack->top->token->type][token->type];
            switch (rule_to_apply) {
                case 0:
                    return_code = 2;
                    error_skip(token_stack, token);
                    break;
                case 20:
                case 21:
                case 22:
                    expression_helper_token = token;
                default:
                    apply_rule(rule_to_apply, token_stack, token, expression_helper_token);
                    break;
            }
        }

        else if (token_stack->top->token->type == token->type) {
            token_stack_pop(token_stack);
            token_clear(token);
            get_next_token(token);
        }

        // Token from scanner has different type than token on top of stack
        else {
            return_code = 2;
            error_skip(token_stack, token);
        }
    }

    int counter = 0;
    printf("Remaining in stack:\n");
    while (!(token_stack->empty)) {
        printf("%02d: %02d\n", counter++, token_stack->top->token->type);
        token_stack_pop(token_stack);
    }

    token_stack_dispose(token_stack);
    token_dispose(token);
    return 0;
}

void apply_rule(int rule, TokenStackPtr stack, TokenPtr token, TokenPtr previous_token) {
    token_stack_pop(stack);

    switch (rule)
    {
    case 1:
        token_stack_push_new(stack, R_G_BODY, true);
        token_stack_push_new(stack, R_VAR_DEF, true);
        token_stack_push_new(stack, ID, false);
        token_stack_push_new(stack, LET, false);
        break;

    case 2:
        token_stack_push_new(stack, R_G_BODY, true);
        token_stack_push_new(stack, R_VAR_DEF, true);
        token_stack_push_new(stack, ID, false);
        token_stack_push_new(stack, VAR, false);
        break;

    case 3:
        token_stack_push_new(stack, R_BODY, true);
        token_stack_push_new(stack, R_VAR_DEF, true);
        token_stack_push_new(stack, ID, false);
        token_stack_push_new(stack, LET, false);
        break;

    case 4:
        token_stack_push_new(stack, R_BODY, true);
        token_stack_push_new(stack, R_VAR_DEF, true);
        token_stack_push_new(stack, ID, false);
        token_stack_push_new(stack, VAR, false);
        break;

    case 6:
        token_stack_push_new(stack, R_VAR_ASG, true);
        token_stack_push_new(stack, TYPE, false);
        token_stack_push_new(stack, D_DOT, false);
        break;

    case 7:
    case 9:
        token_stack_push_new(stack, R_EXPR, true);
        token_stack_push_new(stack, EQUALS, false);
        break;

    case 10:
        token_stack_push_new(stack, R_G_BODY, true);
        token_stack_push_new(stack, R_CBRAC, false);
        token_stack_push_new(stack, R_BODY, true);
        token_stack_push_new(stack, L_CBRAC, false);
        token_stack_push_new(stack, TYPE, false);
        token_stack_push_new(stack, ARROW, false);
        token_stack_push_new(stack, R_BRAC, false);
        token_stack_push_new(stack, R_F_DEF_F, true);
        token_stack_push_new(stack, L_BRAC, false);
        token_stack_push_new(stack, ID, false);
        token_stack_push_new(stack, FUNC, false);
        break;

    case 12:
        token_stack_push_new(stack, R_F_DEF_N, true);
        token_stack_push_new(stack, TYPE, false);
        token_stack_push_new(stack, R_VAR_ASG, true);
        token_stack_push_new(stack, D_DOT, false);
        token_stack_push_new(stack, ID, false);
        token_stack_push_new(stack, ID, false);
        break;

    case 14:
        token_stack_push_new(stack, R_F_DEF_N, true);
        token_stack_push_new(stack, TYPE, false);
        token_stack_push_new(stack, R_VAR_ASG, true);
        token_stack_push_new(stack, D_DOT, false);
        token_stack_push_new(stack, ID, false);
        token_stack_push_new(stack, ID, false);
        token_stack_push_new(stack, COMMA, false);
        break;

    case 15:
        token_stack_push_new(stack, R_G_BODY, true);
        token_stack_push_new(stack, R_CBRAC, false);
        token_stack_push_new(stack, R_BODY, true);
        token_stack_push_new(stack, L_CBRAC, false);
        token_stack_push_new(stack, ELSE, false);
        token_stack_push_new(stack, R_CBRAC, false);
        token_stack_push_new(stack, R_BODY, true);
        token_stack_push_new(stack, L_CBRAC, false);
        token_stack_push_new(stack, R_CON_DEF, true);
        token_stack_push_new(stack, IF, false);
        break;

    case 16:
        token_stack_push_new(stack, R_G_BODY, true);
        token_stack_push_new(stack, R_CBRAC, false);
        token_stack_push_new(stack, R_BODY, true);
        token_stack_push_new(stack, L_CBRAC, false);
        token_stack_push_new(stack, R_CON_DEF, true);
        token_stack_push_new(stack, WHILE, false);
        break;

    case 17:
        token_stack_push_new(stack, R_BODY, true);
        token_stack_push_new(stack, R_CBRAC, false);
        token_stack_push_new(stack, R_BODY, true);
        token_stack_push_new(stack, L_CBRAC, false);
        token_stack_push_new(stack, ELSE, false);
        token_stack_push_new(stack, R_CBRAC, false);
        token_stack_push_new(stack, R_BODY, true);
        token_stack_push_new(stack, L_CBRAC, false);
        token_stack_push_new(stack, R_CON_DEF, true);
        token_stack_push_new(stack, IF, false);
        break;

    case 18:
        token_stack_push_new(stack, R_BODY, true);
        token_stack_push_new(stack, R_CBRAC, false);
        token_stack_push_new(stack, R_BODY, true);
        token_stack_push_new(stack, L_CBRAC, false);
        token_stack_push_new(stack, R_CON_DEF, true);
        token_stack_push_new(stack, WHILE, false);
        break;

    case 19:
        token_stack_push_new(stack, ID, false);
        token_stack_push_new(stack, LET, false);
        break;

    case 20:
        token_stack_push_new(stack, R_BRAC, false);
        token_stack_push_new(stack, R_EXPR, true);
        token_stack_push_new(stack, L_BRAC, false);
        break;

    case 21:
        token_stack_push_new(stack, R_EXPR_ID, true);
        token_stack_push_new(stack, ID, false);
        break;

    case 22:
        token_stack_push_new(stack, R_EXPR_OP, true);
        token_stack_push_new(stack, VALUE, false);
        break;

    case 23:
        token_stack_push_new(stack, R_EXPR_OP, true);
        token_stack_push_new(stack, R_BRAC, false);
        token_stack_push_new(stack, R_F_PAR_F, true);
        token_stack_push_new(stack, L_BRAC, false);
        break;

    case 24:
    case 25:
        // PSA run from here
        unget_token(token);
        unget_token(previous_token);
        parse_expression();
        break;

    case 29:
        token_stack_push_new(stack, R_F_PAR_ID, true);
        token_stack_push_new(stack, ID, false);
        break;

    case 30:
        token_stack_push_new(stack, R_F_PAR_N, true);
        token_stack_push_new(stack, R_EXPR_OP, true);
        token_stack_push_new(stack, VALUE, false);
        break;

    case 31:
        token_stack_push_new(stack, R_F_PAR_N, true);
        token_stack_push_new(stack, R_EXPR, true);
        token_stack_push_new(stack, D_DOT, false);
        break;

    case 32:
        token_stack_push_new(stack, R_F_PAR_N, true);
        break;

    case 33:
        token_stack_push_new(stack, R_F_PAR, true);
        token_stack_push_new(stack, COMMA, false);
        break;

    case 34:
        token_stack_push_new(stack, R_F_PAR_ID, true);
        token_stack_push_new(stack, ID, false);
        break;

    case 35:
        token_stack_push_new(stack, R_F_PAR_N, true);
        token_stack_push_new(stack, R_EXPR_OP, true);
        token_stack_push_new(stack, VALUE, false);
        break;

    case 37:
        token_stack_push_new(stack, R_G_BODY, true);
        token_stack_push_new(stack, R_STAT, true);
        token_stack_push_new(stack, ID, false);
        break;

    case 38:
        token_stack_push_new(stack, R_BODY, true);
        token_stack_push_new(stack, R_STAT, true);
        token_stack_push_new(stack, ID, false);
        break;

    case 39:
        token_stack_push_new(stack, R_BRAC, false);
        token_stack_push_new(stack, R_F_PAR_F, true);
        token_stack_push_new(stack, L_BRAC, false);
        break;

    case 40:
        token_stack_push_new(stack, R_EXPR, true);
        token_stack_push_new(stack, EQUALS, false);
        break;

    case 41:
    case 42:
        token_stack_push_new(stack, R_RET_DEF, true);
        token_stack_push_new(stack, RETURN, false);
        break;

    case 44:
        token_stack_push_new(stack, R_EXPR, true);
        break;

    case 47:
        token_stack_push_new(stack, R_BRAC, false);
        token_stack_push_new(stack, R_F_PAR_F, true);
        token_stack_push_new(stack, L_BRAC, false);

    default:
        break;
    }
}

TokenStackItemPtr error_skip(TokenStackPtr stack, TokenPtr token) {
    printf("Error occured!\n");
    exit(2);
}


int main(){
    parse();

    return 0;
}