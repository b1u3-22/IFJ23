#include "analyzer.h"

AnalyzerPtr analyzer_init(SymTablePtr symtable) {
    AnalyzerPtr analyzer = malloc(sizeof(AnalyzerPtr));
    if (!analyzer) return NULL; // Alocation and check

    // Set properties
    analyzer->depth = 0;
    analyzer->symtable = symtable;

    return analyzer;
}

int check_declaration(AnalyzerPtr analyzer, TokenStackPtr token_stack) {
    //check for redeclaration
    SymTableItemPtr itemToCompare = symtable_get_item(analyzer->symtable, token_stack->tokens[1]->data);
    if (!itemToCompare) {
        if (itemToCompare->depth == analyzer->depth && itemToCompare->block == analyzer->block[analyzer->depth]) return 3;
    }
    
    //if data type is set and its not ?, than its error maybe?
    /*
    if (token_stack->top->type == TYPE) {
        if (token_stack->tokens[2]->value_type == S_INT || token_stack->tokens[2]->value_type == S_DOUBLE || token_stack->tokens[2]->value_type == S_STRING) {
            return ; //what should be the error code????
        }
    } */
    
    //everything is okay, create new item
    SymTableItemPtr newItem = symtable_item_init();
    
    //tokens[0] = let or var
    //tokens[1] = ID
    //tokens[2] = type
    newItem->id = token_stack->tokens[1]->data;
    newItem->depth = analyzer->depth;
    newItem->block = analyzer->block[analyzer->depth];
    newItem->isFunction = false;
    newItem->isDefined = false;
    newItem->value = NULL;

    if (token_stack->top->type == TYPE) {
        newItem->type = token_stack->tokens[2]->value_type;
    } else {
        newItem->type = S_NO_TYPE;
    }

    if (token_stack->tokens[0]->type == VAR) newItem->isVar = true;
    else newItem->isVar = false;

    symtable_add_item(analyzer->symtable, newItem);

    while (!(token_stack->empty)) token_stack_pop(token_stack);

    return 0;
}

int check_definition(AnalyzerPtr analyzer, TokenStackPtr token_stack_left, TokenStackPtr token_stack_right) {
    //check for redeclaration
    SymTableItemPtr itemToCompare = symtable_get_item(analyzer->symtable, token_stack_left->tokens[1]->data);
    if (!itemToCompare) {
        if (itemToCompare->depth == analyzer->depth && itemToCompare->block == analyzer->block[analyzer->depth]) return 3;
    }

    if (check_error_5(analyzer, token_stack_right)) return 5;

    if (token_stack_left->top->type == TYPE) {
        if (check_error_7_8(analyzer, token_stack_left->top->value_type, token_stack_right)) return 7;
    } else {
        if (token_stack_right->top->type == ID) {
            //rewrite with symtable_get_item_with_smaller_depth func
            SymTableItemPtr item = symtable_get_item(analyzer->symtable, token_stack_right->top->data);
            if(check_error_7_8(analyzer, item->type, token_stack_right)) return 8;
        }
        else {
            if(check_error_7_8(analyzer, token_stack_right->top->value_type, token_stack_right)) return 8;
        }
    }

    //everything is okay, create new item
    SymTableItemPtr newItem = symtable_item_init();

    //token_stack_left->tokens[0] = let or var
    //token_stack_left->tokens[1] = ID
    //token_stack_left->tokens[2] = type
    //token_stack_right->tokens[n] - variables and constants
    newItem->id = token_stack_left->tokens[1]->data;
    newItem->depth = analyzer->depth;
    newItem->block = analyzer->block[analyzer->depth];
    newItem->isFunction = false;
    newItem->isDefined = true; //what if int? = nill
    //newItem->type = data_type; what should be here?

    if(token_stack_right->tokens_pos == 0 && token_stack_right->top->type == VALUE) {
        newItem->value = token_stack_right->top->data;
    } else {
        newItem->value = NULL;
    }

    if (token_stack_left->tokens[0]->type == VAR) newItem->isVar = true;
    else newItem->isVar = false;

    symtable_add_item(analyzer->symtable, newItem);

    return 0;
}

int check_value_assingment(AnalyzerPtr analyzer, TokenStackPtr token_stack_left, TokenStackPtr token_stack_right) {
    SymTableItemPtr itemToCompare = symtable_get_item(analyzer->symtable, token_stack_left->top->data);
    if (itemToCompare == NULL) {
        return 5;
    }
    if (!(itemToCompare->isVar) || itemToCompare->isDefined) {
        return 3;
    }

    while (!(token_stack_right->empty)) {
        if (token_stack_right->top->type == ID) {
            SymTableItemPtr item = symtable_get_item(analyzer->symtable, token_stack_right->top->data);
            if (item == NULL || item->isDefined == false) {
                return 5;
            }
        }
        if (token_stack_left->top->value_type != token_stack_right->top->value_type) {
            return 7;
        }

        token_stack_pop(token_stack_right);
    }

    return 0;
}

int check_function_assingment(AnalyzerPtr analyzer, TokenStackPtr token_stack_left, TokenStackPtr token_stack_param) {
    return 0;
}

int check_function_call(AnalyzerPtr analyzer, TokenStackPtr token_stack_left, TokenStackPtr token_stack_param) {
    return 0;
}

int check_function_definition(AnalyzerPtr analyzer, TokenStackPtr token_stack_param) {
    return 0;
}

void increase_depth(AnalyzerPtr analyzer) {
    analyzer->depth++;
    analyzer->block[analyzer->depth]++;
}

void decrease_depth(AnalyzerPtr analyzer) {
    analyzer->depth--;
}

int check_error_5(AnalyzerPtr analyzer, TokenStackPtr token_stack) {
    for (int i = 0; i < token_stack->tokens_pos+1; i++) {

        if (token_stack->tokens[i]->type == ID) {

            //rewrite with symtable_get_item_with_smaller_depth func
            SymTableItemPtr item = symtable_get_item(analyzer->symtable, token_stack->tokens[i]->data);
            if (item == NULL || item->isDefined == false) {
                return 1
            }
        }
    }

    return 0
}

int check_error_7_8(AnalyzerPtr analyzer, int data_type, TokenStackPtr token_stack) {
    for (int i = 0; i < token_stack->tokens_pos+1; i++) {

        if (token_stack->tokens[i]->type == ID) {

            //rewrite with symtable_get_item_with_smaller_depth func
            SymTableItemPtr item = symtable_get_item(analyzer->symtable, token_stack->tokens[i]->data);
            if (item->type != data_type) {
                return 1;
            }

        } else {
            if (token_stack->tokens[i]->value_type != data_type) {
                return 1;
            }
        }
    }

    return 0;
}