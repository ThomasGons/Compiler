#ifndef __TOKEN_H__
#define __TOKEN_H__

#include "main.h"
#include "symbol_tb.h"

#define PUNC_SIZE 48
#define KW_SIZE 32

typedef enum {
    WARN, ERR
} err_kind;

typedef enum {
    TK_IDNT,    // identifier
    TK_KW,      // keyword
    TK_LIT,     // literal
    TK_NB,      // number
    TK_PUNC,    // punctuator (includes operators)
} token_lbl;

typedef struct {
    token_lbl tok;
    // useful in error handling
    char* filename;
    unsigned line, col;
    void *val;
} token;

typedef struct {
    char* filename;
    char* cnt;           
    // position in the file's content (cnt) described with line and column 
    unsigned line, col;  
} str_file;

// all punctuators in C arranged in descending length
char *punc[] = {
    "...", 
    "<<=", ">>=", 
    
    "+=", "-=", "*=", "/=", "%=", "++", "--", 
    "<<", ">>", "|=", "&=", "^=",
    "==", "!=", "<=", ">=", "||", "&&", 
    "->", "##",

    "=", "+", "-", "*", "/", "%",
    "!", "|", "&", "^", "~",
    "<", ">",
    "{", "}", "(", ")", "[", "]", ";", ",", "?", ":",
    ".", "#"
};

// all keywords in C (alphabetical order)
char *kw[] = {
    "auto", "break", "case", "char", "const", "continue", "default",
    "do", "double", "else", "enum", "extern", "float", "for", "goto",
    "if", "int", "long", "register", "return", "short", "signed",
    "sizeof", "static", "struct", "switch", "typedef", "union", "unsigned",
    "void", "volatile", "while"
};

static str_file read_file(char*);
static void error_tok(str_file, err_kind, char*);
static char *err_line(str_file);
static void skip_whitespace(str_file*);
static char *str_copy(str_file*, unsigned, bool);
static char *number(str_file*);
static char *literal(str_file*);
static char *escape_char(str_file *);
static char *punctuator(str_file*);
static char *identifier(str_file*);
static bool keyword(char*);
extern token tokenize(str_file*, symb_tb[]);

#endif