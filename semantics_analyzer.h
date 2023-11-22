#include <stdlib.h>
#include <stdio.h>

#ifndef _TOKEN_STACK
#define _TOKEN_STACK
#include "token_stack.h"
#endif

typedef struct Analyzer {
    int depth;
} *AnalyzerPtr;

AnalyzerPtr init_analyzer();

int check_declaration(TokenStackPtr token_stack);

int check_definition(TokenStackPtr token_stack_left, TokenStackPtr token_stack_right);

int check_value_assingment(TokenStackPtr token_stack_left, TokenStackPtr token_stack_right);

int check_function_assingment(TokenStackPtr token_stack_left, TokenStackPtr token_stack_right, TokenStackPtr token_stack_param);

int check_function_call(TokenStackPtr token_stack_left, TokenStackPtr token_stack_param);

int check_function_definition(TokenStackPtr token_stack_param);