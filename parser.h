#include <stdlib.h>
#include "scanner.h"
#include "token.h"
#include "token_stack.h"

#define LL_TABLE_COL 23
#define LL_TABLE_ROW 16
#define R_EOL 10        // Token with types that are less or equal to this number require EOL before them 

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
    R_RET_DEF
};

static const int ll_table[LL_TABLE_ROW][LL_TABLE_COL] = 
{  // 0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17  18  19  20  21  22
    {37,  0,  0, 16, 15,  0, 10, 41,  1,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},   //  0
    {38,  0,  0, 18, 17,  0,  0, 42,  3,  4,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},   //  1
    { 5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  7,  6,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},   //  2
    { 8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  9,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},   //  3
    {12,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 11,  0,  0,  0},   //  4
    { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 13,  0,  0, 14},   //  5
    { 0,  0,  0,  0,  0,  0,  0,  0, 19,  0,  0,  0,  0,  0,  0,  0,  0,  0, 20,  0,  0,  0,  0},   //  6
    {21, 22,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},   //  7
    {26, 26, 26, 26, 26, 26, 26, 26, 26, 26,  0,  0, 24, 24, 24, 24, 24, 24, 23,  0,  0,  0,  0},   //  8
    {27, 27, 27, 27, 27, 27, 27, 27, 27, 27,  0,  0, 25, 25, 25, 25, 25, 25,  0,  0,  0,  0,  0},   //  9
    {29, 30,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 28,  0,  0,  0},   // 10
    { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 31,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 32},   // 11
    { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 33},   // 12
    { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 36,  0,  0,  0},   // 13
    { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 40,  0,  0,  0,  0,  0,  0,  0, 39,  0,  0,  0,  0},   // 14
    {44, 44,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0}    // 15
};

/** Parse incoming tokens using predictive syntax analysis using LL(1) grammar 
 * following rule sets defined in LL-table.
*/
int parse();

/** Apply rule to stack.
 * @param rule int of the rule
 * @param stack TokenStackPtr
*/  
void apply_rule(int rule, TokenStackPtr stack);

/** Skip to the next sequence of tokens.
 *  This is used when syntax error occures. 
 *  @param stack TokenStackPtr
 *  @param token TokenPtr 
*/
TokenStackItemPtr error_skip(TokenStackPtr stack, TokenPtr token);