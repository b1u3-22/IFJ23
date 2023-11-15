#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define TOKEN_ALLOC_BLOCK 16

typedef struct Token {
    char *data;
    int data_len;
    int data_allocd;
    char *type;
} *TokenPtr;

/** Initialize token
 * @returns TokenPtr if succeded, NULL otherwise
*/
TokenPtr token_init();

/** Add new char to token data
 * @param c new char
 * @param token pointer to token to which c should be added
 * @returns true if succeded, false otherwise
*/
bool token_add_data(TokenPtr token, char c);

/** Delete token and free all memory */
void token_dispose(TokenPtr token);