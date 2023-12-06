/**
 *  Project:    Implementace překladače imperativního jazyka IFJ23.
 *  File:       @brief Hlavičkový soubor pro sémantickou analýzu
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

#ifndef _GEN_STACK
#define _GEN_STACK
#include "gen_stack.h"
#endif

#define ANALYZER_ALLOCK_BLOCK 3

typedef struct Analyzer {
    int depth;
    int *block;
    int block_allocd;
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
 *  @param token_stack_function TokenStackPtr containing function name and it's parameters
 *  @returns int error code if semantic errors 0 otherwise
*/
int check_function_assingment(AnalyzerPtr analyzer, TokenStackPtr token_stack_left, TokenStackPtr token_stack_function);

/** Semantic checks for function call
 *  @param analyzer AnalyzerPtr to work with
 *  @param token_stack_function TokenStackPtr containing function name and it's parameters
 *  @returns int error code if semantic errors 0 otherwise
*/
int check_function_call(AnalyzerPtr analyzer, TokenStackPtr token_stack_function);

/** Semantic checks for function definition
 *  @param analyzer AnalyzerPtr to work with
 *  @param token_stack_id TokenStackPtr containing function name
 *  @param token_stack_param TokenStackPtr containing function parameters
 *  @returns int error code if semantic errors 0 otherwise
*/
int check_function_definition(AnalyzerPtr analyzer, TokenStackPtr token_stack_id, TokenStackPtr token_stack_param);

/** Checks if return was used in main body
 * @param analyzer AnalyzerPtr to work with
 * @param token_stack TokenStackPtr containing return value
 * @returns int error code if semantic errors 0 otherwise
*/
int check_return(AnalyzerPtr analyzer, TokenStackPtr token_stack);

/** Semantic checks for conditions
 * @param analyzer AnalyzerPtr to work with
 * @param token_stack TokenStackPtr containing tokens used in condition
 * @returns int error code if semantic errors 0 otherwise
*/
int check_condition(AnalyzerPtr analyzer, TokenStackPtr token_stack);

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

/** Checks if data types are compatible
 *  @param analyzer AnalyzerPtr to work with
 *  @param token TokenPtr token which is assigned
 *  @param data_type int data type of variable to be assigned
 *  @returns int error code if semantic errors 0 otherwise
*/
int check_data_type(AnalyzerPtr analyzer, TokenPtr token, int data_type);

/** Finds closest item to current depth and block
 *  @param analyzer AnalyzerPtr to work with
 *  @param id char* id to find
 *  @returns SymTableItemPtr closest to current depth and block
*/
SymTableItemPtr get_nearest_item(AnalyzerPtr analyzer, char* id);

/** Checks all undefined functions at the end of parsing
 *  @param analyzer AnalyzerPtr to work with
 *  @returns int error code if semantic errors 0 otherwise
*/
int check_undefined_functions(AnalyzerPtr analyzer);

/** Checks if item isn't redefined in the same block
 *  @param analyzer AnalyzerPtr to work with
 *  @param item SymTableItemPtr item we are checking
 *  @returns int error code if semantic errors, 1 if redefinition check should be skipped, 0 otherwise
*/
int check_redefinition(AnalyzerPtr analyzer, SymTableItemPtr item);

/** Creates new item in symtable
 *  @param analyzer AnalyzerPtr to work with
 *  @param let_var_token TokenPtr containing let or var
 *  @param id_token TokenPtr containing variable id
 *  @param data_type_token TokenPtr containing variable data type
 *  @returns SymTableItemPtr of created item
*/
SymTableItemPtr create_new_item(AnalyzerPtr analyzer, TokenPtr let_var_token, TokenPtr id_token, TokenPtr data_type_token);

/** If left side variable is double, converts data type of integer literals to double
 *  @param analyzer AnalyzerPtr to work with
 *  @param token_stack_left TokenStackPtr containing left tokens
 *  @param token_stack_right TokenStackPtr containing right tokens
*/
void check_typedef(AnalyzerPtr analyzer, TokenStackPtr token_stack_left, GenStackPtr token_stack_right);