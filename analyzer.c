/**
 *  Project:    Implementace překladače imperativního jazyka IFJ23.
 *  File:       @brief Sémantická analýza
 *  Authors:    @author Martin Kučera xkucer0s
*/

#include "analyzer.h"

AnalyzerPtr analyzer_init(SymTablePtr symtable) {
    AnalyzerPtr analyzer = malloc(sizeof(struct Analyzer));
    if (!analyzer) return NULL; // Alocation and check

    analyzer->block = calloc(ANALYZER_ALLOCK_BLOCK, sizeof(int));
    if (!analyzer->block) {
        free(analyzer);
        return NULL;
    }

    //create function stack
    TokenStackPtr functionStack = token_stack_init();

    // Set properties
    analyzer->depth = 0;
    analyzer->block_allocd = ANALYZER_ALLOCK_BLOCK;
    analyzer->symtable = symtable;
    analyzer->functionStack = functionStack;

    return analyzer;
}

int check_declaration(AnalyzerPtr analyzer, TokenStackPtr token_stack) {
    //check for redeclaration
    SymTableItemPtr itemToCompare = symtable_get_item_lower_depth_same_block(analyzer->symtable, token_stack->tokens[1]->data, analyzer->depth, analyzer->block[analyzer->depth]);
    if (check_redefinition(analyzer, itemToCompare)) return 3;
    
    //everything is okay, create new item
    TokenPtr let_var_token   = token_stack->tokens[0];
    TokenPtr id_token        = token_stack->tokens[1];
    TokenPtr data_type_token = token_stack->tokens[2];

    SymTableItemPtr newItem = create_new_variable_item(analyzer, let_var_token, id_token, data_type_token);

    if (newItem->type == S_INTQ || newItem->type == S_DOUBLEQ || newItem->type == S_STRINGQ) newItem->isNil == true;

    symtable_add_item(analyzer->symtable, newItem);

    return 0;
}

int check_definition(AnalyzerPtr analyzer, TokenStackPtr token_stack_left, TokenStackPtr token_stack_right) {
    //check for redeclaration
    SymTableItemPtr itemToCompare = symtable_get_item_lower_depth_same_block(analyzer->symtable, token_stack_left->tokens[1]->data, analyzer->depth, analyzer->block[analyzer->depth]);
    int redefinition = check_redefinition(analyzer, itemToCompare);
    if (redefinition == 1) return 0;
    else if (redefinition != 0) return 3;

    //check if all variables on right side are defined
    if (check_is_not_defined(analyzer, token_stack_right)) return 5;

    //set data_type
    int data_type;
    if (token_stack_left->top->type == TYPE) {
        data_type = token_stack_left->top->value_type;
    }
    else {
        if (token_stack_right->top->type == ID) {
            SymTableItemPtr item = symtable_get_item_lower_depth_same_block(analyzer->symtable, token_stack_right->top->data, analyzer->depth, analyzer->block[analyzer->depth]);
            data_type = item->type;
        }
        else {
            data_type = token_stack_right->top->value_type;
        }
    }

    //check if data_type matches left side
    for (int i = 0; i < token_stack_right->tokens_pos+1; i++) {
        if (check_data_type(analyzer, token_stack_right->tokens[i], data_type)) return 7;
    }

    if (data_type == S_NO_TYPE) return 8;

    //everything is okay, create new item
    TokenPtr let_var_token = token_stack_left->tokens[0];
    TokenPtr id_token = token_stack_left->tokens[1];

    SymTableItemPtr newItem = create_new_variable_item(analyzer, let_var_token, id_token, NULL);

    newItem->isDefined = true;
    newItem->type = data_type;

    if(token_stack_right->tokens_pos == 0 && token_stack_right->top->type == VALUE) {
        if (token_stack_right->top->value_type != S_NO_TYPE) newItem->value = token_stack_right->top->data;
        else newItem->isNil = true;
    }

    symtable_add_item(analyzer->symtable, newItem);

    return 0;
}

int check_value_assingment(AnalyzerPtr analyzer, TokenStackPtr token_stack_left, TokenStackPtr token_stack_right) {
    SymTableItemPtr itemToAssign = symtable_get_item_lower_depth_same_block(analyzer->symtable, token_stack_left->top->data, analyzer->depth, analyzer->block[analyzer->depth]);
    if (itemToAssign == NULL) return 5;    //is left declared?
    if (!(itemToAssign->isVar) && itemToAssign->isDefined) return 3;   //is left defined let?

    if(check_is_not_defined(analyzer, token_stack_right)) return 5; //is right defined?
    
    //check if data types on the right side match data type on the left side
    for (int i = 0; i < token_stack_right->tokens_pos+1; i++) {
        if (check_data_type(analyzer, token_stack_right->tokens[i], itemToAssign->type)) return 7;
    }

    if(!(itemToAssign->isDefined))  itemToAssign->isDefined = true;

    if(token_stack_right->tokens_pos == 0 && token_stack_right->top->type == VALUE) {
        if (token_stack_right->top->value_type != S_NO_TYPE) itemToAssign->value = token_stack_right->top->data;
        else itemToAssign->isNil = true;
    }

    return 0;
}

int check_function_assingment(AnalyzerPtr analyzer, TokenStackPtr token_stack_left, TokenStackPtr token_stack_function) {

    //for let x : Int = func(), we must first add x to symtable
    if (token_stack_left->tokens[0]->type == VAR || token_stack_left->tokens[0]->type == LET) {
        
        //check for redefinition
        SymTableItemPtr itemToCompare = symtable_get_item_lower_depth_same_block(analyzer->symtable, token_stack_left->tokens[1]->data, analyzer->depth, analyzer->block[analyzer->depth]);
        if (check_redefinition(analyzer, itemToCompare)) return 3;

        //create new item
        TokenPtr let_var_token = token_stack_left->tokens[0];
        TokenPtr id_token = token_stack_left->tokens[1];
        SymTableItemPtr newItem = create_new_variable_item(analyzer, let_var_token, id_token, NULL);

        newItem->definedAtFuncAssign = true;

        symtable_add_item(analyzer->symtable, newItem); 
    }
    
    SymTableItemPtr itemToAssign = symtable_get_item_lower_depth_same_block(analyzer->symtable, token_stack_left->tokens[1]->data, analyzer->depth, analyzer->block[analyzer->depth]);
    if (itemToAssign == NULL) return 5;    //is left declared?
    if (!(itemToAssign->isVar) && itemToAssign->isDefined) return 3;       //is left defined let?
    itemToAssign->isDefined = true;

    SymTableItemPtr functionItem = symtable_get_function_item(analyzer->symtable, token_stack_function->tokens[0]->data);
    //if function is not defined, push to stack for later check
    if (functionItem == NULL) {
        token_stack_push(analyzer->functionStack, token_stack_function->tokens[0]);
        return 0;
    }
    
    //check return type
    if (functionItem->type == S_NO_TYPE) return 4;

    if (itemToAssign->type == S_NO_TYPE) {
        itemToAssign->type = functionItem->type;
    }
    else {
        check_data_type(analyzer, functionItem, itemToAssign->type);
    }

    //semantic checks for function
    int error = check_function_call(analyzer, token_stack_function, true);
    if (error)
    {
        return error;
    }

    return 0;
}

int check_function_call(AnalyzerPtr analyzer, TokenStackPtr token_stack_function, bool calledAsAssignment) {
    
    SymTableItemPtr functionId = symtable_get_function_item(analyzer->symtable, token_stack_function->tokens[0]->data);
    //if function is not defined, push to stack for later check
    if (functionId == NULL) {
        token_stack_push(analyzer->functionStack, token_stack_function->tokens[0]);
        return 0;
    }

    //write function doesn't have set number of parameters
    //just check, if parameters are defined
    if (!strcmp(functionId->id, "write")) {

        for (int i = 1; i < token_stack_function->tokens_pos+1; i++) {
            if(token_stack_function->tokens[i]->type != VALUE) {
                SymTableItemPtr itemToCheck = symtable_get_item_lower_depth_same_block(analyzer->symtable, token_stack_function->tokens[i]->data, analyzer->depth, analyzer->block[analyzer->depth]);
                if (itemToCheck == NULL || itemToCheck->isDefined == false) return 5;
            }
        }

        return 0;
    }    

    //Check if parameter count is matching (by count of external names and IDs)
    int numOfExpectedTokens = 0;

    for(int i = 0; i < functionId->paramStack->data_pos+1; i++) {
        if (!strcmp(functionId->paramStack->data[i]->externalName, "_")) numOfExpectedTokens++;
        else numOfExpectedTokens = numOfExpectedTokens + 2;
    }

    if (numOfExpectedTokens != token_stack_function->tokens_pos) return 4;
    
    //compare external names and parameters data types
    int j = 1; //token_stack_function->tokens index

    for(int i = 1; i < functionId->paramStack->data_pos+1; i++) {        
        //check external name
        if (strcmp(functionId->paramStack->data[i]->externalName, "_")) {
            if (strcmp(functionId->paramStack->data[i]->externalName, token_stack_function->tokens[j]->data)) return 4;
            else j++;
        }

        //check if parameter is defined
        if (token_stack_function->tokens[j]->type != VALUE) {
            SymTableItemPtr itemToCheck = symtable_get_item_lower_depth_same_block(analyzer->symtable, token_stack_function->tokens[j]->data, analyzer->depth, analyzer->block[analyzer->depth]);
            if (itemToCheck == NULL || itemToCheck->isDefined == false) return 5;

            //check data type
            if (functionId->paramStack->data[i]->valueType != itemToCheck->type) return 4;
            else j++;
        } else {
            //check data type
            if (functionId->paramStack->data[i]->valueType == S_DOUBLE && (token_stack_function->tokens[j]->value_type == S_DOUBLE || token_stack_function->tokens[j]->value_type == S_INT)) {
                j++;
                continue;
            }
            else if (functionId->paramStack->data[i]->valueType != token_stack_function->tokens[j]->value_type) return 4;
            else j++;
        }
    }

    return 0;
}

int check_function_definition(AnalyzerPtr analyzer, TokenStackPtr token_stack_id, TokenStackPtr token_stack_param) {
    //check if redefining function
    SymTableItemPtr functionId = symtable_get_function_item(analyzer->symtable, token_stack_id->tokens[0]->data);
    if (check_redefinition(analyzer, functionId)) return 3;

    //create parameter stack and check for parameters with same external name or id
    ParamStackPtr stack = param_stack_init();
    for(int i = 0; i < (token_stack_param->tokens_pos+1) / 3; i++) {
        if (strcmp(token_stack_param->tokens[3*i]->data, "_")) {
            //external name and id cannot be the same
            if (!strcmp(token_stack_param->tokens[3*i]->data, token_stack_param->tokens[3*i+1]->data)) return 9;

            //cannot use same external name twice
            if (symtable_find_parameter_external_name(stack, token_stack_param->tokens[3*i]->data)) return 3;
        }
        
        //cannot use same id twice
        if (symtable_find_parameter_id(stack, token_stack_param->tokens[3*i+1]->data)) return 3;

        ParamStackItemPtr item = param_stack_item_init();
        item->externalName = token_stack_param->tokens[3*i]->data;
        item->id = token_stack_param->tokens[3*i+1]->data;
        item->valueType = token_stack_param->tokens[3*i+2]->value_type;

        //also add parameter to symtable
        SymTableItemPtr symtableItem = symtable_item_init();
        symtableItem->depth = analyzer->depth;
        symtableItem->block = analyzer->block[analyzer->depth];
        symtableItem->id = token_stack_param->tokens[3*i+1]->data;
        symtableItem->isDefined = true;
        symtableItem->isFunction = false;
        symtableItem->isVar = true;
        symtableItem->type = token_stack_param->tokens[3*i+2]->value_type;

        symtable_add_item(analyzer->symtable, symtableItem);
        
        param_stack_push(stack, item);
    }
    
    //everything is okay, create new item
    TokenPtr id_token = token_stack_id->tokens[0];
    
    SymTableItemPtr newItem = create_new_variable_item(analyzer, NULL, id_token, NULL);

    newItem->isFunction = true;
    newItem->paramStack = stack;

    if (token_stack_id->top->type == TYPE) {
        newItem->type = token_stack_id->top->value_type;
    } else {
        newItem->type = S_NO_TYPE; //void function
    }

    symtable_add_item(analyzer->symtable, newItem);

    return 0;
}

int check_return(AnalyzerPtr analyzer, TokenStackPtr token_stack) {
    SymTableItemPtr currentFunctionItem;
    if (token_stack->empty) {
        if (currentFunctionItem->type != S_NO_TYPE) return 6;
        else return 0;
    } else {
        if (currentFunctionItem->type == S_INTQ && (token_stack->top->value_type == S_INTQ || token_stack->top->value_type == S_INT || token_stack->top->value_type == S_NO_TYPE)) return 0;
        else if (currentFunctionItem->type == S_DOUBLEQ && (token_stack->top->value_type == S_DOUBLEQ || token_stack->top->value_type == S_DOUBLE || token_stack->top->value_type == S_NO_TYPE || token_stack->top->value_type == S_INTQ || token_stack->top->value_type == S_INT)) return 0;
        else if (currentFunctionItem->type == S_STRINGQ && (token_stack->top->value_type == S_STRINGQ || token_stack->top->value_type == S_STRING || token_stack->top->value_type == S_NO_TYPE)) return 0;
        else if (currentFunctionItem->type == S_DOUBLE && (token_stack->top->value_type == S_DOUBLE || token_stack->top->value_type == S_INT)) return 0;
        else if (currentFunctionItem->type != token_stack->top->value_type) return 6;
    }

    return 0;
}

void increase_depth(AnalyzerPtr analyzer) {
    analyzer->depth++;

    // Allocate more memory for block if we hit the allocd limit
    if (analyzer->depth >= analyzer->block_allocd) {
        int *temp_block = realloc(analyzer->block, (analyzer->block_allocd + ANALYZER_ALLOCK_BLOCK) * sizeof(int));
        if (!temp_block) return; // Reallocation failed
        analyzer->block = temp_block;
        analyzer->block_allocd += ANALYZER_ALLOCK_BLOCK;

        // Initialize the memory to 0
        for (int i = analyzer->depth; i < analyzer->block_allocd; i++) {
            analyzer->block[i] = 0;
        }
    }

    analyzer->block[analyzer->depth]++;
}

void decrease_depth(AnalyzerPtr analyzer) {
    analyzer->depth--;
}

SymTableItemPtr create_new_variable_item(AnalyzerPtr analyzer, TokenPtr let_var_token, TokenPtr id_token, TokenPtr data_type_token) {
    SymTableItemPtr newItem = symtable_item_init();
    newItem->id = id_token->data;
    newItem->depth = analyzer->depth;
    newItem->block = analyzer->block[analyzer->depth];
    newItem->isFunction = false;
    newItem->isDefined = false;
    newItem->definedAtFuncAssign = false;
    newItem->value = NULL;

    if (data_type_token != NULL) {
        newItem->type = data_type_token->value_type;
    } else {
        newItem->type = S_NO_TYPE;
    }

    if (let_var_token != NULL) {
        if (let_var_token->type == VAR) newItem->isVar = true;
        else newItem->isVar = false;
    }

    return newItem;
}

int check_redefinition(AnalyzerPtr analyzer, SymTableItemPtr item) {
    if (item) {
        if (item->definedAtFuncAssign == true) { //only for check_definition function
            item->definedAtFuncAssign = false;
            return 1;
        }
        else if (item->isFunction == true) return 3;
        else if (item->depth == analyzer->depth && item->block == analyzer->block[analyzer->depth]) return 3;
    }

    return 0;
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

int check_data_type(AnalyzerPtr analyzer, TokenPtr token, int data_type) {
    if (token->type == ID) {
        SymTableItemPtr item = symtable_get_item_lower_depth_same_block(analyzer->symtable, token->data, analyzer->depth, analyzer->block[analyzer->depth]);
        if (data_type == S_INTQ && (item->type == S_INTQ || item->type == S_INT || item->type == S_NO_TYPE)) return 0;
        else if (data_type == S_DOUBLEQ && (item->type == S_DOUBLEQ || item->type == S_DOUBLE || item->type == S_NO_TYPE)) return 0;
        else if (data_type == S_STRINGQ && (item->type == S_STRINGQ || item->type == S_STRING || item->type == S_NO_TYPE)) return 0;
        else if (data_type != item->type) return 7;
    } else {
        if (data_type == S_INTQ && (token->value_type == S_INT || token->value_type == S_NO_TYPE)) return 0;
        else if (data_type == S_DOUBLEQ && (token->value_type == S_DOUBLE || token->value_type == S_INT || token->value_type == S_NO_TYPE)) return 0;
        else if (data_type == S_STRINGQ && (token->value_type == S_STRING || token->value_type == S_NO_TYPE)) return 0;
        else if (data_type == S_DOUBLE && (token->value_type == S_INT || token->value_type == S_DOUBLE)) return 0;
        else if (data_type != token->value_type) return 7;
    }

    return 0;
}

SymTableItemPtr get_nearest_item(AnalyzerPtr analyzer, char* id) {
    return symtable_get_item_lower_depth_same_block(analyzer->symtable, id, analyzer->depth, analyzer->block[analyzer->depth]);
}

int check_undefined_functions(AnalyzerPtr analyzer) {
    for (int i = 0; i < analyzer->functionStack->tokens_pos+1; i++) {
        SymTableItemPtr item = symtable_get_function_item(analyzer->symtable, analyzer->functionStack->tokens[i]->data);
        if (item == NULL) return 3;
    }

    return 0;
}