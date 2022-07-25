#include "../inc/token.h"

static FILE *src_fp, *dest_fp;
static int line = 1, column = 0, current_char = '\0';

// retrieve next_char in the source file
void next_char() {
    char prev_char = current_char;
    current_char = getc(src_fp);
    column++;
    if (current_char == '\n') {
        line++;
        column = 0;
    }
}

token following_char(char expected_char, token_val tk_True, token_val tk_False) {
    char prev_char = current_char;
    next_char();
    if (expected_char == current_char) {
        next_char();
        return (token) {tk_True, line, column, {0}};
    }
    else {
        if ((int) tk_False == -1)
            error_token(prev_char);
        return (token) {tk_False, line, column, {0}};
    }
}

token ident_keyword() {
    int err_l = line, err_c = column;
    char* str = malloc(sizeof(char));
    while (isalnum(current_char) || current_char == '_') {
        strcat(str, (char*) &current_char);
        err_l = line; err_c = column;
        next_char();
    }
    return (token) {tk_Idnt, err_l, err_c, {.text=str}};
}

token lit_expr(token_val tk_val, char delim) {
    int err_l = line, err_c = column;
    char *str = malloc(sizeof(char));
    while (delim != current_char) {
        if (current_char == EOF) error(line, column, "missing terminated character \"");
        strcat(str, (char*) &current_char);
        err_l = line; err_c = column;
        next_char();
    }
    next_char();
    return (token) {tk_val, err_l, err_c, {.text=str}};
}

token div_com() {
    switch (current_char) {
        case '/': next_char(); return lit_expr(tk_Com, '\n');
        // case '*': return lit_expr(tk_Com, "*/");
        default: return (token) {tk_Div, line, column, {0}};
    }
}

token gettok() {
    while (isspace(current_char))
        next_char();
    int err_l = line, err_c = column;
    switch (current_char) {
        case '*': next_char(); return (token) {tk_Mul, err_l, err_c, {0}};
        case '/': next_char(); return div_com();
        case '%': next_char(); return (token) {tk_Mod, err_l, err_c, {0}};
        case '+': next_char(); return (token) {tk_Add, err_l, err_c, {0}};
        case '-': next_char(); return (token) {tk_Sub, err_l, err_c, {0}};
        case '<': return following_char('=', tk_Le, tk_Lt);
        case '=': next_char(); return (token) {tk_Eq, err_l, err_c, {0}};
        case '>': return following_char('=', tk_Ge, tk_Gt);
        case '!': return following_char('=', tk_Neq, tk_Not);
        case '&': return following_char('&', tk_And, tk_Addr); // Not undefined but can be bitwise op or pointer adress
        case '|': return following_char('|', tk_Or, -1);  // Not undefined but can be bitwise op 
        case '{': next_char(); return (token) {tk_LBrc, err_l, err_c, {0}};
        case '}': next_char(); return (token) {tk_RBrc, err_l, err_c, {0}};
        case '[': next_char(); return (token) {tk_LBrk, err_l, err_c, {0}};
        case ']': next_char(); return (token) {tk_RBrk, err_l, err_c, {0}};
        case '(': next_char(); return (token) {tk_LPrt, err_l, err_c, {0}};
        case ')': next_char(); return (token) {tk_RPrt, err_l, err_c, {0}};
        case ';': next_char(); return (token) {tk_Semi, err_l, err_c, {0}};
        case ',': next_char(); return (token) {tk_Comma, err_l, err_c, {0}};
        case '.': next_char(); return (token) {tk_Dot, err_l, err_c, {0}};
        case ':': next_char(); return (token) {tk_Colon, err_l, err_c, {0}};
        case '#': next_char(); return (token) {tk_Hash, err_l, err_c, {0}};
        case '"': next_char(); return lit_expr(tk_Lit_Expr,'\"');
        case '\'': next_char(); return lit_expr(tk_Lit_Expr, '\'');
        default: return ident_keyword();
        case EOF: return (token) {tk_EOI, err_l, err_c, {0}};
    }
}

void error_token(char prev_char) {
    printf("error: unexpected character: %c, line %d, column %d\n",
     prev_char, line, --column);
    exit(-1);
}

int main (int argc, char **argv) {
    token tok;
    src_fp = fopen("config/training", "r");
    next_char();
    int i = 0;
    do {
        tok = gettok();
        printf("%d  %d %.15s",
            tok.err_l, tok.err_c,
            &"End_of_Input    Op_Multiply     Op_Divide       Op_Modulo       Op_Add          \
Op_Subtract     Op_LesserThan   Op_LesserEqual  Op_Equal        Op_GreaterEqual Op_GreaterThan  \
Op_Not_Equal    Op_And          Op_Or           Op_Not          Op_Address      LeftBrace       \
RightBrace      LeftBracket     RightBracket    LeftParent      RightParent     Semicolon       \
Comma           Dot             Colon           Hash            Return          Type            \
Identifier      Litteral_Expr   Comment         "
            [tok.tok * 16]);
    printf(!tok.text ? "\n": ": %s\n", tok.text);
    } while (tok.tok != tk_EOI);
    return 0;
}