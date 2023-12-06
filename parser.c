/**
 *  Project:    Implementace překladače imperativního jazyka IFJ23.
 *  File:       @brief Implementace syntaktické analýzy pomocí prediktivní 
 *              analýzy řízené LL-tabulkou
 *  Authors:    @author Jiří Sedlák xsedla2e
*/

#include "parser.h"
#include <stdlib.h>
#include <stdio.h>

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
    TokenStackPtr sa_3 = token_stack_init();
    if (!sa_1 || !sa_2 || !sa_3) exit(99);

    // ===================== Token stacks for semantic analyzer =====================

    RuleStackPtr rule_stack = rule_stack_init();
    if (!rule_stack) exit(99); // Token stack initialization failed, return code 99
    
    rule_stack_push(rule_stack, R_G_BODY, true, false); // Push global body rule into rule stack

    TokenPtr token = token_stack_get(token_stack);
    if (!token) exit(99); // Something failed when creating and getting new token

    int rule_to_apply = 0;
    bool new_line = true;
    int func_ass = 0;

    while (!(rule_stack->empty)) {
        if (PARSER_DEBUG) printf("Token data: %s, Token type: %d\n", token->data, token->type);
        if (PARSER_DEBUG) printf("[%02d, %02d] - %d - %d\n", rule_stack->top->type, token->type, rule_stack->top->rule, rule_stack->top->function);
        if (token->type == ERROR) exit(1); // Lexical error occured
        // ===================== Handling of newlines that some things require =====================
        if (token->type == NEWLINE) {
            if (PARSER_DEBUG) printf("Got new line\n");
            token_stack_pop(token_stack); // Don't save newlines in token_stack
            new_line = true;
            token = token_stack_get(token_stack);
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
            if (rule_stack->top->type == F_P_GET_T) {
                rule_stack_pop(rule_stack);
                token = token_stack_get(token_stack);
            }
            else {
                apply_function(rule_stack->top->type, rule_stack, token, token_stack, sa_1, sa_2, sa_3, analyzer, rule_to_apply, &func_ass);
            }
        }

        else if (rule_stack->top->rule) { // Search for rule in LL table if we have rule at top of our rule_stack
            rule_to_apply = ll_table[rule_stack->top->type][token->type];
            if (PARSER_DEBUG) printf("[%02d, %02d]: %02d\n", rule_stack->top->type, token->type, rule_to_apply);
            apply_rule(rule_to_apply, rule_stack, token_stack, sa_2);
        }

        else if (rule_stack->top->type == token->type) {
            rule_stack_pop(rule_stack);
            token = token_stack_get(token_stack);
        }

        // Token from scanner has different type than token on top of stack
        else {
            exit(2);
            //error_skip(rule_stack, token_stack);
        }
    } // main while

    int counter = 0;
    if (rule_stack->empty || (rule_stack->data_pos == 0 && rule_stack->top->type == R_G_BODY)) {
        if (PARSER_DEBUG) printf("Success! Nothing remaining in parser stack!\n");
    }

    else {
        if (PARSER_DEBUG) {
            printf("Remaining in stack:\n");
            while (!(rule_stack->empty)) {
                printf("%02d: %02d - %d\n", counter++, rule_stack->top->type, rule_stack->top->rule);
                rule_stack_pop(rule_stack);
            }
        }
    }

        if (PARSER_DEBUG){
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
    }

    int return_code;
    if ((return_code = check_undefined_functions(analyzer))) exit(return_code);
    return 0;
}

void apply_rule(int rule, RuleStackPtr stack, TokenStackPtr token_stack, TokenStackPtr sa_stack) {
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
        errors += rule_stack_push(stack, F_S_VAR_DEF, false, true);
        errors += rule_stack_push(stack, R_EXPR, true, false);
        errors += rule_stack_push(stack, EQUALS, false, false);
        errors += rule_stack_push(stack, F_G_DEF_VAR, false, true);
        break;

    case 10:
        errors += rule_stack_push(stack, R_G_BODY, true, false);
        errors += rule_stack_push(stack, F_G_FUN_E, false, true);
        errors += rule_stack_push(stack, F_S_DEC_DEP, false, true);
        errors += rule_stack_push(stack, R_CBRAC, false, false);
        errors += rule_stack_push(stack, R_BODY, true, false);
        errors += rule_stack_push(stack, F_P_CLEAR_2, false, true);
        errors += rule_stack_push(stack, F_P_CLEAR_1, false, true);
        errors += rule_stack_push(stack, L_CBRAC, false, false);
        errors += rule_stack_push(stack, F_G_FUN_S, false, true);
        errors += rule_stack_push(stack, F_S_FUN_DEF, false, true); 
        errors += rule_stack_push(stack, R_F_RET_DEF, true, false);
        errors += rule_stack_push(stack, R_BRAC, false, false);
        errors += rule_stack_push(stack, R_F_DEF_F, true, false);
        errors += rule_stack_push(stack, L_BRAC, false, false);
        errors += rule_stack_push(stack, F_S_INC_DEP, false, true);
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
        errors += rule_stack_push(stack, F_P_PUSH_2, false, true);
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
        errors += rule_stack_push(stack, F_G_W_C, false, true);
        errors += rule_stack_push(stack, R_CON_DEF, true, false);
        errors += rule_stack_push(stack, F_G_W_S, false, true);
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
        errors += rule_stack_push(stack, F_G_W_C, false, true);
        errors += rule_stack_push(stack, R_CON_DEF, true, false);
        errors += rule_stack_push(stack, F_G_W_S, false, true);
        errors += rule_stack_push(stack, WHILE, false, false);
        break;

    case 19:
        errors += rule_stack_push(stack, ID, false, false);
        errors += rule_stack_push(stack, F_G_IF_N, false, true);
        errors += rule_stack_push(stack, LET, false, false);
        break;

    case 21:
        errors += rule_stack_push(stack, R_EXPR_ID, true, false);
        errors += rule_stack_push(stack, ID, false, false);
        errors += rule_stack_push(stack, F_G_SAVE_SYM, false, true);
        errors += rule_stack_push(stack, F_P_PUSH_3, false, true);
        errors += rule_stack_push(stack, F_P_PUSH_2, false, true);
        break;

    case 23:
        errors += rule_stack_push(stack, F_P_CLEAR_3, false, true);
        errors += rule_stack_push(stack, F_P_CLEAR_2, false, true);
        errors += rule_stack_push(stack, F_P_CLEAR_1, false, true);
        errors += rule_stack_push(stack, F_G_FUN_C, false, true);
        errors += rule_stack_push(stack, F_S_FUN_ASG, false, true);
        errors += rule_stack_push(stack, R_BRAC, false, false);
        errors += rule_stack_push(stack, R_F_PAR_F, true, false);
        errors += rule_stack_push(stack, L_BRAC, false, false);
        break;

    case 24:
        errors += rule_stack_push(stack, F_P_POP_3, false, true);
        errors += rule_stack_push(stack, F_P_POP_2, false, true);
    case 25:
        errors += token_stack_unget(token_stack);

    case 20:  
    case 22:  
    case 52:
        errors += token_stack_unget(token_stack);
        errors += rule_stack_push(stack, F_P_GET_T, false, true);
        errors += rule_stack_push(stack, F_P_PSA, false, true);
        break;

    case 30:
    case 35:
        errors += rule_stack_push(stack, R_F_PAR_N, true, false);
        errors += rule_stack_push(stack, VALUE, false, false);
        errors += rule_stack_push(stack, F_P_PUSH_3, false, true);
        errors += rule_stack_push(stack, F_P_PUSH_2, false, true);
        break;

    case 26:
        errors += rule_stack_push(stack, F_G_SYM_CONF, false, true);
        break;

    case 29:
        errors += rule_stack_push(stack, R_F_PAR_ID, true, false);
        errors += rule_stack_push(stack, ID, false, false);
        errors += rule_stack_push(stack, F_P_PUSH_3, false, true);
        errors += rule_stack_push(stack, F_P_PUSH_2, false, true);
        break;

    case 31:
        errors += rule_stack_push(stack, R_F_PAR_NA, true, false);
        errors += rule_stack_push(stack, D_DOT, false, false);
        errors += rule_stack_push(stack, F_P_POP_3, false, true);
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
        errors += rule_stack_push(stack, F_P_PUSH_3, false, true);
        errors += rule_stack_push(stack, F_P_PUSH_2, false, true);
        break;

    case 37:
        errors += rule_stack_push(stack, R_G_BODY, true, false);
        errors += rule_stack_push(stack, R_STAT, true, false);
        errors += rule_stack_push(stack, ID, false, false);
        errors += rule_stack_push(stack, F_P_PUSH_3, false, true);
        errors += rule_stack_push(stack, F_P_PUSH_2, false, true);
        errors += rule_stack_push(stack, F_P_PUSH_1, false, true);
        break;

    case 38:
        errors += rule_stack_push(stack, R_BODY, true, false);
        errors += rule_stack_push(stack, R_STAT, true, false);
        errors += rule_stack_push(stack, ID, false, false);
        errors += rule_stack_push(stack, F_P_PUSH_3, false, true);
        errors += rule_stack_push(stack, F_P_PUSH_2, false, true);
        errors += rule_stack_push(stack, F_P_PUSH_1, false, true);
        break;

    case 39:
        errors += rule_stack_push(stack, F_P_CLEAR_3, false, true);
        errors += rule_stack_push(stack, F_P_CLEAR_2, false, true);
        errors += rule_stack_push(stack, F_P_CLEAR_1, false, true);
        errors += rule_stack_push(stack, F_G_FUN_C, false, true);
        errors += rule_stack_push(stack, F_S_FUN_CAL, false, true);
        errors += rule_stack_push(stack, R_BRAC, false, false);
        errors += rule_stack_push(stack, R_F_PAR_F, true, false);
        errors += rule_stack_push(stack, L_BRAC, false, false);
        errors += rule_stack_push(stack, F_P_POP_2, false, true);
        break;

    case 40:
        errors += rule_stack_push(stack, F_G_SET_VAR, false, true);
        errors += rule_stack_push(stack, F_S_VAL_ASG, false, true);
        errors += rule_stack_push(stack, R_EXPR, true, false);
        errors += rule_stack_push(stack, EQUALS, false, false);
        errors += rule_stack_push(stack, F_P_CLEAR_3, false, true);
        errors += rule_stack_push(stack, F_P_CLEAR_2, false, true);
        break;

    case 41:
        errors += rule_stack_push(stack, F_P_CLEAR_2, false, true);
        errors += rule_stack_push(stack, F_P_CLEAR_1, false, true);
        errors += rule_stack_push(stack, R_RET_DEF, true, false);
        errors += rule_stack_push(stack, RETURN, false, false);
        break;

    case 42:
        errors += rule_stack_push(stack, F_P_CLEAR_2, false, true);
        errors += rule_stack_push(stack, F_P_CLEAR_1, false, true);
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

    case 53:
    case 54:
        break;
        //errors += rule_stack_push(stack, R_CBRAC, false, false);

    case 55:
        errors += rule_stack_push(stack, R_F_PAR_N, true, false);
        errors += rule_stack_push(stack, ID, false, false);
        errors += rule_stack_push(stack, F_P_PUSH_3, false, true);
        errors += rule_stack_push(stack, F_P_PUSH_2, false, true);
        break;

    case 56:
        errors += rule_stack_push(stack, R_F_PAR_N, true, false);
        errors += rule_stack_push(stack, VALUE, false, false);
        errors += rule_stack_push(stack, F_P_PUSH_3, false, true);
        errors += rule_stack_push(stack, F_P_PUSH_2, false, true);
        break;

    default:
        break;
    }

    if (errors > 0) exit(99);
}

void apply_function(int function, RuleStackPtr rule_stack, TokenPtr token, TokenStackPtr token_stack, TokenStackPtr stack_1, TokenStackPtr stack_2, TokenStackPtr stack_3, AnalyzerPtr analyzer, int rule, int *func_ass) {
    rule_stack_pop(rule_stack);
    int return_code = 0;
    int end_type;
    TokenStackPtr helper_stack;
    SymTableItemPtr item;

    if (!CODE_GEN && function >= F_G_DEF_VAR) return;

    switch (function) {
        case F_P_GET_T: 
            token = token_stack_get(token_stack);
            break;
        case F_P_PSA:
            if (rule == 30 || rule == 35) end_type = R_BRAC;
            else end_type = END;
            if ((return_code = parse_expression(analyzer, end_type, stack_2))) exit(return_code);
            break;
        case F_P_PUSH_1:
            if ((return_code = token_stack_push(stack_1, token))) exit(return_code);
            break;
        case F_P_PUSH_2:
            if ((return_code = token_stack_push(stack_2, token))) exit(return_code);
            break;
        case F_P_PUSH_3:
            if ((return_code = token_stack_push(stack_3, token))) exit(return_code);
            break;
        case F_P_POP_1:
            token_stack_pop(stack_1);
            break;
        case F_P_POP_2:
            token_stack_pop(stack_2);
            break;
        case F_P_POP_3:
            token_stack_pop(stack_3);
            break;
        case F_P_CLEAR_1:
            while (!stack_1->empty) token_stack_pop(stack_1);
            break;
        case F_P_CLEAR_2:
            while (!stack_2->empty) token_stack_pop(stack_2);
            break;
        case F_P_CLEAR_3:
            while (!stack_3->empty) token_stack_pop(stack_3);
            break;
        case F_S_INC_DEP:
            increase_depth(analyzer);
            break;
        case F_S_DEC_DEP:
            decrease_depth(analyzer);
            break;
        case F_S_VAR_DEC:
            if ((return_code = check_declaration(analyzer, stack_1))) exit(return_code);
            break; 
        case F_S_VAR_DEF:
            // if (*func_ass) {
            //     (*func_ass)--;
            //     break;
            // }
            if(PARSER_DEBUG) printf("Token data for check_definition(): %s\n", token->data);
            if ((return_code = check_definition(analyzer, stack_1, stack_2))) exit(return_code);
            break;
        case F_S_VAL_ASG:
            if (*func_ass) {
                (*func_ass)--;
                break;
            }
            if ((return_code = check_value_assingment(analyzer, stack_1, stack_2))) exit(return_code);
            break;
        case F_S_FUN_ASG:
            if ((return_code = check_function_assingment(analyzer, stack_1, stack_2))) exit(return_code);
            (*func_ass)++;
            break;
        case F_S_FUN_CAL:
            helper_stack = token_stack_init();
            token_stack_push(helper_stack, stack_1->top);

            for (int i = 0; i <= stack_2->tokens_pos; i++){
                token_stack_push(helper_stack, stack_2->tokens[i]);
            }

            if (PARSER_DEBUG) {
                printf("CHECK FUNCTION CALL\n");
                for (int i = 0; i <= helper_stack->tokens_pos; i++) {
                    printf("Token type: %d, token data: %s\n", helper_stack->tokens[i]->type, helper_stack->tokens[i]->data);
                }
            }

            if ((return_code = check_function_call(analyzer, helper_stack, false))) exit(return_code);
            free(helper_stack);
            break;
        case F_S_FUN_DEF:
            if ((return_code = check_function_definition(analyzer, stack_1, stack_2))) exit(return_code);
            (*func_ass)++;
            break;
        case F_G_DEF_VAR:
            def_var(stack_1->tokens[1]->data, analyzer->depth);
            break;  
        case F_G_SET_VAR:
            if (PARSER_DEBUG) {
                printf("GENERATE SET VAR STACK 1:\n");
                for (int i = 0; i <= stack_1->tokens_pos; i++){
                    printf("[%d] Token data: %s\n", i, stack_1->tokens[i]->data);
                }
            }
            if (stack_1->tokens[0]->type == VAR || stack_1->tokens[0]->type == LET) {
                set_var(get_nearest_item(analyzer, stack_1->tokens[1]->data));
            }
            else {
                set_var(get_nearest_item(analyzer, stack_1->tokens[0]->data));
            }
            
            break;  
        case F_G_PUSH:
            break;     
        case F_G_IF_N:
            item = get_nearest_item(analyzer, token->data);
            if (!item) exit(5);
            push_sym(item);
            exp_instruction(E_NUL);
            break;
        case F_G_IF_S:
            if_check();
            break;     
        case F_G_IF_EL:
            if_end();
            break;    
        case F_G_IF_E:
            if_else_end();
            break;     
        case F_G_W_S:
            while_start();
            break;     
        case F_G_W_C:
            while_check();
            break;     
        case F_G_W_E:
            while_end();
            break;      
        case F_G_FUN_S:
            func_start(stack_1->tokens[0]->data);
            SymTableItemPtr func_item = symtable_get_function_item(analyzer->symtable, stack_1->tokens[0]->data);
            for (int param = 0; param <= func_item->paramStack->data_pos; param++){
                func_param(func_item->paramStack->data[param]);
            }
            break;    
        case F_G_FUN_C:
            func_call();

            if (PARSER_DEBUG) {
                printf("GENERATE FUNCTION CALL STACK 1:\n");
                for (int i = 0; i <= stack_1->tokens_pos; i++){
                    printf("Token data: %s\n", stack_1->tokens[i]->data);
                }

                printf("GENERATE FUNCTION CALL STACK 2:\n");
                for (int i = 0; i <= stack_2->tokens_pos; i++){
                    printf("Token data: %s\n", stack_2->tokens[i]->data);
                }

                printf("GENERATE FUNCTION CALL STACK 3:\n");
                for (int i = 0; i <= stack_3->tokens_pos; i++){
                    printf("Token data: %s\n", stack_3->tokens[i]->data);
                }
            }


            for (int i = 1; i <= stack_3->tokens_pos; i++){
                if (stack_3->tokens[i]->type == ID) {
                    item = get_nearest_item(analyzer, stack_3->tokens[i]->data);
                    if (!item) exit(5); // Variable hasn't been declared yet
                }

                else if (stack_3->tokens[i]->type == VALUE) {
                    item = symtable_item_init();
                    item->type = stack_3->tokens[i]->value_type;
                    item->value = stack_3->tokens[i]->data;
                    item->isLiteral = true;
                }

                func_call_param(item);
                //symtable_item_dispose(item);
                item = NULL;
            }
            
            func_call_end(stack_3->tokens[0]->data);
            break;    
        case F_G_FUN_E:
            break;  
        case F_G_SAVE_SYM:
            item = get_nearest_item(analyzer, stack_2->tokens[0]->data);
            //item = symtable_get_function_item(analyzer->symtable, stack_2->tokens[0]->data);
            //if (!item) exit(5);
            save_sym(item);
            break;
        case F_G_SYM_CONF:
            if (token_stack->top->type != 1) confirm_sym(); // confirm sym if we didn't get here from value (i don't think this is possible anymore but I'm too scared to delete this)
            break;
        default:
            break;
    }
}

int main(){
    if (CODE_GEN) code_header();
    parse();
    if (CODE_GEN) code_footer();
    return 0;
}