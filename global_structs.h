
#define ALLOC_BLOCK 10

typedef struct Token {
    char* type;
    char* data;
}* TokenPtr;

void init_token(TokenPtr token) {
    token->type = malloc(sizeof(char) * ALLOC_BLOCK);
    token->data = NULL;
}