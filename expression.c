/**
 *  Project:    Implementace překladače imperativního jazyka IFJ23.
 *  File:       @brief Implementace precedenčního syntaktického analýzy,
 *              generování kódu pro výrazy
 *  Authors:    @author Jiří Sedlák xsedla2e
*/

#include "expression.h"

int parse_expression(AnalyzerPtr analyzer, int end_type, TokenStackPtr sa_stack) {
    ExpressionStackPtr stack = expression_stack_init();
    if (!stack) return 99;

    expression_stack_push(stack, E_END, false);
    
    TokenStackPtr token_stack = token_stack_init();
    if (!token_stack) {
        expression_stack_dispose(stack);
        return 99;
    }
    
    int action = E_ERR;
    bool rule_applied;
    int token_type;
    ExpressionStackItemPtr stack_type_helper;

    expression_get_next_token(token_stack, end_type, &token_type, sa_stack);

    do {
        stack_type_helper = stack->top;
        while (stack_type_helper->expression) {
            stack_type_helper = stack_type_helper->previous; // Find the nearest non-expression stack item
        }
        action = p_table[stack_type_helper->type][token_type];

        //if (token_stack->top->type <= VALUE) token_stack_push(sa_stack, token_stack->top);

        //printf("[%02d, %02d]: %d\n", stack_type_helper->type, token_type, action);
        switch (action) {
            case E_ERR:
                return 2;
            case E_SFT:
                expression_stack_push(stack, token_type, false);
                // Only get next token if we haven't hit the last one
                if (token_type != E_END) expression_get_next_token(token_stack, end_type, &token_type, sa_stack);
                break;
            case E_EQL:
                expression_stack_push(stack, token_type, false);
                rule_applied = apply_expression_rule(analyzer, stack, token_stack);
                if (!rule_applied) {
                    //printf("Error in PSA\n");
                    return 2; // Syntax error occured
                }
                if (token_type != E_END) expression_get_next_token(token_stack, end_type, &token_type, sa_stack);
                break;
            case E_RED:
                rule_applied = apply_expression_rule(analyzer, stack, token_stack);
                if (!rule_applied) {
                    //printf("Error in PSA\n");
                    return 2; // Syntax error occured
                }
                break;
            case E_SCS:
                //printf("Expression ended succ\n");
                return 0;
                break; 
        }
        for (int i = 0; i <= stack->data_pos; i++) {
            //printf("%d ", stack->data[i]->type);
        }
        //printf("\n");

    } while (true); // If we hit the bottom element...
    return 0;
}

void expression_get_next_token(TokenStackPtr stack, int end_type, int *type, TokenStackPtr sa_stack) {
    TokenPtr token = token_stack_get(stack);
    while (token->type == NEWLINE) {
        token_stack_pop_free(stack);
        token = token_stack_get(stack); // skip newline characters
    }
    
    // Next token is ID and previous is ID or VALUE, we have to decide if we want to continue
    // with expression parsing or not, so we search for = sign which would mark start
    // of the next expression or ( bracket which *could* mark start of function call
    if (token->type == ID && (*type == E_VALUE || *type == E_ID)) {
        token = token_stack_get(stack); // get next token
        if (token->type == EQUALS || token->type == L_BRAC) {
            token_stack_unget(stack);
            token_stack_unget(stack);
            *type = E_END;
            return;
        }
    }

    if (token->type == ID || token->type == VALUE) token_stack_push(sa_stack, token);

    *type = get_translated_type(token);

    if (*type == E_END) {
        token_stack_unget(stack);
    }
}

int get_translated_type(TokenPtr token) {
    switch (token->type)
    {
    case ID:
        return E_ID;

    case VALUE:
        return E_VALUE;

    case PLUS:
        return E_PLS;

    case MINUS:
        return E_MIN;

    case MULT:
        return E_MUL;

    case DIV:
        return E_DIV;

    case QQ_MARK:
        return E_QQ;

    case L_BRAC:
        return E_LBRAC;

    case R_BRAC:
        return E_RBRAC;
    
    case EQUALS_EQUALS:
        return E_E;

    case EXL_MARK:
        return E_EXC;

    case EXL_EQ_MARK:
        return E_NE;

    case LARGER_THAN:
        return E_GR;

    case LARGER_EQUALS:
        return E_EGR;

    case SMALLER_THAN:
        return E_SM;

    case SMALLER_EQUALS:
        return E_ESM;

    default:
        return E_END;
    }
}

bool apply_expression_rule(AnalyzerPtr analyzer, ExpressionStackPtr stack, TokenStackPtr token_stack) {
    switch (stack->top->type) {
        case E_ID:  // Rule E -> ID
            expression_stack_pop(stack);
            expression_stack_push(stack, E_END, !(stack->top->type == E_LBRAC));
            SymTableItemPtr item = get_nearest_item(analyzer, token_stack->tokens[token_stack->tokens_pos - (bool)(token_stack->top->type != ID)]->data);
            if (!item) exit(5);
            push_sym(item);
            return true;
        case E_VALUE:   // Rule E -> VALUE
            expression_stack_pop(stack);
            expression_stack_push(stack, E_END, !(stack->top->type == E_LBRAC));
            item = symtable_item_init();
            item->value = token_stack->tokens[token_stack->tokens_pos - (bool)(token_stack->top->type != VALUE)]->data;
            item->type = token_stack->tokens[token_stack->tokens_pos - (bool)(token_stack->top->type != VALUE)]->value_type;
            push_sym(item);
            return true;
        case E_RBRAC: // Rule E -> (E)
            if (
                stack->top->previous && 
                stack->top->previous->type == E_END && 
                stack->top->previous->previous &&
                stack->top->previous->previous->type == E_LBRAC
            ) {
                for (int i = 0; i < 3; i++) {
                    expression_stack_pop(stack);
                }
                expression_stack_push(stack, E_END, true);
                break;
            }

            return false;
            
        case E_END: // Rest of the rules
            if (!stack->top->previous) return false;

            switch (stack->top->previous->type) {
                case E_EXC: // E -> !E
                    expression_stack_pop(stack);
                    expression_stack_pop(stack);
                    break;
                case E_MUL: // E -> E * E
                case E_DIV: // E -> E / E
                case E_PLS: // E -> E + E
                case E_MIN: // E -> E - E
                case E_E:   // E -> E == E
                case E_NE:  // E -> E != E
                case E_GR:  // E -> E > E
                case E_SM:  // E -> E < E
                case E_EGR: // E -> E >= E
                case E_ESM: // E -> E <= E
                case E_QQ:  // E -> E ?? E
                    if (!stack->top->previous->previous) return false;
                    exp_instruction(stack->top->previous->type);
                    expression_stack_pop(stack);
                    expression_stack_pop(stack);
                    expression_stack_pop(stack);
                    break;
            }
            expression_stack_push(stack, E_END, true);

            break;
        default: // Any other type, such as operators can't be the first
            return false;
    }
    return true;
}