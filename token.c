#include "token.h"

TokenPtr token_init(){
    TokenPtr token = (TokenPtr) malloc(sizeof(struct Token));
    if (!token) return NULL;
    token->data = NULL;
    token->data_len = 0;
    token->data_allocd = 0;
    return token;
}

bool token_add_data(TokenPtr token, char c){
    if (token->data_len == token->data_allocd){ // no more space in data, we need to allocate more memory
        char* tmp_data = (char*) realloc(token->data, token->data_allocd + ALLOC_BLOCK); // allocate more memory
        if (!tmp_data) return false; // if realloc failed return false
        token->data = tmp_data; // if it succeded set data to new data with more memory
        token->data_allocd += ALLOC_BLOCK; // correct allocated data size
    }
    token->data[token->data_len++] = c; // add char to end of data
    return true;
}

void token_dispose(TokenPtr token) {
    free(token);
}