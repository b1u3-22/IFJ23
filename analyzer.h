/**
 *  Project:    Implementace překladače imperativního jazyka IFJ23.
 *  File:       @brief Sémantická analýza
 *  Authors:    @author Martin Kučera xkucer0s
*/

#include <stdlib.h>
#include <stdio.h>

#ifndef _PARAM_STACK
#define _PARAM_STACK
#include "param_stack.h"
#endif

#ifndef _TOKEN_STACK
#define _TOKEN_STACK
#include "token_stack.h"
#endif

#ifndef _SYMTABLE
#define _SYMTABLE
#include "symtable.h"
#endif

#ifndef _TOKEN
#define _TOKEN
#include "token.h"
#endif

typedef struct Analyzer {
    int depth;
    int block[1000];
    SymTablePtr symtable;
    TokenStackPtr functionStack;
} *AnalyzerPtr;

/** Initialize analyzer
    @returns AnalyzerPtr if succeded NULL otherwise
*/
AnalyzerPtr analyzer_init(SymTablePtr symtable);

/** Semantic checks for declaration
 *  @param analyzer AnalyzerPtr to work with
 *  @param token_stack TokenStackPtr containing let or var, id, data type
 *  @returns int error code if semantic errors 0 otherwise
*/
int check_declaration(AnalyzerPtr analyzer, TokenStackPtr token_stack);

/** Semantic checks for definition
 *  @param analyzer AnalyzerPtr to work with
 *  @param token_stack_left TokenStackPtr containing let or var, id, data type
 *  @param token_stack_right TokenStackPtr containing expression to assign
 *  @returns int error code if semantic errors 0 otherwise
*/
int check_definition(AnalyzerPtr analyzer, TokenStackPtr token_stack_left, TokenStackPtr token_stack_right);

/** Semantic checks for expression assignment to a declared or defined variable
 *  @param analyzer AnalyzerPtr to work with
 *  @param token_stack_left TokenStackPtr containing variable to assign
 *  @param token_stack_right TokenStackPtr containing expression to assign
 *  @returns int error code if semantic errors 0 otherwise
*/
int check_value_assingment(AnalyzerPtr analyzer, TokenStackPtr token_stack_left, TokenStackPtr token_stack_right);

/** Semantic checks for function assignment to a declared or defined variable
 *  @param analyzer AnalyzerPtr to work with
 *  @param token_stack_left TokenStackPtr containing variable to assign
 *  @param token_stack_param TokenStackPtr containing function to assign
 *  @returns int error code if semantic errors 0 otherwise
*/
int check_function_assingment(AnalyzerPtr analyzer, TokenStackPtr token_stack_left, TokenStackPtr token_stack_param);

/** Semantic checks for function call
 *  @param analyzer AnalyzerPtr to work with
 *  @param token_stack_left TokenStackPtr containing function name
 *  @param token_stack_param TokenStackPtr containing function parameters
 *  @returns int error code if semantic errors 0 otherwise
*/
int check_function_call(AnalyzerPtr analyzer, TokenStackPtr token_stack_function, bool calledAsAssignment);

/** Semantic checks for function definition
 *  @param analyzer AnalyzerPtr to work with
 *  @param token_stack_id TokenStackPtr containing function name
 *  @param token_stack_param TokenStackPtr containing function parameters
 *  @returns int error code if semantic errors 0 otherwise
*/
int check_function_definition(AnalyzerPtr analyzer, TokenStackPtr token_stack_id, TokenStackPtr token_stack_param);

/** Increases analyzer depth and block attribute
 *  @param analyzer AnalyzerPtr to work with
*/
void increase_depth(AnalyzerPtr analyzer);

/** Decreases analyzer depth attribute
 *  @param analyzer AnalyzerPtr to work with
*/
void decrease_depth(AnalyzerPtr analyzer);

/** Checks if variables are defined
 *  @param analyzer AnalyzerPtr to work with
 *  @param token_stack TokenStackPtr containing variables
 *  @returns int 1 if any variable is not defined 0 otherwise
*/
int check_is_not_defined(AnalyzerPtr analyzer, TokenStackPtr token_stack);

/** Checks if variables have compatible data types
 *  @param analyzer AnalyzerPtr to work with
 *  @param data_type int defining data type
 *  @param token_stack TokenStackPtr containing variables
 *  @returns int 1 if any variable has not compatible data type 0 otherwise
*/
int check_error_7_8(AnalyzerPtr analyzer, int data_type, TokenStackPtr token_stack);

/** Finds closest item to current depth and block
 *  @param analyzer AnalyzerPtr to work with
 *  @param id char* id to find
 *  @returns SymTableItemPtr closest to current depth and block
*/
SymTableItemPtr get_nearest_item(AnalyzerPtr analyzer, char* id);

/** Returns current depth
 *  @param analyzer AnalyzerPtr to work with
 *  @returns int of current depth
*/
int get_current_depth(AnalyzerPtr analyzer);

/** Returns current block
 *  @param analyzer AnalyzerPtr to work with
 *  @returns int of current block
*/
int get_current_block(AnalyzerPtr analyzer);

/** Checks all undefined functions at the end of parsing
 *  @param analyzer AnalyzerPtr to work with
 *  @returns int error code if semantic errors 0 otherwise
*/
int check_undefined_functions(AnalyzerPtr analyzer);