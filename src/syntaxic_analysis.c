#include "syntaxic_analysis.h"


char *spec[] = {
    "auto", "extern", "register", "static"
};

char *qual[] = {
    "const", "restrict", "volatile"
};

char *type[] = {
    "char", "float", "int"
};

bool in_array(char* array[], uint8_t len) {
    for (uint8_t i = 0; i < len; i++) {
        if (!strcmp(array[i], tok.val))
            return true;
    }
    return false;
}


decl *declaration() {
    decl *new_decl = malloc(sizeof *new_decl);
    if (!new_decl)
        exit(EXIT_FAILURE);
    *new_decl = (decl) {NULL};
    if (tok.lbl == TK_KW && in_array(spec, 4)) {        
        new_decl->specifier = tok.val;
        get_next_token();
    }
    
    if (tok.lbl == TK_KW && in_array(qual, 3)) {
        new_decl->qualifier = tok.val;
        get_next_token();
    }
    if (tok.lbl == TK_KW && in_array(type, 3)) {
        new_decl->type = tok.val;
        get_next_token();
    } else
        error_tok(ERR, "missing type");
    if (tok.lbl == TK_IDNT) {
        new_decl->identifier = tok.val;
        get_next_token();
    } else
        error_tok(ERR, "missing identifier");
    if (*tok.val == '=') {
        get_next_token();
        new_decl->value = tok.val;
    } else if (*tok.val != ';')
        error_tok(ERR, "missing assignment symbol");
    return new_decl;
    
}