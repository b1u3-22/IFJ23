/**
 *  Project:    Implementace překladače imperativního jazyka IFJ23.
 *  File:       @brief Sémantická analýza
 *  Authors:    @author Martin Kučera xkucer0s
*/

#include "analyzer.h"

AnalyzerPtr analyzer_init(SymTablePtr symtable) {
    AnalyzerPtr analyzer = malloc(sizeof(struct Analyzer));
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
    newItem->definedAtFuncAssign = false;
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
        if (itemToCompare->definedAtFuncAssign == true) return 0;
        else if (itemToCompare->depth == analyzer->depth && itemToCompare->block == analyzer->block[analyzer->depth]) return 3;
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
    newItem->definedAtFuncAssign = false;
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
    if (!(itemToAssign->isVar) && itemToAssign->isDefined) return 3;   //is left defined let?

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

    if (token_stack_left->tokens[0]->type == VAR || token_stack_left->tokens[0]->type == LET) {
        SymTableItemPtr itemToCompare = symtable_get_item_lower_depth_same_block(analyzer->symtable, token_stack_left->tokens[1]->data, analyzer->depth, analyzer->block[analyzer->depth]);
        if (itemToCompare) {
            if (itemToCompare->depth == analyzer->depth && itemToCompare->block == analyzer->block[analyzer->depth]) return 3;
        }

        SymTableItemPtr newItem = symtable_item_init(); 
 
        newItem->id = token_stack_left->tokens[1]->data; 
        newItem->depth = analyzer->depth; 
        newItem->block = analyzer->block[analyzer->depth]; 
        newItem->isFunction = false; 
        newItem->isDefined = false; 
        newItem->definedAtFuncAssign = true;
        
        if (token_stack_left->top->type == TYPE) { 
             newItem->type = token_stack_left->top->value_type; 
         } else { 
            newItem->type = S_NO_TYPE; 
        } 
    
        if (token_stack_left->tokens[0]->type == VAR) newItem->isVar = true; 
        else newItem->isVar = false; 
    
        symtable_add_item(analyzer->symtable, newItem); 
    }
    
    SymTableItemPtr itemToAssign = symtable_get_item_lower_depth_same_block(analyzer->symtable, token_stack_left->tokens[1]->data, analyzer->depth, analyzer->block[analyzer->depth]);
    if (itemToAssign == NULL) return 5;    //is left declared?
    if (!(itemToAssign->isVar) && itemToAssign->isDefined) return 3;       //is left defined let?
    itemToAssign->isDefined = true; //this is wrong

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
        if      (itemToAssign->type == S_INTQ && (functionItem->type == S_INT || functionItem->type == S_INTQ)) return 0;
        else if (itemToAssign->type == S_STRINGQ && (functionItem->type == S_STRING || functionItem->type == S_STRINGQ)) return 0;
        else if (itemToAssign->type == S_DOUBLEQ && (functionItem->type == S_DOUBLE || functionItem->type == S_DOUBLEQ || functionItem->type == S_INT || functionItem->type == S_INTQ)) return 0;
        else if (itemToAssign->type == S_DOUBLE && (functionItem->type == S_DOUBLE || functionItem->type == S_INT)) return 0;
        else if (itemToAssign->type != functionItem->type) return 4;
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
    if (functionId) return 3;

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
            if (data_type == S_INTQ && (item->type == S_INT || item->type == S_INTQ || item->type == S_NO_TYPE)) {
                return 0;
            }

            else if (data_type == S_DOUBLEQ && (item->type == S_DOUBLE || item->type == S_DOUBLEQ || item->type == S_INT || item->type == S_INTQ || item->type == S_NO_TYPE)) {
                return 0;
            }

            else if (data_type == S_STRINGQ && (item->type == S_STRING || item->type == S_STRINGQ || item->type == S_NO_TYPE)) {
                return 0;
            }

            else if (data_type == S_DOUBLE && (item->type == S_DOUBLE || item->type == S_INT)) {
                return 0;
            }
            
            else if (item->type != data_type) {
                return 1;
            }

        } else {
            if (data_type == S_INTQ && token_stack->tokens[i]->value_type == S_INT || token_stack->tokens[i]->value_type == S_NO_TYPE) {
                return 0;
            }

            else if (data_type == S_DOUBLEQ && (token_stack->tokens[i]->value_type == S_DOUBLE || token_stack->tokens[i]->value_type == S_INT || token_stack->tokens[i]->value_type == S_NO_TYPE)) {
                return 0;
            }

            else if (data_type == S_STRINGQ && token_stack->tokens[i]->value_type == S_STRING || token_stack->tokens[i]->value_type == S_NO_TYPE) {
                return 0;
            }

            else if (data_type == S_DOUBLE && (token_stack->tokens[i]->value_type == S_DOUBLE || token_stack->tokens[i]->value_type == S_INT)) {
                return 0;
            }

            else if (token_stack->tokens[i]->value_type != data_type) {
                return 1;
            }
        }
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