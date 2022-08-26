#ifndef __TOKEN_H__
#define __TOKEN_H__

#include "main.h"

// temporary macro for error handling
#define ERROR(fmt, args...) ({fprintf(stderr, fmt, ##args); exit(1);})

typedef enum {
    false, true
} bool;

typedef enum {
    TK_IDNT,    // identifier
    TK_KW,      // keyword
    TK_LIT,     // literal
    TK_NB_LIT,  // numbers
    TK_PUNC,    // punctuator (includes operators)
} token_lbl;

typedef struct {
    token_lbl tok;
    // useful in error handling
    char* filename;
    int line, col;
    void *val;
} token;

typedef struct {
    char* filename;
    char* cnt;           
    // position in the file's content (cnt) described with line and column 
    unsigned line, col;  
} str_file;

// all punctuators in C
char *punc[] = { 
    "{", "}", "(", ")", "[", "]", ";", ".", "->", ",", "?", ":",
    "+", "-", "*", "/", "%", "++", "--", "=", "+=", "-=", "*=", "/=", "%=",
    "==", "!=", "<=", ">=", "<", ">", "||", "&&", 
    "!", "|", "&", "^", "~", "<<", ">>", "|=", "&=", "^=", "<<=", ">>=",
    "...", "#", "##"
};

// all keywords in C
char *kw[] = {
    "auto", "break", "case", "char", "const", "continue", "default",
    "do", "double", "else", "enum", "extern", "float", "for", "goto",
    "if", "int", "long", "register", "return", "short", "signed",
    "sizeof", "static", "struct", "switch", "typedef", "union", "unsigned",
    "void", "volatile", "while"
};

extern token tokenize (str_file);
static str_file read_file (char*);
static char *punctuator (char*);
static bool is_keyword (char*);
static char *identifier (char*);
// static error_tok(char, ...);

#endif