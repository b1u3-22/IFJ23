#include <stdlib.h>
#include <stdio.h>
#include "expression.h"
#include "rule_stack.h"

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

#define LL_TABLE_COL 34
#define LL_TABLE_ROW 18
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
    R_F_RET_DEF
};

static const int ll_table[LL_TABLE_ROW][LL_TABLE_COL] = 
{  // 0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17  18  19  20  21  22  23  24  25  26  27  28  29  30  31  32  33
    {37,  0,  0, 16, 15,  0, 10, 41,  1,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 45,  0,  0,  0,  0,  0, 45},   //  0
    {38,  0,  0, 18, 17,  0,  0, 42,  3,  4,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 46,  0,  0,  0,  0,  0, 46},   //  1
    { 5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  7,  6,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},   //  2
    { 8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  9,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},   //  3
    {12,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 11,  0,  0,  0,  0,  0, 12,  0,  0},   //  4
    { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 13,  0,  0, 14,  0,  0,  0,  0,  0},   //  5
    { 0,  0,  0,  0,  0,  0,  0,  0, 19,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 20,  0,  0,  0,  0,  0,  0,  0,  0,  0},   //  6
    {21, 22,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 52,  0,  0,  0,  0,  0,  0,  0, 52,  0,  0,  0,  0,  0,  0,  0,  0,  0},   //  7
    {26, 26, 26, 26, 26, 26, 26, 26, 26, 26,  0,  0, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 23, 26,  0, 26,  0,  0,  0,  0,  0, 26},   //  8
    {27, 27, 27, 27, 27, 27, 27, 27, 27, 27,  0,  0, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,  0,  0,  0, 27,  0,  0,  0,  0,  0, 27},   //  9
    {29, 30,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 28,  0,  0,  0,  0,  0,  0,  0,  0},   // 10
    { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 31,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 47, 32,  0,  0, 32,  0,  0,  0,  0,  0},   // 11
    { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 36,  0,  0, 33,  0,  0,  0,  0,  0},   // 12
    {34, 35,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},   // 13
    { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 40,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 39,  0,  0,  0,  0,  0,  0,  0,  0,  0},   // 14
    {44, 44,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},   // 15
    {48,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 49,  0,  0},   // 16
    { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 51,  0,  0, 50,  0,  0,  0,  0}    // 17
};

/** Parse incoming tokens using predictive syntax analysis using LL(1) grammar 
 * following rule sets defined in LL-table.
*/
int parse();

/** Apply rule to stack.
 * @param rule int of the rule
 * @param stack TokenStackPtr
*/  
void apply_rule(int rule, RuleStackPtr stack, TokenStackPtr token_stack);

/** Skip to the next sequence of tokens.
 *  This is used when syntax error occures. 
 *  @param stack TokenStackPtr
 *  @param token TokenPtr 
*/
RuleStackItemPtr error_skip(RuleStackPtr stack, TokenStackPtr token_stack);