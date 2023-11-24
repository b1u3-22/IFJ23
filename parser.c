#include "parser.h"

int parse() {
    SymTablePtr symtable = symtable_init();
    if (!symtable) exit(99);

    AnalyzerPtr analyzer = analyzer_init(symtable);
    if (!analyzer) exit(99);

    TokenStackPtr token_stack = token_stack_init();
    if (!token_stack) exit(99); // If token initialization fails, return error code 99 

    // ===================== Token stacks for semantic analyzer =====================

    TokenStackPtr sa_1 = token_stack_init();
    TokenStackPtr sa_2 = token_stack_init();
    if (!sa_1 || !sa_2) exit(99);

    // ===================== Token stacks for semantic analyzer =====================

    RuleStackPtr rule_stack = rule_stack_init();
    if (!rule_stack) exit(99); // Token stack initialization failed, return code 99
    
    rule_stack_push(rule_stack, R_G_BODY, true, false); // Push global body rule into rule stack

    TokenPtr token = token_stack_get(token_stack);
    if (!token) exit(99); // Something failed when creating and getting new token

    int rule_to_apply = 0;
    bool new_line = true;

    while (!(rule_stack->empty)) {
        if (token->type == ERROR) exit(1); // Lexical error occured
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
            exit(2);
        }

        // Handling for tokens that require newline when current rule is body or global body
        else if (token->type <= R_EOL && token->type <= R_EOL_B && rule_stack->top->rule && (rule_stack->top->type == R_BODY || rule_stack->top->type == R_G_BODY)) {
            if (!new_line) {
                exit(2);
            }
        }

        // ==================================== End of newline handling ====================================
        if (rule_stack->top->function) {
           apply_function(rule_stack->top->type, rule_stack, token, token_stack, sa_1, sa_2, analyzer);
        }

        else if (rule_stack->top->rule) { // Search for rule in LL table if we have rule at top of our rule_stack
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
            exit(2);
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
    int errors = 0;

    switch (rule)
    {
    case 0:
        exit(2);
        break;

    case 1:
        errors += rule_stack_push(stack, R_G_BODY, true, false);
        errors += rule_stack_push(stack, R_VAR_DEF, true, false);
        errors += rule_stack_push(stack, ID, false, false);
        errors += rule_stack_push(stack, F_P_PUSH_1, false, true);
        errors += rule_stack_push(stack, LET, false, false);
        errors += rule_stack_push(stack, F_P_PUSH_1, false, true);
        break;

    case 2:
        errors += rule_stack_push(stack, R_G_BODY, true, false);
        errors += rule_stack_push(stack, R_VAR_DEF, true, false);
        errors += rule_stack_push(stack, ID, false, false);
        errors += rule_stack_push(stack, F_P_PUSH_1, false, true);
        errors += rule_stack_push(stack, VAR, false, false);
        errors += rule_stack_push(stack, F_P_PUSH_1, false, true);
        break;

    case 3:
        errors += rule_stack_push(stack, R_BODY, true, false);
        errors += rule_stack_push(stack, R_VAR_DEF, true, false);
        errors += rule_stack_push(stack, ID, false, false);
        errors += rule_stack_push(stack, F_P_PUSH_1, false, true);
        errors += rule_stack_push(stack, LET, false, false);
        errors += rule_stack_push(stack, F_P_PUSH_1, false, true);
        break;

    case 4:
        errors += rule_stack_push(stack, R_BODY, true, false);
        errors += rule_stack_push(stack, R_VAR_DEF, true, false);
        errors += rule_stack_push(stack, ID, false, false);
        errors += rule_stack_push(stack, F_P_PUSH_1, false, true);
        errors += rule_stack_push(stack, VAR, false, false);
        errors += rule_stack_push(stack, F_P_PUSH_1, false, true);
        break;

    case 5:
        errors += rule_stack_push(stack, F_P_CLEAR_2, false, true);
        errors += rule_stack_push(stack, F_P_CLEAR_1, false, true);
        errors += rule_stack_push(stack, F_G_DEF_VAR, false, true);
        errors += rule_stack_push(stack, F_S_VAR_DEC, false, true);
        break;

    case 6:
        errors += rule_stack_push(stack, R_VAR_ASG, true, false);
        errors += rule_stack_push(stack, TYPE, false, false);
        errors += rule_stack_push(stack, F_P_PUSH_1, false, true);
        errors += rule_stack_push(stack, D_DOT, false, false);
        break;

    case 8:
        errors += rule_stack_push(stack, F_P_CLEAR_2, false, true);
        errors += rule_stack_push(stack, F_P_CLEAR_1, false, true);
        errors += rule_stack_push(stack, F_G_DEF_VAR, false, true);
        errors += rule_stack_push(stack, F_S_VAR_DEC, false, true);
        break;

    case 7:
    case 9:
        errors += rule_stack_push(stack, F_P_CLEAR_2, false, true);
        errors += rule_stack_push(stack, F_P_CLEAR_1, false, true);
        errors += rule_stack_push(stack, F_G_SET_VAR, false, true);
        errors += rule_stack_push(stack, F_G_DEF_VAR, false, true);
        errors += rule_stack_push(stack, F_S_VAR_DEF, false, true);
        errors += rule_stack_push(stack, R_EXPR, true, false);
        errors += rule_stack_push(stack, EQUALS, false, false);
        break;

    case 10:
        errors += rule_stack_push(stack, R_G_BODY, true, false);
        errors += rule_stack_push(stack, R_CBRAC, false, false);
        errors += rule_stack_push(stack, F_S_DEC_DEP, false, true);
        errors += rule_stack_push(stack, R_BODY, true, false);
        errors += rule_stack_push(stack, F_S_INC_DEP, false, true);
        errors += rule_stack_push(stack, L_CBRAC, false, false);
        errors += rule_stack_push(stack, R_F_RET_DEF, true, false);
        errors += rule_stack_push(stack, R_BRAC, false, false);
        errors += rule_stack_push(stack, F_G_FUN_S, false, true);
        errors += rule_stack_push(stack, F_S_FUN_DEF, false, true); 
        errors += rule_stack_push(stack, R_F_DEF_F, true, false);
        errors += rule_stack_push(stack, L_BRAC, false, false);
        errors += rule_stack_push(stack, ID, false, false);
        errors += rule_stack_push(stack, F_P_PUSH_1, false, true);
        errors += rule_stack_push(stack, FUNC, false, false);
        break;

    case 12:
        errors += rule_stack_push(stack, R_F_DEF_N, true, false);
        errors += rule_stack_push(stack, TYPE, false, false);
        errors += rule_stack_push(stack, F_P_PUSH_2, false, true);
        errors += rule_stack_push(stack, D_DOT, false, false);
        errors += rule_stack_push(stack, R_F_DEF_ID, true, false);
        break;

    case 14:
        errors += rule_stack_push(stack, R_F_DEF_N, true, false);
        errors += rule_stack_push(stack, TYPE, false, false);
        errors += rule_stack_push(stack, R_VAR_ASG, true, false);
        errors += rule_stack_push(stack, D_DOT, false, false);
        errors += rule_stack_push(stack, R_F_DEF_ID, true, false);
        errors += rule_stack_push(stack, COMMA, false, false);
        break;

    case 15:
        errors += rule_stack_push(stack, R_G_BODY, true, false);
        errors += rule_stack_push(stack, F_G_IF_E, false, true);
        errors += rule_stack_push(stack, R_CBRAC, false, false);
        errors += rule_stack_push(stack, F_S_DEC_DEP, false, true);
        errors += rule_stack_push(stack, R_BODY, true, false);
        errors += rule_stack_push(stack, F_S_INC_DEP, false, true);
        errors += rule_stack_push(stack, L_CBRAC, false, false);
        errors += rule_stack_push(stack, ELSE, false, false);
        errors += rule_stack_push(stack, F_G_IF_EL, false, true);
        errors += rule_stack_push(stack, R_CBRAC, false, false);
        errors += rule_stack_push(stack, F_S_DEC_DEP, false, true);
        errors += rule_stack_push(stack, R_BODY, true, false);
        errors += rule_stack_push(stack, F_S_INC_DEP, false, true);
        errors += rule_stack_push(stack, L_CBRAC, false, false);
        errors += rule_stack_push(stack, F_G_IF_S, false, true);
        errors += rule_stack_push(stack, R_CON_DEF, true, false);
        errors += rule_stack_push(stack, IF, false, false);
        break;

    case 16:
        errors += rule_stack_push(stack, R_G_BODY, true, false);
        errors += rule_stack_push(stack, F_G_W_E, false, true);
        errors += rule_stack_push(stack, R_CBRAC, false, false);
        errors += rule_stack_push(stack, F_S_DEC_DEP, false, true);
        errors += rule_stack_push(stack, R_BODY, true, false);
        errors += rule_stack_push(stack, F_S_INC_DEP, false, true);
        errors += rule_stack_push(stack, L_CBRAC, false, false);
        errors += rule_stack_push(stack, F_G_W_S, false, true);
        errors += rule_stack_push(stack, R_CON_DEF, true, false);
        errors += rule_stack_push(stack, WHILE, false, false);
        break;

    case 17:
        errors += rule_stack_push(stack, R_BODY, true, false);
        errors += rule_stack_push(stack, F_G_IF_E, false, true);
        errors += rule_stack_push(stack, R_CBRAC, false, false);
        errors += rule_stack_push(stack, F_S_DEC_DEP, false, true);
        errors += rule_stack_push(stack, R_BODY, true, false);
        errors += rule_stack_push(stack, F_S_INC_DEP, false, true);
        errors += rule_stack_push(stack, L_CBRAC, false, false);
        errors += rule_stack_push(stack, ELSE, false, false);
        errors += rule_stack_push(stack, F_G_IF_EL, false, true);
        errors += rule_stack_push(stack, R_CBRAC, false, false);
        errors += rule_stack_push(stack, F_S_DEC_DEP, false, true);
        errors += rule_stack_push(stack, R_BODY, true, false);
        errors += rule_stack_push(stack, F_S_INC_DEP, false, true);
        errors += rule_stack_push(stack, L_CBRAC, false, false);
        errors += rule_stack_push(stack, F_G_IF_S, false, true);
        errors += rule_stack_push(stack, R_CON_DEF, true, false);
        errors += rule_stack_push(stack, IF, false, false);
        break;

    case 18:
        errors += rule_stack_push(stack, R_BODY, true, false);
        errors += rule_stack_push(stack, F_G_W_E, false, true);
        errors += rule_stack_push(stack, R_CBRAC, false, false);
        errors += rule_stack_push(stack, F_S_DEC_DEP, false, true);
        errors += rule_stack_push(stack, R_BODY, true, false);
        errors += rule_stack_push(stack, F_S_INC_DEP, false, true);
        errors += rule_stack_push(stack, L_CBRAC, false, false);
        errors += rule_stack_push(stack, F_G_W_S, false, true);
        errors += rule_stack_push(stack, R_CON_DEF, true, false);
        errors += rule_stack_push(stack, WHILE, false, false);
        break;

    case 19:
        errors += rule_stack_push(stack, ID, false, false);
        errors += rule_stack_push(stack, LET, false, false);
        break;

    case 21:
        errors += rule_stack_push(stack, R_EXPR_ID, true, false);
        errors += rule_stack_push(stack, ID, false, false);
        errors += rule_stack_push(stack, F_P_PUSH_2, false, true);
        break;

    case 22:
        errors += rule_stack_push(stack, R_EXPR_OP, true, false);
        errors += rule_stack_push(stack, VALUE, false, false);
        errors += rule_stack_push(stack, F_P_PUSH_2, false, true);
        break;

    case 23:
        errors += rule_stack_push(stack, F_S_FUN_ASG, false, true);
        errors += rule_stack_push(stack, R_EXPR_OP, true, false);
        errors += rule_stack_push(stack, R_BRAC, false, false);
        errors += rule_stack_push(stack, R_F_PAR_F, true, false);
        errors += rule_stack_push(stack, L_BRAC, false, false);
        break;

    case 24:
    case 25:
        errors += token_stack_unget(token_stack);

    case 20:    
    case 52:
        errors += token_stack_unget(token_stack);
        errors += rule_stack_pop(stack);
        errors += rule_stack_push(stack, F_P_GET_T, false, true);
        errors += rule_stack_push(stack, F_P_PSA, false, true);
        break;

    case 29:
        errors += rule_stack_push(stack, R_F_PAR_ID, true, false);
        errors += rule_stack_push(stack, ID, false, false);
        break;

    case 30:
        errors += rule_stack_push(stack, R_F_PAR_N, true, false);
        errors += rule_stack_push(stack, R_EXPR_OP, true, false);
        errors += rule_stack_push(stack, VALUE, false, false);
        break;

    case 31:
        errors += rule_stack_push(stack, R_F_PAR_N, true, false);
        errors += rule_stack_push(stack, R_EXPR, true, false);
        errors += rule_stack_push(stack, D_DOT, false, false);
        break;

    case 32:
        errors += rule_stack_push(stack, R_F_PAR_N, true, false);
        break;

    case 33:
        errors += rule_stack_push(stack, R_F_PAR, true, false);
        errors += rule_stack_push(stack, COMMA, false, false);
        break;

    case 34:
        errors += rule_stack_push(stack, R_F_PAR_ID, true, false);
        errors += rule_stack_push(stack, ID, false, false);
        break;

    case 35:
        errors += rule_stack_push(stack, R_F_PAR_N, true, false);
        errors += rule_stack_push(stack, R_EXPR_OP, true, false);
        errors += rule_stack_push(stack, VALUE, false, false);
        break;

    case 37:
        errors += rule_stack_push(stack, R_G_BODY, true, false);
        errors += rule_stack_push(stack, R_STAT, true, false);
        errors += rule_stack_push(stack, ID, false, false);
        errors += rule_stack_push(stack, F_P_PUSH_1, false, true);
        break;

    case 38:
        errors += rule_stack_push(stack, R_BODY, true, false);
        errors += rule_stack_push(stack, R_STAT, true, false);
        errors += rule_stack_push(stack, ID, false, false);
        errors += rule_stack_push(stack, F_P_PUSH_1, false, true);
        break;

    case 39:
        errors += rule_stack_push(stack, F_S_FUN_CAL, false, true);
        errors += rule_stack_push(stack, R_BRAC, false, false);
        errors += rule_stack_push(stack, R_F_PAR_F, true, false);
        errors += rule_stack_push(stack, L_BRAC, false, false);
        break;

    case 40:
        errors += rule_stack_push(stack, F_S_VAL_ASG, false, true);
        errors += rule_stack_push(stack, R_EXPR, true, false);
        errors += rule_stack_push(stack, EQUALS, false, false);
        break;

    case 41:
        errors += rule_stack_push(stack, R_RET_DEF, true, false);
        errors += rule_stack_push(stack, RETURN, false, false);
        break;

    case 42:
        errors += rule_stack_push(stack, R_RET_DEF, true, false);
        errors += rule_stack_push(stack, RETURN, false, false);
        break;

    case 44:
        errors += rule_stack_push(stack, R_EXPR, true, false);
        break;

    case 47:
        errors += rule_stack_push(stack, R_BRAC, false, false);
        errors += rule_stack_push(stack, R_F_PAR_F, true, false);
        errors += rule_stack_push(stack, L_BRAC, false, false);
        break;

    case 48:
        errors += rule_stack_push(stack, ID, false, false);
        errors += rule_stack_push(stack, F_P_PUSH_2, false, true);
        errors += rule_stack_push(stack, ID, false, false);
        errors += rule_stack_push(stack, F_P_PUSH_2, false, true);
        break;

    case 49:
        errors += rule_stack_push(stack, ID, false, false);
        errors += rule_stack_push(stack, F_P_PUSH_2, false, true);
        errors += rule_stack_push(stack, UNDERSCR, false, false);
        errors += rule_stack_push(stack, F_P_PUSH_2, false, true);
        break;

    case 50:
        errors += rule_stack_push(stack, TYPE, false, false);
        errors += rule_stack_push(stack, F_P_PUSH_1, false, true);
        errors += rule_stack_push(stack, ARROW, false, false);
        break;

    default:
        break;
    }

    if (errors > 0) exit(99);
}

void apply_function(int function, RuleStackPtr rule_stack, TokenPtr token, TokenStackPtr token_stack, TokenStackPtr stack_1, TokenStackPtr stack_2, AnalyzerPtr analyzer) {
    rule_stack_pop(rule_stack);
    int return_code = 0;

    switch (function) {
        case F_P_GET_T: 
            token = token_stack_get(token_stack);
            break;
        case F_P_PSA:
            if (return_code = parse_expression(END, stack_2)) exit(return_code);
            break;
        case F_P_PUSH_1:
            if (return_code = token_stack_push(stack_1, token)) exit(return_code);
            break;
        case F_P_PUSH_2:
            if (return_code = token_stack_push(stack_2, token)) exit(return_code);
            break;
        case F_S_INC_DEP:
            increase_depth(analyzer);
            break;
        case F_S_DEC_DEP:
            decrease_depth(analyzer);
            break;
        case F_S_VAR_DEC:
            if (return_code = check_declaration(analyzer, stack_1)) exit(return_code);
            break; 
        case F_S_VAR_DEF:
            if (return_code = check_definition(analyzer, stack_1, stack_2)) exit(return_code);
            break;
        case F_S_VAL_ASG:
            if (return_code = check_value_assingment(analyzer, stack_1, stack_2)) exit(return_code);
            break;
        case F_S_FUN_ASG:
            if (return_code = check_function_assingment(analyzer, stack_1, stack_2)) exit(return_code);
            break;
        case F_S_FUN_CAL:
            if (return_code = check_function_call(analyzer, stack_1, stack_2)) exit(return_code);
            break;
        case F_S_FUN_DEF:
            if (return_code = check_function_definition(analyzer, stack_1, stack_2)) exit(return_code);
            break;
        case F_G_DEF_VAR:
            //def_var(get_nearest_item(analyzer, stack_1->tokens[1]->data));
            break;  
        case F_G_SET_VAR:
            //set_var(get_nearest_item(analyzer, stack_1->tokens[1]->data));
            break;  
        case F_G_PUSH:
            break;     
        case F_G_IF_S:
            break;     
        case F_G_IF_EL:
            break;    
        case F_G_IF_E:
            break;     
        case F_G_W_S:
            break;     
        case F_G_W_E:
            break;      
        case F_G_FUN_S:
            break;    
        case F_G_FUN_P:
            break;    
        case F_G_FUN_R:
            break;    
        case F_G_FUN_C:
            break;    
        case F_G_FUN_C_P:
            break;  
        case F_G_FUN_E:
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