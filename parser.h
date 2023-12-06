/**
 *  Project:    Implementace překladače imperativního jazyka IFJ23.
 *  File:       @brief Hlavičkový soubor pro parser, definice LL-tabulky
 *  Authors:    @author Jiří Sedlák xsedla2e
*/

#include <stdlib.h>
#include <stdio.h>
#include "rule_stack.h"

#ifndef _ANALYZER
#define _ANALYZER
#include "analyzer.h"
#endif

#ifndef _C_GEN
#define _C_GEN
#include "code_generator.h"
#endif

#ifndef _TOKEN
#define _TOKEN
#include "token.h"
#endif

#ifndef _SCANNER
#define _SCANNER
#include "scanner.h"
#endif

#ifndef _TOKEN_STACK
#define _TOKEN_STACK
#include "token_stack.h"
#endif

#ifndef _SYMTABLE
#define _SYMTABLE
#include "symtable.h"
#endif

#ifndef _EXPRESSION
#define _EXPRESSION
#include "expression.h"
#endif

#ifndef _GEN_STACK
#define _GEN_STACK
#include "gen_stack.h"
#endif

#define PARSER_DEBUG    0
#define CODE_GEN        1

#define LL_TABLE_COL 34
#define LL_TABLE_ROW 19
#define R_EOL_B 2       // Tokens which require EOL only when current rule is either R_G_BODY or R_BODY
#define R_EOL 9         // Token with types that are less or equal to this number require EOL before them 

enum Rules {
    R_G_BODY,
    R_BODY,
    R_VAR_DEF,
    R_VAR_ASG,
    R_F_DEF_F,
    R_F_DEF_N,
    R_CON_DEF,
    R_EXPR,
    R_EXPR_ID,
    R_EXPR_OP,
    R_F_PAR_F,
    R_F_PAR_ID,
    R_F_PAR_N,
    R_F_PAR,
    R_STAT,
    R_RET_DEF,
    R_F_DEF_ID,
    R_F_RET_DEF,
    R_F_PAR_NA
};

enum Function {
    F_P_GET_T,      // Get new token
    F_P_PUSH_1,     // Push to Stack 1 for analyzer and generator
    F_P_PUSH_2,     // Push to Stack 2 for analyzer and generator
    F_P_PUSH_3,     // Push to Stack 3 for generator
    F_P_POP_1,      // Pop from Stack 1
    F_P_POP_2,      // Pop from Stack 2
    F_P_POP_3,      // Pop from Stack 3
    F_P_CLEAR_1,    // Clear Stack 1
    F_P_CLEAR_2,    // Clear Stack 2
    F_P_CLEAR_3,    // Clear Stack 3
    F_P_PSA,        // Start expression parser
    F_P_GEN_C,      // Generate instuction from sequence generated by PSA with typedef checks
    F_P_GEN,        // Generate instructions from sequence generated by PSA without typdef checks
    F_S_INC_DEP,    // Increase depth in semantic analyzer
    F_S_DEC_DEP,    // Decrease depth in semantic analyzer
    F_S_VAR_DEC,    // Check variable declaration with analyzer
    F_S_VAR_DEF,    // Check variable definition with analyzer
    F_S_VAL_ASG,    // Check value assigment with analyzer      e.g. a = 5 + b
    F_S_FUN_ASG,    // Check function assigment with analyzer   e.g. a = b(5, a : c)   
    F_S_FUN_CAL,    // Check function call
    F_S_FUN_DEF,    // Check function definition
    F_S_CON_C,       // Check condition
    F_G_DEF_VAR,    // Generate variable definition
    F_G_SET_VAR,    // Generate variable assigment
    F_G_PUSH,       // Push value into generator stack
    F_G_IF_N,       // Generate condition for if let ID
    F_G_IF_S,       // Generate start of if
    F_G_IF_EL,      // Generate end of if, start of else
    F_G_IF_E,       // Generate end of else
    F_G_W_S,        // Generate start of while
    F_G_W_C,        // Generate condition check for while
    F_G_W_E,        // Generate end of while
    F_G_FUN_S,      // Generate start of function definition
    F_G_FUN_P,      // Generate function parameter
    F_G_FUN_R,      // Generate function return
    F_G_FUN_C,      // Generate function call
    F_G_FUN_C_P,    // Generate function call parameter
    F_G_FUN_E,      // Generate end of function definition
    F_G_SAVE_SYM,   // Save item to save it ¯\_(ツ)_/¯
    F_G_SYM_CONF    // Confirm saving item
};

static const int ll_table[LL_TABLE_ROW][LL_TABLE_COL] = 
{  // 0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17  18  19  20  21  22  23  24  25  26  27  28  29  30  31  32  33
    {37,  0,  0, 16, 15,  0, 10, 41,  1,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 53,  0,  0,  0,  0,  0, 45},   //  0
    {38,  0,  0, 18, 17,  0,  0, 42,  3,  4,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 54,  0,  0,  0,  0,  0, 46},   //  1
    { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  7,  6,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},   //  2
    { 8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  9,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},   //  3
    {12,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 11,  0,  0,  0,  0,  0, 12,  0,  0},   //  4
    { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 13,  0,  0, 14,  0,  0,  0,  0,  0},   //  5
    {20, 20,  0,  0,  0,  0,  0,  0, 19,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 20,  0,  0,  0,  0,  0,  0,  0,  0,  0},   //  6
    {21, 22,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 52,  0,  0,  0,  0,  0,  0,  0, 52,  0,  0,  0,  0,  0,  0,  0,  0,  0},   //  7
    {26, 26, 26, 26, 26, 26, 26, 26, 26, 26,  0,  0, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 23, 26,  0, 26,  0,  0,  0,  0,  0, 26},   //  8
    {27, 27, 27, 27, 27, 27, 27, 27, 27, 27,  0,  0, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,  0, 27,  0, 27,  0,  0,  0,  0,  0, 27},   //  9
    {29, 30,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 28,  0,  0,  0,  0,  0,  0,  0,  0},   // 10
    { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 31,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 47, 32,  0,  0, 32,  0,  0,  0,  0,  0},   // 11
    { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 36,  0,  0, 33,  0,  0,  0,  0,  0},   // 12
    {34, 35,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},   // 13
    { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 40,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 39,  0,  0,  0,  0,  0,  0,  0,  0,  0},   // 14
    {44, 44,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},   // 15
    {48,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 49,  0,  0},   // 16
    { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 51,  0,  0, 50,  0,  0,  0,  0},   // 17
    { 55, 56,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0}   // 18
};

/** Parse incoming tokens using predictive syntax analysis using LL(1) grammar 
 * following rule sets defined in LL-table.
*/
int parse();

/** Apply rule to stack.
 * @param rule int of the rule
 * @param stack TokenStackPtr
*/  
void apply_rule(int rule, RuleStackPtr stack, TokenStackPtr token_stack, TokenStackPtr sa_stack);

void apply_function(int function, RuleStackPtr rule_stack, TokenPtr token, TokenStackPtr token_stack, TokenStackPtr stack_1, TokenStackPtr stack_2, TokenStackPtr stack_3, AnalyzerPtr analyzer, int rule, int *func_ass, GenStackPtr gen_stack);