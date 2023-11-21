#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "expression_stack.h"

#ifndef _TOKEN
#define _TOKEN
#include "scanner.h"
#endif

#define P_TABLE_SIZE 17

enum ExpressionOperation {
    E_ERR,  // ERROR - Not permitted combination
    E_SFT,  // SHIFT
    E_RED,  // REDUCE
    E_EQL,  // EQUAL
    E_SCS   // SUCCESS
};

enum ExpressionTypes {
    E_EXC,
    E_MUL,
    E_DIV,
    E_PLS,
    E_MIN,
    E_E,
    E_NE,
    E_GR,
    E_SM,
    E_EGR,
    E_ESM,
    E_QQ,
    E_LBRAC,
    E_RBRAC,
    E_ID,
    E_VALUE,
    E_END
};

static const int p_table[P_TABLE_SIZE][P_TABLE_SIZE] = 
{   //   !      *      /      +      -     ==     !=      >      <     >=     <=     ??      (      )     ID  VALUE     END
    {E_ERR, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_SFT, E_ERR, E_SFT, E_SFT, E_RED},    //  !
    {E_SFT, E_RED, E_ERR, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_SFT, E_RED, E_SFT, E_SFT, E_RED},    //  *
    {E_SFT, E_ERR, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_SFT, E_RED, E_SFT, E_SFT, E_RED},    //  /
    {E_SFT, E_SFT, E_SFT, E_RED, E_ERR, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_SFT, E_RED, E_SFT, E_SFT, E_RED},    //  +
    {E_SFT, E_SFT, E_SFT, E_ERR, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_SFT, E_RED, E_SFT, E_SFT, E_RED},    //  -
    {E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_ERR, E_ERR, E_ERR, E_ERR, E_ERR, E_ERR, E_RED, E_SFT, E_RED, E_SFT, E_SFT, E_RED},    // ==
    {E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_ERR, E_ERR, E_ERR, E_ERR, E_ERR, E_ERR, E_RED, E_SFT, E_RED, E_SFT, E_SFT, E_RED},    // !=
    {E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_ERR, E_ERR, E_ERR, E_ERR, E_ERR, E_ERR, E_RED, E_SFT, E_RED, E_SFT, E_SFT, E_RED},    //  >
    {E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_ERR, E_ERR, E_ERR, E_ERR, E_ERR, E_ERR, E_RED, E_SFT, E_RED, E_SFT, E_SFT, E_RED},    //  <
    {E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_ERR, E_ERR, E_ERR, E_ERR, E_ERR, E_ERR, E_RED, E_SFT, E_RED, E_SFT, E_SFT, E_RED},    // >=
    {E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_ERR, E_ERR, E_ERR, E_ERR, E_ERR, E_ERR, E_RED, E_SFT, E_RED, E_SFT, E_SFT, E_RED},    // <=
    {E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_RED, E_SFT, E_SFT, E_RED},    // ??
    {E_SFT, E_ERR, E_ERR, E_ERR, E_ERR, E_ERR, E_ERR, E_ERR, E_ERR, E_ERR, E_ERR, E_ERR, E_SFT, E_EQL, E_SFT, E_SFT, E_RED},    //  (
    {E_ERR, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_ERR, E_RED, E_ERR, E_ERR, E_RED},    //  )
    {E_ERR, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_ERR, E_RED, E_ERR, E_ERR, E_RED},    // ID
    {E_ERR, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_RED, E_ERR, E_RED, E_ERR, E_ERR, E_RED},    // VA
    {E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_SFT, E_SCS},    // EN
};

int parse_expression();
void expression_get_next_token(TokenPtr token, int end_type, int *type);
int get_translated_type(TokenPtr token);
bool apply_expression_rule(ExpressionStackPtr stack);