#ifndef __TOKEN_H__
#define __TOKEN_H__

#include "main.h"


typedef enum {
    false, true, unknown
} bool;

// set of tokens in C language
typedef enum {
    tk_EOI,
    
    // operators
    tk_Mul, tk_Div, tk_Mod, tk_Add, tk_Sub, 
    tk_Lt, tk_Le, tk_Eq, tk_Ge, tk_Gt, tk_Neq, 
    tk_And, tk_Or, tk_Not, tk_Addr,

    // separators
    tk_LBrc, tk_RBrc, tk_LBrk, tk_RBrk,
    tk_LPrt, tk_RPrt, tk_Semi, tk_Comma,
    tk_Dot, tk_Colon, tk_Hash,
    
    // keywords
    tk_Return, tk_Type,

    // identifiers
    tk_Idnt, tk_Keyw,

    tk_Lit_Expr
} token_val;

// struct for each token
typedef struct {
    token_val tok;
    int err_l, err_c; // line and column of error
    void *value;
} token;


void next_char();
token following_char(char, token_val, token_val);
token ident_keyword();
bool is_keyword(char*);
token lit_expr(token_val, char);
bool is_div();
token gettok();
void error_token(char);

#endif