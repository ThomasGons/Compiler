#ifndef __SYNTAXIC_ANALYSIS_H__
#define __SYNTAXIC_ANALYSIS_H__

#include "main.h"
#include "lexical_analysis.h"
#include "token.h"
#include "symbol_tb.h"

// binary expression tree to parse condition, arithmetic or binary expressions

typedef struct bin_expr_node{
    union {
        token_lbl operator;
        char *identifier;
        void *value;
    };
    struct bin_expr_node *right;
    struct bin_expr_node *left;
} cond, expr;

// declarations are not statements 
typedef struct {    
    char *specifier;  // storage-class specifier: auto, register, static and extern
    char *qualifier;  // type qualifier: const, volatile and restrict
    char *type;       // int, float, char, pointer and compound types 
    char *identifier;
    char *value;      // void pointer to cast later;
} decl;

typedef struct param {
    decl parameter;
    struct param *next_parameter;
} param, *param_list;

// structures of all type of statements according to cppreference

typedef struct {
    char *identifier;
} labl_stmt;

typedef void *comp_stmt;

typedef union {
    expr expression;
    struct {
        char* function;
        param_list inputs;
    };
} expr_stmt;

typedef struct {
    cond condition;
} selc_stmt;


typedef union {
        // for statement with its declaration, its condition and its evaluation
        struct {
            union {
                decl declaration_iter;
                expr_stmt expression_iter;
            };
            cond condition_f;
            expr_stmt evaluation;
        };
        // do and while statement with just a condition
        cond condition_w;
} iter_stmt;

typedef struct {
    expr_stmt expr;
} jump_stmt;


typedef union {
    /* Any statement can be labeled, by providing a name followed by a
    colon before the statement itself.
    -> target for goto, case and default label in switch statement. */ 
    labl_stmt labl;
    
    /* A compound statement, or block, is a brace-enclosed sequence of
    statements and declarations. It forms its own block scope. */ 
    comp_stmt comp;

    /* Most statements in a typical C program are expression
    statements, such as assignments or function calls. */
    expr_stmt expr;

    /* The selection statements choose between one of several
    statements depending on the value of an expression. 
    -> if statement, eventually else (and else if also) and switch. */
    selc_stmt selc;

    /* The iteration statements repeatedly execute a statement.
    -> for, do, while loops. */
    iter_stmt iter;

    /* The jump statements unconditionally transfer flow control.
    -> break, continue, return, goto statements. */
    jump_stmt jump;
} stmt;


/* The parser will create a parse tree (Abstract Syntax Tree) for each function
as gcc does. parse tree is an n ary tree represented as follows */

typedef struct ast_node {
    char *label;
    union {
        decl declaration;
        struct {
            stmt statement;
            struct ast_node *child;     // body of the statements
        };
    };
    struct ast_node *sibling;
} parse_t_node, *parse_t;

// Add-on for the parse tree a kind of root that describes the function itself as a prototype.

typedef struct {
    param_list inputs;
    param output;
    parse_t body;
} parse_t_node_root, *parse_t_root;


extern token tok;
extern str_file sf;
extern symb_tb sb_t[BUCKET_SIZE];


bool in_array(char*[], uint8_t);
decl *declaration();

#endif