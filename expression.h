/**
 *  Project:    Implementace překladače imperativního jazyka IFJ23.
 *  File:       @brief Hlavičkový soubor pro expression parser,definice
 *              enum hodnot a precedenční tabulky
 *  Authors:    @author Jiří Sedlák xsedla2e
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#ifndef _SCANNER
#define _SCANNER
#include "scanner.h"
#endif

#ifndef _TOKEN_STACK
#define _TOKEN_STACK
#include "token_stack.h"
#endif

#ifndef _ANALYZER
#define _ANALYZER
#include "analyzer.h"
#endif

#ifndef _EXP_STACK
#define _EXP_STACK
#include "expression_stack.h"
#endif

#ifndef _GEN_STACK
#define _GEN_STACK
#include "gen_stack.h"
#endif


#define EXPR_DEBUG 0

#define P_TABLE_SIZE 17

enum ExpressionOperation {
    E_ERR,  // ERROR - Not permitted combination
    E_SFT,  // SHIFT
    E_RED,  // REDUCE
    E_EQL,  // EQUAL
    E_SCS   // SUCCESS
};

enum ExpressionTypes {
    E_EXC,      // 0
    E_MUL,      // 1
    E_DIV,      // 2
    E_PLS,      // 3
    E_MIN,      // 4
    E_E,        // 5
    E_NE,       // 6
    E_GR,       // 7
    E_SM,       // 8
    E_EGR,      // 9
    E_ESM,      // 10
    E_QQ,       // 11
    E_LBRAC,    // 12
    E_RBRAC,    // 13
    E_ID,       // 14
    E_VALUE,    // 15
    E_END,      // 16
    E_CONCAT,   // 17
    E_NUL       // 18
};

static const int p_table[P_TABLE_SIZE][P_TABLE_SIZE] = 
{   //   0      1      2      3      4      5      6      7      8      9     10     11     12     13     14     15     16
    //   !      *      /      +      -     ==     !=      >      <     >=     <=     ??      (      )     ID  VALUE    END
    {E_ERR, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_SFT, E_ERR, E_SFT, E_SFT, E_RED},    //  !   0
    {E_SFT, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_SFT, E_RED, E_SFT, E_SFT, E_RED},    //  *   1
    {E_SFT, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_SFT, E_RED, E_SFT, E_SFT, E_RED},    //  /   2
    {E_SFT, E_SFT, E_SFT, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_SFT, E_RED, E_SFT, E_SFT, E_RED},    //  +   3
    {E_SFT, E_SFT, E_SFT, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_SFT, E_RED, E_SFT, E_SFT, E_RED},    //  -   4
    {E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_ERR, E_ERR, E_ERR, E_ERR, E_ERR, E_ERR, E_RED, E_SFT, E_RED, E_SFT, E_SFT, E_RED},    // ==   5
    {E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_ERR, E_ERR, E_ERR, E_ERR, E_ERR, E_ERR, E_RED, E_SFT, E_RED, E_SFT, E_SFT, E_RED},    // !=   6
    {E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_ERR, E_ERR, E_ERR, E_ERR, E_ERR, E_ERR, E_RED, E_SFT, E_RED, E_SFT, E_SFT, E_RED},    //  >   7
    {E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_ERR, E_ERR, E_ERR, E_ERR, E_ERR, E_ERR, E_RED, E_SFT, E_RED, E_SFT, E_SFT, E_RED},    //  <   8
    {E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_ERR, E_ERR, E_ERR, E_ERR, E_ERR, E_ERR, E_RED, E_SFT, E_RED, E_SFT, E_SFT, E_RED},    // >=   9
    {E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_ERR, E_ERR, E_ERR, E_ERR, E_ERR, E_ERR, E_RED, E_SFT, E_RED, E_SFT, E_SFT, E_RED},    // <=   10  
    {E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_RED, E_SFT, E_SFT, E_RED},    // ??   11
    {E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_EQL, E_SFT, E_SFT, E_RED},    //  (   12
    {E_ERR, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_ERR, E_RED, E_ERR, E_ERR, E_RED},    //  )   13
    {E_ERR, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_ERR, E_RED, E_ERR, E_ERR, E_RED},    // ID   14
    {E_ERR, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_ERR, E_RED, E_ERR, E_ERR, E_RED},    // VA   15
    {E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_SCS},    // EN   16
};

int parse_expression(AnalyzerPtr analyzer, int end_type, TokenStackPtr sa_stack, GenStackPtr gen_stack);
void expression_get_next_token(TokenStackPtr stack, int end_type, int *type, TokenStackPtr sa_stack);
int get_translated_type(TokenPtr token);
bool apply_expression_rule(AnalyzerPtr analyzer, ExpressionStackPtr stack, TokenStackPtr token_stack, GenStackPtr gen_stack);
