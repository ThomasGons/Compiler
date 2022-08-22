#include "../inc/token.h"

static FILE *src_fp, *dest_fp;
static int line = 1, column = 0, current_char = '\0';

const char* keywords[32] = {
    "auto", "break", "case", "char", "const", "continue", "default", "do",
    "double", "else", "enum", "extern", "float", "for", "goto", "if", "int",
    "long", "register", "return", "short", "signed", "sizeof", "static", "struct",
    "switch", "typedef", "union", "unsigned", "void", "volatile", "while"
};

// retrieve the next har in the source file
void next_char() {
    char prev_char = current_char;
    current_char = getc(src_fp);
    column++;
    //refresh values after newline
    if (current_char == '\n') {
        line++;
        column = 0;
    }
}

/* return a token struct with a different token tag (token_val) 
   depending on whether the following char is the one expected or no. */
token following_char(char expected_char, token_val tk_True, token_val tk_False) {
    char prev_char = current_char;
    next_char();
    if (expected_char == current_char) {
        next_char();
        return (token) {tk_True, line, column, NULL};
    }
    else {
        // return an error for char that should not be alone
        if ((int) tk_False == -1) {
            printf("error: unexpected character: %c, line %d, column %d\n",
                prev_char, line, --column);
            exit(-1);
        }
        return (token) {tk_False, line, column, NULL};
    }
}

 
// token struct for identifiers or keywords
token ident_keyword() {
    int err_l = line, err_c = column;
    // allocate memory for the string that describes the identifier or the keyword
    char* str = malloc(sizeof(char));
    // just alphanumeric characters and underscore char are allowed for this token
    while (isalnum(current_char) || current_char == '_') {
        strcat(str, (char*) &current_char);
        err_l = line; err_c = column;
        next_char();
    }
    is_keyword(str);
    token_val keyw_ident = is_keyword(str) ? tk_Keyw : tk_Idnt;
    return (token) {keyw_ident, err_l, err_c, (char*) str};
}

// change token value if it's an keyword
bool is_keyword(char* str) {
    for (int i = 0; i < 32; i++) {
        if (!strcmp(str, keywords[i]))
            return true;
    }
    return false;

}

// token struct for literal expressions. Including comments and strings
token lit_expr(token_val tk_val, char delim) {
    int err_l = line, err_c = column;
    char *str = malloc(sizeof(char));
    // continue while the delim has not been encountered or if current char is not EOF
    while (delim != current_char) {
        if (current_char == EOF) {
            printf("missing terminated character %c, line: %d, column: %d\n",
             delim, line, column);
            exit(-1);
        }
        strcat(str, (char*) &current_char);
        err_l = line; err_c = column;
        next_char();
    }
    next_char();
    return (token) {tk_val, err_l, err_c, (char*) str};
}


bool is_div() {
    switch (current_char) {
        case '/': {
            while (column) {
                next_char();
            }
            return false;
        }
        case '*': {
            char prev_char;
            do {
                prev_char = current_char;
                next_char();
            } while (prev_char != '*' || current_char != '/');
            return false;
        }
        default: return true;
    }
}

/* determine the correct token according to the the current char
   in some cases according to the followings */
token gettok() {
    // all spaces are not considered (spaces, tabulations, ...)
    while (isspace(current_char))
        next_char();
    int err_l = line, err_c = column;
    switch (current_char) {
        case '*': next_char(); return (token) {tk_Mul, err_l, err_c, NULL};
        case '/': next_char(); if (is_div()) return (token) {tk_Div, err_l, err_c, NULL}; break;
        case '%': next_char(); return (token) {tk_Mod, err_l, err_c, NULL};
        case '+': next_char(); return (token) {tk_Add, err_l, err_c, NULL};
        case '-': next_char(); return (token) {tk_Sub, err_l, err_c, NULL};
        case '<': return following_char('=', tk_Le, tk_Lt);
        case '=': next_char(); return (token) {tk_Eq, err_l, err_c, NULL};
        case '>': return following_char('=', tk_Ge, tk_Gt);
        case '!': return following_char('=', tk_Neq, tk_Not);
        case '&': return following_char('&', tk_And, tk_Addr); // Not undefined but can be bitwise op or pointer adress
        case '|': return following_char('|', tk_Or, -1);  // Not undefined but can be bitwise op 
        case '{': next_char(); return (token) {tk_LBrc, err_l, err_c, NULL};
        case '}': next_char(); return (token) {tk_RBrc, err_l, err_c, NULL};
        case '[': next_char(); return (token) {tk_LBrk, err_l, err_c, NULL};
        case ']': next_char(); return (token) {tk_RBrk, err_l, err_c, NULL};
        case '(': next_char(); return (token) {tk_LPrt, err_l, err_c, NULL};
        case ')': next_char(); return (token) {tk_RPrt, err_l, err_c, NULL};
        case ';': next_char(); return (token) {tk_Semi, err_l, err_c, NULL};
        case ',': next_char(); return (token) {tk_Comma, err_l, err_c, NULL};
        case '.': next_char(); return (token) {tk_Dot, err_l, err_c, NULL};
        case ':': next_char(); return (token) {tk_Colon, err_l, err_c, NULL};
        case '#': next_char(); return (token) {tk_Hash, err_l, err_c, NULL};
        case '"': next_char(); return lit_expr(tk_Lit_Expr,'\"');
        case '\'': next_char(); return lit_expr(tk_Lit_Expr, '\'');
        default: return ident_keyword();
        case EOF: return (token) {tk_EOI, err_l, err_c, NULL};
    }
    gettok(); // recursive call for comments that are ignored
}

int main (int argc, char **argv) {
    token tok;
    src_fp = fopen(argv[1], "r");
    next_char();
    int i = 0;
    do {
        // get the next token
        tok = gettok();
        // write formated informations about this token in the dest file
        printf("%d  %d %.15s",
            tok.err_l, tok.err_c,
            &"End_of_Input    Op_Multiply     Op_Divide       Op_Modulo       Op_Add          \
Op_Subtract     Op_LesserThan   Op_LesserEqual  Op_Equal        Op_GreaterEqual Op_GreaterThan  \
Op_Not_Equal    Op_And          Op_Or           Op_Not          Op_Address      LeftBrace       \
RightBrace      LeftBracket     RightBracket    LeftParent      RightParent     Semicolon       \
Comma           Dot             Colon           Hash            Return          Type            \
Identifier      Keyword         Litteral_Expr"
            [tok.tok * 16]);
    } while (tok.tok != tk_EOI);
    return 0;
}