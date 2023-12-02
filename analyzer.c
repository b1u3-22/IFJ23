/**
 *  Project:    Implementace překladače imperativního jazyka IFJ23.
 *  File:       @brief Sémantická analýza
 *  Authors:    @author Martin Kučera xkucer0s
*/

#include "analyzer.h"

AnalyzerPtr analyzer_init(SymTablePtr symtable) {
    AnalyzerPtr analyzer = malloc(sizeof(AnalyzerPtr));
    if (!analyzer) return NULL; // Alocation and check

    //create function stack
    TokenStackPtr functionStack = token_stack_init();

    // Set properties
    analyzer->depth = 0;
    analyzer->symtable = symtable;
    analyzer->functionStack = functionStack;

    return analyzer;
}

int check_declaration(AnalyzerPtr analyzer, TokenStackPtr token_stack) {
    //check for redeclaration
    SymTableItemPtr itemToCompare = symtable_get_item_lower_depth_same_block(analyzer->symtable, token_stack->tokens[1]->data, analyzer->depth, analyzer->block[analyzer->depth]);
    if (itemToCompare) {
        if (itemToCompare->depth == analyzer->depth && itemToCompare->block == analyzer->block[analyzer->depth]) return 3;
    }
    
    //if data type is set and its not ?, than its error?
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

    return 0;
}

int check_definition(AnalyzerPtr analyzer, TokenStackPtr token_stack_left, TokenStackPtr token_stack_right) {
    //check for redeclaration
    SymTableItemPtr itemToCompare = symtable_get_item_lower_depth_same_block(analyzer->symtable, token_stack_left->tokens[1]->data, analyzer->depth, analyzer->block[analyzer->depth]);
    if (itemToCompare) {
        if (itemToCompare->depth == analyzer->depth && itemToCompare->block == analyzer->block[analyzer->depth]) return 3;
    }

    if (check_is_not_defined(analyzer, token_stack_right)) return 5;    //is right defined?

    int data_type;
    if (token_stack_left->top->type == TYPE) {
        data_type = token_stack_left->top->value_type;
        if (check_error_7_8(analyzer, data_type, token_stack_right)) return 7;
    } else {
        if (token_stack_right->top->type == ID) {
            SymTableItemPtr item = symtable_get_item_lower_depth_same_block(analyzer->symtable, token_stack_right->top->data, analyzer->depth, analyzer->block[analyzer->depth]);
            data_type = item->type;
            if(check_error_7_8(analyzer, data_type, token_stack_right)) return 8;
        }
        else {
            data_type = token_stack_right->top->value_type;
            if(check_error_7_8(analyzer, data_type, token_stack_right)) return 8;
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
    newItem->isDefined = true; //what if var x int? = nill
    newItem->type = data_type;

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
    SymTableItemPtr itemToAssign = symtable_get_item_lower_depth_same_block(analyzer->symtable, token_stack_left->top->data, analyzer->depth, analyzer->block[analyzer->depth]);
    if (itemToAssign == NULL) return 5;    //is left declared?
    if (!(itemToAssign->isVar) && itemToAssign->isDefined) return 3;    //is left defined let?

    if(check_is_not_defined(analyzer, token_stack_right)) return 5; //is right defined?
    
    int data_type = itemToAssign->type;
    if (data_type != S_NO_TYPE) {
        if (check_error_7_8(analyzer, data_type, token_stack_right)) return 7;
    } else {
        if (token_stack_right->top->type == ID) {
            SymTableItemPtr item = symtable_get_item_lower_depth_same_block(analyzer->symtable, token_stack_right->top->data, analyzer->depth, analyzer->block[analyzer->depth]);
            data_type = item->type;
            if(check_error_7_8(analyzer, data_type, token_stack_right)) return 8;
            itemToAssign->type = data_type;
        }
        else {
            data_type = token_stack_right->top->value_type;
            if(check_error_7_8(analyzer, data_type, token_stack_right)) return 8;
            itemToAssign->type = data_type;
        }
    }

    if(!(itemToAssign->isDefined))  itemToAssign->isDefined = true;
    
    if(token_stack_right->tokens_pos == 0 && token_stack_right->top->type == VALUE) {
        itemToAssign->value = token_stack_right->top->data;
    } else {
        itemToAssign->value = NULL;
    }

    return 0;
}

int check_function_assingment(AnalyzerPtr analyzer, TokenStackPtr token_stack_left, TokenStackPtr token_stack_function) {
    SymTableItemPtr itemToAssign = symtable_get_item_lower_depth_same_block(analyzer->symtable, token_stack_left->top->data, analyzer->depth, analyzer->block[analyzer->depth]);
    if (itemToAssign == NULL) return 5;    //is left declared?
    if (!(itemToAssign->isVar) && itemToAssign->isDefined) return 3;    //is left defined let?
    
    SymTableItemPtr functionItem = symtable_get_function_item(analyzer->symtable, token_stack_function->tokens[0]->data);
    //if function is not defined, push to stack for later check
    if (functionItem == NULL) {
        token_stack_push(analyzer->functionStack, token_stack_function->tokens[0]);
        return 0;
    }
    
    //semantic checks for function
    check_function_call(analyzer, token_stack_function, true);

    //check return type
    if (functionItem->type == S_NO_TYPE) return 4;

    return 0;
}

int check_function_call(AnalyzerPtr analyzer, TokenStackPtr token_stack_function, bool calledAsAssignment) {
    SymTableItemPtr functionId = symtable_get_function_item(analyzer->symtable, token_stack_function->tokens[0]->data);
    //if function is not defined, push to stack for later check
    if (functionId == NULL) {
        token_stack_push(analyzer->functionStack, token_stack_function->tokens[0]);
        return 0;
    }

    //Check if parameter count is matching (by count of external names and IDs)
    int ids1 = 0;
    int ids2 = 0;

    for(int i = 0; i < functionId->paramStack->data_pos+1; i++) {
        if (functionId->paramStack->data[i]->externalName == '_') ids1++;
        else ids1 = ids1 + 2;
    }

    for(int i = 1; i < token_stack_function->tokens_pos+1; i++) {
        if(token_stack_function->tokens[i]->type = ID) ids2++;
    }

    if (ids1 != ids2) return 4;

    //compare external names and parameters data types
    int j = 1; //token_stack_function->tokens index
    for(int i = 0; i < functionId->paramStack->data_pos+1; i++) {
        //check external name
        if (functionId->paramStack->data[i]->externalName != "_") {
            if (functionId->paramStack->data[i]->externalName != token_stack_function->tokens[j]->data) return 4;
            else j++;
        }

        //check data type
        if (functionId->paramStack->data[i]->valueType != token_stack_function->tokens[j]->value_type) return 4;
        else j++;
    }

    //check return types
    if (!calledAsAssignment) { //function must be void
        if (functionId->type != S_NO_TYPE) return 4;
    }

    return 0;
}

int check_function_definition(AnalyzerPtr analyzer, TokenStackPtr token_stack_id, TokenStackPtr token_stack_param) {
    //check if redefining function
    SymTableItemPtr functionId = symtable_get_function_item(analyzer->symtable, token_stack_id->tokens[0]->data);
    if (functionId) return 3;

    //create parameter stack and check for parameters with same external name or id
    ParamStackPtr stack = param_stack_init();
    for(int i = 0; i < (token_stack_param->tokens_pos+1) / 3; i++) {
        ParamStackItemPtr item = param_stack_item_init();
        if (symtable_find_parameter_external_name(stack, token_stack_param->tokens[3*i]->data)) return 3;
        if (symtable_find_parameter_id(stack, token_stack_param->tokens[3*i+1]->data)) return 3;
        item->externalName = token_stack_param->tokens[3*i]->data;
        item->id = token_stack_param->tokens[3*i+1]->data;
        item->valueType = token_stack_param->tokens[3*i+2]->value_type;
        
        param_stack_push(stack, item);
    }
    
    //everything is okay, create new item
    SymTableItemPtr newItem = symtable_item_init();
    newItem->id = token_stack_id->tokens[0]->data;
    newItem->depth = analyzer->depth;
    newItem->block = analyzer->block[analyzer->depth];
    newItem->isDefined = true;
    newItem->isFunction = true;
    newItem->isVar = NULL;
    newItem->value = NULL;
    newItem->paramStack = stack;

    if (token_stack_id->top->type == TYPE) {
        newItem->type = token_stack_id->top->value_type;
    } else {
        newItem->type = S_NO_TYPE; //void function
    }

    symtable_add_item(analyzer->symtable, newItem);

    return 0;
}

void increase_depth(AnalyzerPtr analyzer) {
    analyzer->depth++;
    analyzer->block[analyzer->depth]++;
}

void decrease_depth(AnalyzerPtr analyzer) {
    analyzer->depth--;
}

int check_is_not_defined(AnalyzerPtr analyzer, TokenStackPtr token_stack) {
    for (int i = 0; i < token_stack->tokens_pos+1; i++) {

        if (token_stack->tokens[i]->type == ID) {

            SymTableItemPtr item = symtable_get_item_lower_depth_same_block(analyzer->symtable, token_stack->tokens[i]->data, analyzer->depth, analyzer->block[analyzer->depth]);
            if (item == NULL || item->isDefined == false) {
                return 1;
            }
        }
    }

    return 0;
}

int check_error_7_8(AnalyzerPtr analyzer, int data_type, TokenStackPtr token_stack) {
    for (int i = 0; i < token_stack->tokens_pos+1; i++) {

        if (token_stack->tokens[i]->type == ID) {

            SymTableItemPtr item = symtable_get_item_lower_depth_same_block(analyzer->symtable, token_stack->tokens[i]->data, analyzer->depth, analyzer->block[analyzer->depth]);
            if (data_type == S_INTQ && (item->type == S_INT || item->type == S_INTQ)) {
                return 0;
            }

            else if (data_type == S_DOUBLEQ && (item->type == S_DOUBLE || item->type == S_DOUBLEQ)) {
                return 0;
            }

            else if (data_type == S_STRINGQ && (item->type == S_STRING || item->type == S_STRINGQ)) {
                return 0;
            }
            
            else if (item->type != data_type) {
                return 1;
            }

        } else {
            if (data_type == S_INTQ && token_stack->tokens[i]->value_type == S_INT) {
                return 0;
            }

            else if (data_type == S_DOUBLEQ && token_stack->tokens[i]->value_type == S_DOUBLE) {
                return 0;
            }

            else if (data_type == S_STRINGQ && token_stack->tokens[i]->value_type == S_STRING) {
                return 0;
            }

            if (token_stack->tokens[i]->value_type != data_type) {
                return 1;
            }
        }
    }

    return 0;
}

SymTableItemPtr get_nearest_item(AnalyzerPtr analyzer, char* id) {
    return symtable_get_item_lower_depth_same_block(analyzer->symtable, id, analyzer->depth, analyzer->block[analyzer->depth]);
}

int get_current_depth(AnalyzerPtr analyzer) {
    return analyzer->depth;
}

int get_current_block(AnalyzerPtr analyzer) {
    return analyzer->block;
}

int check_undefined_functions(AnalyzerPtr analyzer) {
    for (int i = 0; i < analyzer->functionStack->tokens_pos+1; i++) {
        if (!symtable_get_function_item(analyzer->symtable, analyzer->functionStack->tokens[i]->data)) return 3;
    }
}