#include "../inc/token.h"



static FILE *src_fp, *dest_fp;
static int line = 1, column = 0, current_char = '\0';

// retrieve next_char in the source file
void next_char() {
    current_char = getc(src_fp);
    column++;
    if (current_char == '\n') {
        line++;
        column = 0;
    }
}

token gettok() {
    while (isspace(current_char))
        next_char();
    int err_l = line, err_c = column;
    printf("%c", current_char);
    switch (current_char) {
        case '{': next_char(&current_char); return (token) {tk_LBrc, err_l, err_c, {0}};
        case '}': next_char(&current_char); return (token) {tk_RBrc, err_l, err_c, {0}};
        case '(': next_char(&current_char); return (token) {tk_LPrt, err_l, err_c, {0}};
        case ')': next_char(&current_char); return (token) {tk_RPrt, err_l, err_c, {0}};
        case '+': next_char(&current_char); return (token) {tk_Add, err_l, err_c, {0}};
        case '-': next_char(&current_char); return (token) {tk_Sub, err_l, err_c, {0}};
        case '*': next_char(&current_char); return (token) {tk_Mul, err_l, err_c, {0}};
        case '%': next_char(&current_char); return (token) {tk_Mod, err_l, err_c, {0}};
        // case '/': next_char(&current_char); return (token) {tk_Div, err_l, err_c, {0}};
        // case '\'': next_char(&current_char); return (token) {tk_Lbrace, err_l, err_c, {0}};
        case ';': next_char(&current_char); return (token) {tk_Semi, err_l, err_c, {0}};
        case ',': next_char(&current_char); return (token) {tk_Comma, err_l, err_c, {0}};
        // case '<': next_char(&current_char); return (token) {tk_Lbrace, err_l, err_c, {0}};
        // case '>': next_char(&current_char); return (token) {tk_Lbrace, err_l, err_c, {0}};
        // case '=': next_char(&current_char); return (token) {tk_Lbrace, err_l, err_c, {0}};
        // case '!': next_char(&current_char); return (token) {tk_Lbrace, err_l, err_c, {0}};
        // case '&': next_char(&current_char); return (token) {tk_Lbrace, err_l, err_c, {0}};
        // case '|': next_char(&current_char); return (token) {tk_Lbrace, err_l, err_c, {0}};
        // case '"': next_char(&current_char);
        case EOF: return (token) {tk_EOI, err_l, err_c, {0}};
    }
}

int main (int argc, char **argv) {
    token tok;
    src_fp = fopen("config/training", "r");
    do {
        tok = gettok();
        printf("%5d  %5d %.15s",
            tok.err_l, tok.err_c,
            &"End_of_input    Op_multiply     Op_divide       Op_modulo       Op_add          "
             "Op_subtract     LeftBrace       RightBrace      LeftParent      RightParent     "
             "Semicolon       Comma           "
            [tok.tok * 16]);
    } while (tok.tok != tk_EOI);
    return 0;
}