#include "parser.h"

int parse() {
    // AnalyzerPtr analyzer = init_analyzer();
    // if (!analyzer) return 99;

    TokenStackPtr token_stack = token_stack_init();
    if (!token_stack) return 99; // If token initialization fails, return error code 99 

    // ===================== Token stacks for semantic analyzer =====================

    TokenStackPtr sa_1 = token_stack_init();
    TokenStackPtr sa_2 = token_stack_init();
    if (!sa_1 || !sa_2) return 99;

    // ===================== Token stacks for semantic analyzer =====================

    RuleStackPtr rule_stack = rule_stack_init();
    if (!rule_stack) return 99; // Token stack initialization failed, return code 99
    
    rule_stack_push(rule_stack, R_G_BODY, true, false); // Push global body rule into rule stack

    TokenPtr token = token_stack_get(token_stack);
    if (!token) return 99; // Something failed when creating and getting new token

    int rule_to_apply = 0;
    int return_code = 0;
    bool new_line = true;

    while (!(rule_stack->empty)) {
        //printf("Token type: %02d\n", token->type);
        // ===================== Handling of newlines that some things require =====================
        if (token->type == NEWLINE) {
            token_stack_pop(token_stack); // Don't save newlines in token_stack
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
        if (rule_stack->top->function) {
           apply_function(rule_stack->top->type, rule_stack, token, sa_1, sa_2);
        }

        else if (rule_stack->top->rule) { // Search for rule in LL table if we have rule at top of our rule_stack
            rule_to_apply = ll_table[rule_stack->top->type][token->type];
            printf("[%02d, %02d]: %02d\n", rule_stack->top->type, token->type, rule_to_apply);
            apply_rule(rule_to_apply, rule_stack, token_stack);
            if (rule_to_apply == 20) token = token_stack_get(token_stack); // quick fix for if and while conditions being expressions
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
    } // main while

    int counter = 0;
    if (rule_stack->empty || (rule_stack->data_pos == 0 && rule_stack->top->type == R_G_BODY)) {
        printf("Success! Nothing remaining in parser stack!\n");
    }

    else {
        
        printf("Remaining in stack:\n");
        while (!(rule_stack->empty)) {
            printf("%02d: %02d - %d\n", counter++, rule_stack->top->type, rule_stack->top->rule);
            rule_stack_pop(rule_stack);
        }
    }

    printf("Tokens in token stack:\n");
    counter = 0;
    while (!(token_stack->empty)) {
        printf("%02d: %02d\n", counter++, token_stack->top->type);
        token_stack_pop(token_stack);
    }

    printf("Tokens in sa stack 1:\n");
    counter = 0;
    while (!(sa_1->empty)) {
        printf("%02d: %02d\n", counter++, sa_1->top->type);
        token_stack_pop(sa_1);
    }

    printf("Tokens in sa stack 2:\n");
    counter = 0;
    while (!(sa_2->empty)) {
        printf("%02d: %02d\n", counter++, sa_2->top->type);
        token_stack_pop(sa_2);
    }

    //rule_stack_dispose(rule_stack);
    //token_dispose(token);
    return 0;
}

void apply_rule(int rule, RuleStackPtr stack, TokenStackPtr token_stack) {
    rule_stack_pop(stack);

    switch (rule)
    {
    case 0:
        error_skip(stack, token_stack);
        break;

    case 1:
        rule_stack_push(stack, R_G_BODY, true, false);
        rule_stack_push(stack, R_VAR_DEF, true, false);
        rule_stack_push(stack, ID, false, false);
        rule_stack_push(stack, F_P_PUSH_1, false, true);
        rule_stack_push(stack, LET, false, false);
        rule_stack_push(stack, F_P_PUSH_1, false, true);
        break;

    case 2:
        rule_stack_push(stack, R_G_BODY, true, false);
        rule_stack_push(stack, R_VAR_DEF, true, false);
        rule_stack_push(stack, ID, false, false);
        rule_stack_push(stack, F_P_PUSH_1, false, true);
        rule_stack_push(stack, VAR, false, false);
        rule_stack_push(stack, F_P_PUSH_1, false, true);
        break;

    case 3:
        rule_stack_push(stack, R_BODY, true, false);
        rule_stack_push(stack, R_VAR_DEF, true, false);
        rule_stack_push(stack, ID, false, false);
        rule_stack_push(stack, F_P_PUSH_1, false, true);
        rule_stack_push(stack, LET, false, false);
        rule_stack_push(stack, F_P_PUSH_1, false, true);
        break;

    case 4:
        rule_stack_push(stack, R_BODY, true, false);
        rule_stack_push(stack, R_VAR_DEF, true, false);
        rule_stack_push(stack, ID, false, false);
        rule_stack_push(stack, F_P_PUSH_1, false, true);
        rule_stack_push(stack, VAR, false, false);
        rule_stack_push(stack, F_P_PUSH_1, false, true);
        break;

    case 5:
        rule_stack_push(stack, F_S_VAR_DEC, false, true);

    case 6:
        rule_stack_push(stack, R_VAR_ASG, true, false);
        rule_stack_push(stack, TYPE, false, false);
        rule_stack_push(stack, F_P_PUSH_1, false, true);
        rule_stack_push(stack, D_DOT, false, false);
        break;

    case 8:
        rule_stack_push(stack, F_S_VAR_DEC, false, true);

    case 7:
    case 9:
        rule_stack_push(stack, F_S_VAR_DEF, false, true);
        rule_stack_push(stack, R_EXPR, true, false);
        rule_stack_push(stack, EQUALS, false, false);
        break;

    case 10:
        rule_stack_push(stack, R_G_BODY, true, false);
        rule_stack_push(stack, R_CBRAC, false, false);
        rule_stack_push(stack, R_BODY, true, false);
        rule_stack_push(stack, L_CBRAC, false, false);
        rule_stack_push(stack, R_F_RET_DEF, true, false);
        rule_stack_push(stack, R_BRAC, false, false);
        rule_stack_push(stack, F_S_FUN_DEF, false, true);
        rule_stack_push(stack, R_F_DEF_F, true, false);
        rule_stack_push(stack, L_BRAC, false, false);
        rule_stack_push(stack, ID, false, false);
        rule_stack_push(stack, F_P_PUSH_1, false, true);
        rule_stack_push(stack, FUNC, false, false);
        break;

    case 12:
        rule_stack_push(stack, R_F_DEF_N, true, false);
        rule_stack_push(stack, TYPE, false, false);
        rule_stack_push(stack, D_DOT, false, false);
        rule_stack_push(stack, R_F_DEF_ID, true, false);
        break;

    case 14:
        rule_stack_push(stack, R_F_DEF_N, true, false);
        rule_stack_push(stack, TYPE, false, false);
        rule_stack_push(stack, R_VAR_ASG, true, false);
        rule_stack_push(stack, D_DOT, false, false);
        rule_stack_push(stack, R_F_DEF_ID, true, false);
        rule_stack_push(stack, COMMA, false, false);
        break;

    case 15:
        rule_stack_push(stack, R_G_BODY, true, false);
        rule_stack_push(stack, R_CBRAC, false, false);
        rule_stack_push(stack, R_BODY, true, false);
        rule_stack_push(stack, L_CBRAC, false, false);
        rule_stack_push(stack, ELSE, false, false);
        rule_stack_push(stack, R_CBRAC, false, false);
        rule_stack_push(stack, R_BODY, true, false);
        rule_stack_push(stack, L_CBRAC, false, false);
        rule_stack_push(stack, R_CON_DEF, true, false);
        rule_stack_push(stack, IF, false, false);
        break;

    case 16:
        rule_stack_push(stack, R_G_BODY, true, false);
        rule_stack_push(stack, R_CBRAC, false, false);
        rule_stack_push(stack, R_BODY, true, false);
        rule_stack_push(stack, L_CBRAC, false, false);
        rule_stack_push(stack, R_CON_DEF, true, false);
        rule_stack_push(stack, WHILE, false, false);
        break;

    case 17:
        rule_stack_push(stack, R_BODY, true, false);
        rule_stack_push(stack, R_CBRAC, false, false);
        rule_stack_push(stack, R_BODY, true, false);
        rule_stack_push(stack, L_CBRAC, false, false);
        rule_stack_push(stack, ELSE, false, false);
        rule_stack_push(stack, R_CBRAC, false, false);
        rule_stack_push(stack, R_BODY, true, false);
        rule_stack_push(stack, L_CBRAC, false, false);
        rule_stack_push(stack, R_CON_DEF, true, false);
        rule_stack_push(stack, IF, false, false);
        break;

    case 18:
        rule_stack_push(stack, R_BODY, true, false);
        rule_stack_push(stack, R_CBRAC, false, false);
        rule_stack_push(stack, R_BODY, true, false);
        rule_stack_push(stack, L_CBRAC, false, false);
        rule_stack_push(stack, R_CON_DEF, true, false);
        rule_stack_push(stack, WHILE, false, false);
        break;

    case 19:
        rule_stack_push(stack, ID, false, false);
        rule_stack_push(stack, LET, false, false);
        break;

    case 21:
        rule_stack_push(stack, R_EXPR_ID, true, false);
        rule_stack_push(stack, ID, false, false);
        rule_stack_push(stack, F_P_PUSH_2, false, true);
        break;

    case 22:
        rule_stack_push(stack, R_EXPR_OP, true, false);
        rule_stack_push(stack, VALUE, false, false);
        rule_stack_push(stack, F_P_PUSH_2, false, true);
        break;

    case 23:
        rule_stack_push(stack, F_S_FUN_ASG, false, true);
        rule_stack_push(stack, R_EXPR_OP, true, false);
        rule_stack_push(stack, R_BRAC, false, false);
        rule_stack_push(stack, R_F_PAR_F, true, false);
        rule_stack_push(stack, L_BRAC, false, false);
        break;

    case 24:
    case 25:
        // PSA run from here
        token_stack_unget(token_stack);
    
    case 20:
    case 52:
        token_stack_unget(token_stack);
        rule_stack_pop(stack);
        rule_stack_push(stack, F_P_PSA, false, true);
        break;

    case 29:
        rule_stack_push(stack, R_F_PAR_ID, true, false);
        rule_stack_push(stack, ID, false, false);
        break;

    case 30:
        rule_stack_push(stack, R_F_PAR_N, true, false);
        rule_stack_push(stack, R_EXPR_OP, true, false);
        rule_stack_push(stack, VALUE, false, false);
        break;

    case 31:
        rule_stack_push(stack, R_F_PAR_N, true, false);
        rule_stack_push(stack, R_EXPR, true, false);
        rule_stack_push(stack, D_DOT, false, false);
        break;

    case 32:
        rule_stack_push(stack, R_F_PAR_N, true, false);
        break;

    case 33:
        rule_stack_push(stack, R_F_PAR, true, false);
        rule_stack_push(stack, COMMA, false, false);
        break;

    case 34:
        rule_stack_push(stack, R_F_PAR_ID, true, false);
        rule_stack_push(stack, ID, false, false);
        break;

    case 35:
        rule_stack_push(stack, R_F_PAR_N, true, false);
        rule_stack_push(stack, R_EXPR_OP, true, false);
        rule_stack_push(stack, VALUE, false, false);
        break;

    case 37:
        rule_stack_push(stack, R_G_BODY, true, false);
        rule_stack_push(stack, R_STAT, true, false);
        rule_stack_push(stack, ID, false, false);
        break;

    case 38:
        rule_stack_push(stack, R_BODY, true, false);
        rule_stack_push(stack, R_STAT, true, false);
        rule_stack_push(stack, ID, false, false);
        break;

    case 39:
        rule_stack_push(stack, F_S_FUN_CAL, false, true);
        rule_stack_push(stack, R_BRAC, false, false);
        rule_stack_push(stack, R_F_PAR_F, true, false);
        rule_stack_push(stack, L_BRAC, false, false);
        break;

    case 40:
        rule_stack_push(stack, F_S_VAL_ASG, false, true);
        rule_stack_push(stack, R_EXPR, true, false);
        rule_stack_push(stack, EQUALS, false, false);
        break;

    case 41:
        rule_stack_push(stack, R_RET_DEF, true, false);
        rule_stack_push(stack, RETURN, false, false);
        break;

    case 42:
        rule_stack_push(stack, R_RET_DEF, true, false);
        rule_stack_push(stack, RETURN, false, false);
        break;

    case 44:
        rule_stack_push(stack, R_EXPR, true, false);
        break;

    case 47:
        rule_stack_push(stack, R_BRAC, false, false);
        rule_stack_push(stack, R_F_PAR_F, true, false);
        rule_stack_push(stack, L_BRAC, false, false);
        break;

    case 48:
        rule_stack_push(stack, ID, false, false);
        rule_stack_push(stack, ID, false, false);
        break;

    case 49:
        rule_stack_push(stack, ID, false, false);
        rule_stack_push(stack, UNDERSCR, false, false);
        break;

    case 50:
        rule_stack_push(stack, TYPE, false, false);
        rule_stack_push(stack, F_P_PUSH_1, false, true);
        rule_stack_push(stack, ARROW, false, false);
        break;

    default:
        break;
    }
}

void apply_function(int function, RuleStackPtr rule_stack, TokenPtr token, TokenStackPtr stack_1, TokenStackPtr stack_2) {
    rule_stack_pop(rule_stack);

    switch (function) {
        case F_P_PSA:
            parse_expression(END, stack_2);
            break;
        case F_P_PUSH_1:
            token_stack_push(stack_1, token);
            break;
        case F_P_PUSH_2:
            token_stack_push(stack_2, token);
            break;
        case F_S_VAR_DEC:
            check_declaration(stack_1);
            break; 
        case F_S_VAR_DEF:
            check_definition(stack_1, stack_2);
            break;
        case F_S_VAL_ASG:
            check_value_assingment(stack_1, stack_2);
            break;
        case F_S_FUN_ASG:
            check_function_assingment(stack_1, stack_2);
            break;
        case F_S_FUN_CAL:
            check_function_call(stack_1, stack_2);
            break;
        case F_S_FUN_DEF:
            check_function_definition(stack_1);
            break;
        default:
            break;
    }
}

RuleStackItemPtr error_skip(RuleStackPtr stack, TokenStackPtr token_stack) {
    printf("Error occured!\n");
    int counter = 0;
    printf("Remaining in stack:\n");
    while (!(stack->empty)) {
        printf("%02d: %02d - %d\n", counter++, stack->top->type, stack->top->rule);
        rule_stack_pop(stack);
    }
    printf("Tokens in token stack:\n");
    counter = 0;
    while (!(token_stack->empty)) {
        printf("%02d: %02d\n", counter++, token_stack->top->type);
        token_stack_pop(token_stack);
    }
    exit(2);
}


int main(){
    parse();
    return 0;
}