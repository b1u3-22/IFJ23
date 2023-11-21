#include "parser.h"

int parse() {
    TokenStackPtr token_stack = token_stack_init();
    if (!token_stack) return 99; // If token initialization fails, return error code 99 

    RuleStackPtr rule_stack = rule_stack_init();
    if (!rule_stack) return 99; // Token stack initialization failed, return code 99
    
    rule_stack_push(rule_stack, R_G_BODY, true); // Push global body rule into rule stack

    TokenPtr token = token_stack_get(token_stack);
    if (!token) return 99; // Something failed when creating and getting new token

    int rule_to_apply = 0;
    int return_code = 0;
    bool new_line = true;

    while (token->type != END || !(rule_stack->empty)) {
        printf("Token type: %02d, Type at top of stack: %02d\n", token->type, rule_stack->top->type);

        // ===================== Handling of newlines that some things require =====================
        if (token->type == NEWLINE) {
            new_line = true;
            continue;
        }

        // This allows one line if statements: if (smt) {<body>}, left curly bracket sets the newline to true
        else if (token->type == L_CBRAC) {
            new_line = true;
        }

        // Handling for tokens that always require a newline
        else if (token->type <= R_EOL && token->type > R_EOL_B && !new_line) {
            return_code = 2;
            error_skip(rule_stack, token_stack);
        }

        // Handling for tokens that require newline when current rule is body or global body
        else if (token->type <= R_EOL && token->type <= R_EOL_B && rule_stack->top->rule && (rule_stack->top->type == R_BODY || rule_stack->top->type == R_G_BODY)) {
            if (!new_line) {
                return_code = 2;
                error_skip(rule_stack, token_stack);
            }
        }

        // ==================================== End of newline handling ====================================

        if (rule_stack->top->rule) { // Search for rule in LL table if we have rule at top of our rule_stack
            rule_to_apply = ll_table[rule_stack->top->type][token->type];
            printf("[%02d, %02d]: %02d\n", rule_stack->top->type, token->type, rule_to_apply);
            apply_rule(rule_to_apply, rule_stack, token_stack);
        }

        else if (rule_stack->top->type == token->type) {
            rule_stack_pop(rule_stack);
            token = token_stack_get(token_stack);
        }

        // Token from scanner has different type than token on top of stack
        else {
            return_code = 2;
            error_skip(rule_stack, token_stack);
        }
    }

    if (rule_stack->empty) {
        printf("Success! Nothing remaining in parser stack!\n");
        return 0;
    }

    int counter = 0;
    printf("Remaining in stack:\n");
    while (!(rule_stack->empty)) {
        printf("%02d: %02d\n", counter++, rule_stack->top->type);
        rule_stack_pop(rule_stack);
    }

    rule_stack_dispose(rule_stack);
    token_dispose(token);
    return 2;
}

void apply_rule(int rule, RuleStackPtr stack, TokenStackPtr token_stack) {
    rule_stack_pop(stack);

    switch (rule)
    {
    case 0:
        error_skip(stack, token_stack);
        break;

    case 1:
        rule_stack_push(stack, R_G_BODY, true);
        rule_stack_push(stack, R_VAR_DEF, true);
        rule_stack_push(stack, ID, false);
        rule_stack_push(stack, LET, false);
        break;

    case 2:
        rule_stack_push(stack, R_G_BODY, true);
        rule_stack_push(stack, R_VAR_DEF, true);
        rule_stack_push(stack, ID, false);
        rule_stack_push(stack, VAR, false);
        break;

    case 3:
        rule_stack_push(stack, R_BODY, true);
        rule_stack_push(stack, R_VAR_DEF, true);
        rule_stack_push(stack, ID, false);
        rule_stack_push(stack, LET, false);
        break;

    case 4:
        rule_stack_push(stack, R_BODY, true);
        rule_stack_push(stack, R_VAR_DEF, true);
        rule_stack_push(stack, ID, false);
        rule_stack_push(stack, VAR, false);
        break;

    case 6:
        rule_stack_push(stack, R_VAR_ASG, true);
        rule_stack_push(stack, TYPE, false);
        rule_stack_push(stack, D_DOT, false);
        break;

    case 7:
    case 9:
        rule_stack_push(stack, R_EXPR, true);
        rule_stack_push(stack, EQUALS, false);
        break;

    case 10:
        rule_stack_push(stack, R_G_BODY, true);
        rule_stack_push(stack, R_CBRAC, false);
        rule_stack_push(stack, R_BODY, true);
        rule_stack_push(stack, L_CBRAC, false);
        rule_stack_push(stack, R_F_RET_DEF, true);
        rule_stack_push(stack, R_BRAC, false);
        rule_stack_push(stack, R_F_DEF_F, true);
        rule_stack_push(stack, L_BRAC, false);
        rule_stack_push(stack, ID, false);
        rule_stack_push(stack, FUNC, false);
        break;

    case 12:
        rule_stack_push(stack, R_F_DEF_N, true);
        rule_stack_push(stack, TYPE, false);
        rule_stack_push(stack, R_VAR_ASG, true);
        rule_stack_push(stack, D_DOT, false);
        rule_stack_push(stack, R_F_DEF_ID, true);
        break;

    case 14:
        rule_stack_push(stack, R_F_DEF_N, true);
        rule_stack_push(stack, TYPE, false);
        rule_stack_push(stack, R_VAR_ASG, true);
        rule_stack_push(stack, D_DOT, false);
        rule_stack_push(stack, R_F_DEF_ID, true);
        rule_stack_push(stack, COMMA, false);
        break;

    case 15:
        rule_stack_push(stack, R_G_BODY, true);
        rule_stack_push(stack, R_CBRAC, false);
        rule_stack_push(stack, R_BODY, true);
        rule_stack_push(stack, L_CBRAC, false);
        rule_stack_push(stack, ELSE, false);
        rule_stack_push(stack, R_CBRAC, false);
        rule_stack_push(stack, R_BODY, true);
        rule_stack_push(stack, L_CBRAC, false);
        rule_stack_push(stack, R_CON_DEF, true);
        rule_stack_push(stack, IF, false);
        break;

    case 16:
        rule_stack_push(stack, R_G_BODY, true);
        rule_stack_push(stack, R_CBRAC, false);
        rule_stack_push(stack, R_BODY, true);
        rule_stack_push(stack, L_CBRAC, false);
        rule_stack_push(stack, R_CON_DEF, true);
        rule_stack_push(stack, WHILE, false);
        break;

    case 17:
        rule_stack_push(stack, R_BODY, true);
        rule_stack_push(stack, R_CBRAC, false);
        rule_stack_push(stack, R_BODY, true);
        rule_stack_push(stack, L_CBRAC, false);
        rule_stack_push(stack, ELSE, false);
        rule_stack_push(stack, R_CBRAC, false);
        rule_stack_push(stack, R_BODY, true);
        rule_stack_push(stack, L_CBRAC, false);
        rule_stack_push(stack, R_CON_DEF, true);
        rule_stack_push(stack, IF, false);
        break;

    case 18:
        rule_stack_push(stack, R_BODY, true);
        rule_stack_push(stack, R_CBRAC, false);
        rule_stack_push(stack, R_BODY, true);
        rule_stack_push(stack, L_CBRAC, false);
        rule_stack_push(stack, R_CON_DEF, true);
        rule_stack_push(stack, WHILE, false);
        break;

    case 19:
        rule_stack_push(stack, ID, false);
        rule_stack_push(stack, LET, false);
        break;

    case 20:
        rule_stack_push(stack, R_BRAC, false);
        rule_stack_push(stack, R_EXPR, true);
        rule_stack_push(stack, L_BRAC, false);
        break;

    case 21:
        rule_stack_push(stack, R_EXPR_ID, true);
        rule_stack_push(stack, ID, false);
        break;

    case 22:
        rule_stack_push(stack, R_EXPR_OP, true);
        rule_stack_push(stack, VALUE, false);
        break;

    case 23:
        rule_stack_push(stack, R_EXPR_OP, true);
        rule_stack_push(stack, R_BRAC, false);
        rule_stack_push(stack, R_F_PAR_F, true);
        rule_stack_push(stack, L_BRAC, false);
        break;

    case 24:
    case 25:
        // PSA run from here
        token_stack_unget(token_stack);
        token_stack_unget(token_stack);
        printf("Result from expression parser: %d\n", parse_expression(END));
        break;

    case 29:
        rule_stack_push(stack, R_F_PAR_ID, true);
        rule_stack_push(stack, ID, false);
        break;

    case 30:
        rule_stack_push(stack, R_F_PAR_N, true);
        rule_stack_push(stack, R_EXPR_OP, true);
        rule_stack_push(stack, VALUE, false);
        break;

    case 31:
        rule_stack_push(stack, R_F_PAR_N, true);
        rule_stack_push(stack, R_EXPR, true);
        rule_stack_push(stack, D_DOT, false);
        break;

    case 32:
        rule_stack_push(stack, R_F_PAR_N, true);
        break;

    case 33:
        rule_stack_push(stack, R_F_PAR, true);
        rule_stack_push(stack, COMMA, false);
        break;

    case 34:
        rule_stack_push(stack, R_F_PAR_ID, true);
        rule_stack_push(stack, ID, false);
        break;

    case 35:
        rule_stack_push(stack, R_F_PAR_N, true);
        rule_stack_push(stack, R_EXPR_OP, true);
        rule_stack_push(stack, VALUE, false);
        break;

    case 37:
        rule_stack_push(stack, R_G_BODY, true);
        rule_stack_push(stack, R_STAT, true);
        rule_stack_push(stack, ID, false);
        break;

    case 38:
        rule_stack_push(stack, R_BODY, true);
        rule_stack_push(stack, R_STAT, true);
        rule_stack_push(stack, ID, false);
        break;

    case 39:
        rule_stack_push(stack, R_BRAC, false);
        rule_stack_push(stack, R_F_PAR_F, true);
        rule_stack_push(stack, L_BRAC, false);
        break;

    case 40:
        rule_stack_push(stack, R_EXPR, true);
        rule_stack_push(stack, EQUALS, false);
        break;

    case 41:
    case 42:
        rule_stack_push(stack, R_RET_DEF, true);
        rule_stack_push(stack, RETURN, false);
        break;

    case 44:
        rule_stack_push(stack, R_EXPR, true);
        break;

    case 47:
        rule_stack_push(stack, R_BRAC, false);
        rule_stack_push(stack, R_F_PAR_F, true);
        rule_stack_push(stack, L_BRAC, false);

    case 48:
        rule_stack_push(stack, ID, false);
        rule_stack_push(stack, ID, false);

    case 49:
        rule_stack_push(stack, ID, false);
        rule_stack_push(stack, UNDERSCR, false);

    case 50:
        rule_stack_push(stack, TYPE, false);
        rule_stack_push(stack, ARROW, false);

    default:
        break;
    }
}

RuleStackItemPtr error_skip(RuleStackPtr stack, TokenStackPtr token_stack) {
    printf("Error occured!\n");
    exit(2);
}


int main(){
    parse();
    return 0;
}