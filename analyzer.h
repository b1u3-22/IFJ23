#include <stdlib.h>
#include <stdio.h>

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
} *AnalyzerPtr;

AnalyzerPtr analyzer_init(SymTablePtr symtable);

int check_declaration(AnalyzerPtr analyzer, TokenStackPtr token_stack);

int check_definition(AnalyzerPtr analyzer, TokenStackPtr token_stack_left, TokenStackPtr token_stack_right);

int check_value_assingment(AnalyzerPtr analyzer, TokenStackPtr token_stack_left, TokenStackPtr token_stack_right);

int check_function_assingment(AnalyzerPtr analyzer, TokenStackPtr token_stack_left, TokenStackPtr token_stack_param);

int check_function_call(AnalyzerPtr analyzer, TokenStackPtr token_stack_left, TokenStackPtr token_stack_param);

int check_function_definition(AnalyzerPtr analyzer, TokenStackPtr token_stack_param);

void increase_depth(AnalyzerPtr analyzer);

void decrease_depth(AnalyzerPtr analyzer);

int check_error_5(AnalyzerPtr analyzer);

int check_error_7_8(AnalyzerPtr analyzer, int data_type, TokenStackPtr token_stack);