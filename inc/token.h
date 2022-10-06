#ifndef __TOKEN_H__
#define __TOKEN_H__

#include "main.h"

typedef enum {
    TK_IDNT,    // identifier
    TK_KW,      // keyword
    TK_LIT,     // literal
    TK_NB,      // number
    TK_PUNC,    // punctuator (includes operators)
} token_lbl;

typedef struct {
    token_lbl lbl;
    // useful in error handling
    char* filename;
    unsigned line, col;
    char *val;
} token;

#endif