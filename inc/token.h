#ifndef __TOKEN_H__
#define __TOKEN_H__

#include "main.h"
#include "symbol_tb.h"

#define CIRC_BUF_LEN 8
#define PUNC_SIZE 48
#define KW_SIZE 32

#define IS_WHITESPACE(c) ((isspace(c) || (c) == '\a' || (c) == '\b') ? true: false)

#define INSUFFICIENT_SPACE_PEEK(f, c_buf, np_bytes) {                         \
    if ((size_t) CIRC_BUF_LEN - (c_buf->cread_p - c_buf->cnt) < np_bytes)     \
        peek(f, c_buf);                                                       \
}
#define CREAD_P_FORWARD(f, c_buf) {                              \
    if (c_buf->cread_p - c_buf->cnt == CIRC_BUF_LEN)             \
        fill_circ_buffer(f, c_buf, 0);                           \
    else                                                         \
        c_buf->cread_p++;                                        \
}          
#define UNDEFINED_SIZE_TOK(f, c_buf, buf, tmp, len) {            \
    if (tmp - c_buf->cnt == CIRC_BUF_LEN - 1) {                  \
        buf = fill_buffer(f, c_buf, buf, len + 1);               \
        tmp = c_buf->cnt;                                        \
        len = 0;                                                 \
    } else {                                                     \
        tmp++;                                                   \
        len++;                                                   \
    }                                                            \
}                                                                \


typedef struct {
    char c;
    unsigned line, col;
} circ_buf_d;

typedef struct {
    circ_buf_d *cread_p;
    circ_buf_d *cnt;
} circ_buf;

typedef enum {
    WARN, ERR
} err_kind;

typedef enum {
    TK_IDNT,    // identifier
    TK_KW,      // keyword
    TK_LIT,     // literal
    TK_NB,      // number
    TK_PUNC,    // punctuator (includes operators)
} token_kind;

typedef struct {
    token_kind tok;
    // useful in error handling
    char* filename;
    unsigned line, col;
    void *val;
} token;

typedef struct {
    FILE *sf;
    char* filename;           
    // position in the file's content (cnt) described with line and column 
    unsigned line, col;  
} file_d;

typedef struct {
    char *filename;
    char *cnt;
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


circ_buf init_circ_buffer();
void fill_circ_buffer(file_d*, circ_buf*, uint8_t);
circ_buf_d skip_whitespace(file_d*, bool);
void len_skipped(file_d*, char);
void peek(file_d*, circ_buf*);
void circ_buf_display_cnt(circ_buf_d*, unsigned);

// static void error_tok(str_file, err_kind, char*);
// static char *err_line(str_file);

bool compounded_strcmp(circ_buf_d*, char*);
char *str_copy(file_d*, circ_buf*, unsigned);
char *number(file_d*, circ_buf*, bool);
char *literal(file_d*, circ_buf*);
char *escape_char(file_d*, circ_buf_d*);
char *punctuator(file_d*, circ_buf*);
char *identifier(file_d*, circ_buf*);
bool keyword(char*);
token tokenize(file_d*, circ_buf*, symb_tb[], bool*);

#endif