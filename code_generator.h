

 // CODE GENERATOR HEADER //



#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H


#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "symtable.h"


#define inst(...) \
    do {const char *instruction[] = {__VA_ARGS__}; \
        size_t inst_len = sizeof(instruction) / sizeof(const char *); \
        for (int i = 0; i < (int)inst_len; i++) { \
            fputs(instruction[i], stdout); \
        } \
        fputs("\n", stdout); \
    } while (0)
	    
#define part(...) \
    do {const char *inst_part[] = {__VA_ARGS__}; \
        size_t part_len = sizeof(inst_part) / sizeof(const char *); \
        for (int i = 0; i < (int)part_len; i++) { \
            fputs(inst_part[i], stdout); \
        } \
    } while (0)


static unsigned int num;
static unsigned int if_num = 0;
static unsigned int if_new = 1;
static unsigned int while_num = 0;
static unsigned int while_new = 1;


///// FUNCTIONS /////

// beginning of file (header and definition of builtin function)
void code_header();

// ending of file
void code_footer();

// make operation on stack
void exp_instruction(int type);

// define variable
void def_var(SymTableItemPtr var);

// pop value to variable
void set_var(SymTableItemPtr var);

// push symbol to stack for further computing or as final value of whole expression
void push_sym(SymTableItemPtr sym);

// check condition (value on top of stack) and start of if
// condition has to be calculated before this function
void if_check();

// end of if and start of else
void if_end();

// end of else
void if_else_end();

// start of while (called before condition is calculated)
void while_start();

// check condition (value on top of stack) and start of while
// condition has to be calculated before this function
void while_check();

// end of while
void while_end();

// start of function definition
void func_start(char* func);

// define single parameter of function
void func_param(SymTableItemPtr param);

// end of function
void func_end();

// start of function call
void func_call();

// set value to single parameter of function
void func_call_param(SymTableItemPtr param);

// end of function call
void func_call_end(char* func);


///// AUXILIARY FUNCTIONS /////

void auxil_opdecider();
void auxil_divdecider();
void auxil_qqdecider();


///// BUILTIN FUNCTIONS /////

void builtin_read();
void builtin_write();
void builtin_int2float();
void builtin_float2int();
void builtin_len();
void builtin_substr();
void builtin_ord();
void builtin_chr();
