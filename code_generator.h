

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
static unsigned int qq_num = 0;
static unsigned int depth = 0;
static unsigned int if_num = 0;
static unsigned int if_new = 1;
static unsigned int while_num = 0;
static unsigned int while_new = 1;


// FUNCTIONS

void code_header();
void code_footer();
void exp_instruction(int type);
void def_var(SymTableItemPtr var);
void set_var(SymTableItemPtr var, SymTableItemPtr sym);
void push_sym(SymTableItemPtr sym);
void if_check();
void if_end();
void if_else_end();
void while_start();
void while_check();
void while_end();
void func_start(char* func);
void func_param(SymTableItemPtr param);
void func_return(SymTableItemPtr var);
void func_end(char* func);
void func_call();
void func_call_param(SymTableItemPtr param);
void func_call_end(char* func);


// BUILTIN FUNCTIONS

void builtin_read();
void builtin_write();
void builtin_int2float();
void builtin_float2int();
void builtin_substring();
void builtin_ord();
void builtin_chr();
