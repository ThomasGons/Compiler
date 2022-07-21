#ifndef __TOKEN_H__
#define __TOKEN_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// set of tokens in C language
typedef enum {
    tk_EOI, tk_Mul, tk_Div, tk_Mod, tk_Add, tk_Sub,
    tk_LBrc, tk_RBrc, tk_LPrt, tk_RPrt, tk_Semi, tk_Comma,
} token_type;

// struct for each token
typedef struct {
    token_type tok;
    int err_l, err_c; // line and column of error
    union {
        int value;
        char *text;
    };
} token;

token gettok();
void next_char();

#endif