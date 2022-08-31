#ifndef __SYMBOL_TABLE__
#define __SYMBOL_TABLE__

#include "main.h"

/* The symbol table is implemented using a hash map */

#define BUCKET_SIZE 256 // arbitrary number of buckets

typedef struct node {
    unsigned usg_l;
    struct node *next;
} node, *list;

typedef struct {
        char *name;
        char *type;
        unsigned size;      // in bytes
        uint8_t dim;        // dimension: 0 for the scalar
        unsigned decl_l;    // declaration line
        list usg_llist;     // lines of usage
        unsigned address;
} entry_d;

typedef struct entry {
    entry_d data;
    struct entry *next; // chained list to solve collisions
} entry, *symb_tb;


unsigned hash_djb2(char*);
symb_tb add_entry_symb_tb(symb_tb[], char*, unsigned);
bool lookup_symb_tb(symb_tb[], char*);
void display_symb_tb(symb_tb[]);


#endif