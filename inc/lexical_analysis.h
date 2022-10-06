#ifndef __LEXICAL_ANALYSIS_H__
#define __LEXICAL_ANALYSIS_H__

#include "main.h"
#include "token.h"
#include "symbol_tb.h"

#define PUNC_SIZE 48
#define KW_SIZE 32

typedef enum {
    WARN, ERR
} err_kind;

typedef struct {
    char* filename;
    char* cnt;           
    // position in the file's content (cnt) described with line and column 
    unsigned line, col;  
} str_file;


extern char *punc[], *kw[];

void read_file(char*);
void error_tok(err_kind, char*);
char *err_line();
void skip_whitespace();
char *str_copy(unsigned, bool);
char *number();
char *literal();
char *escape_char();
char *punctuator();
char *identifier();
bool keyword(char*);
void get_next_token();

#endif