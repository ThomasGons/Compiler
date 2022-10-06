#include "lexical_analysis.h"
#include "syntaxic_analysis.h"

str_file sf = {0};
token tok = {0};
symb_tb sb_t[BUCKET_SIZE] = {NULL};


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

int main(int argc, char **argv) {
    read_file(argv[1]);
    get_next_token();
    decl *new_decl = declaration();
    printf("specifier: %s\n \
            \rqualifier: %s\n \
            \rtype: %s\n \
            \ridentifier: %s\n \
            \rvalue: %s\n", new_decl->specifier, new_decl->qualifier, new_decl->type, new_decl->identifier, new_decl->value
    );
    // do {
    //     get_next_token();
    // } while (*sf.cnt != '\0');
    // display_symb_tb(sb_t);
    return 0;
}

void error_tok(err_kind kind, char *msg) {
    fprintf(stderr, BOLD "%s" RES ":%u:%u: %s: %s", 
        sf.filename, sf.line, sf.col,
        kind == WARN ? WARN_CLR "warning" RES : ERR_CLR "error" RES,
        msg);
    char* err_l = err_line(sf);
    fprintf(stderr, "\n%u |\t%s\n  |\t", sf.line, err_l);
    for (uint8_t i = 0; *(err_l + i) != '\0'; i++) {
        if (i > sf.col && isspace(*(err_l + i)))
            break;
        fprintf(stderr, i < sf.col ? " ":
                        i == sf.col ? 
                            ((kind == WARN) ? WARN_CLR "^" RES: ERR_CLR "^" RES):
                            ((kind == WARN) ? WARN_CLR "~" RES: ERR_CLR "~" RES));
    }
    fprintf(stderr, "\n");
    if (kind == ERR)
        exit(1);
}

char *err_line() {
    char *end_l = strchr(sf.cnt, '\n');
    unsigned final_len = end_l - (sf.cnt - sf.col);
    char *buffer = malloc(final_len);
    memset(buffer, '\0', final_len);
    strncpy(buffer, sf.cnt - sf.col, final_len);
    return buffer;
}


// The content of the file is retrieved and stored in a string
void read_file(char *fname) {
    FILE * fp = fopen (fname, "r");
    if (!fp) {
        perror("Error"); exit(1);
    }

    // calculation of the number of characters in the file
    fseek(fp, 0, SEEK_END);
    long length = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    // buffer that will store the content of the file
    char *buffer = malloc (sizeof(char) *length);
    if (!buffer) {
        perror("Error"); exit(1);
    }
    fread (buffer, 1, length, fp);
    fclose (fp);
    sf = (str_file) {fname, buffer, 1, 1}; // init to line 1 column 1
}


// check if the current token is a punctuator; if it's, it returns it
void skip_whitespace() {
    char *s = sf.cnt;
    unsigned len = 0;
    while (isspace(*s) || *s == '\a' || *s == '\b') {
        // line and column are updated if it's a new line
        len = (*s == '\t' || *s == '\v') ? 4: 1;
        if (*s == '\n') {
            sf.line++;
            sf.col = 0;
        }
        else  {
            sf.col += len;
        }
        s += len;
    }
    sf.cnt = s;
}

char *str_copy(unsigned len, bool is_lit) {
    if (!len)
        return NULL;

    // copy of the str;
    char *buffer = malloc(len);
    memset(buffer, '\0', len);
    strncpy(buffer, sf.cnt, len);
    sf.col += (len + (is_lit ? 1: 0));   // add the closing quote
    sf.cnt += (len + (is_lit ? 1: 0));
    return buffer;
}

char *number() {
    /* all base 10 floats are supported */
    char *s = sf.cnt;
    if (isdigit(*s) || (*s == '.' && isspace(*(s - 1)) || *(s - 1) == '=')  
        || (*s == '-' && isspace(*(s - 1)) || *(s - 1) == '=')) {
        char *tmp = s;
        unsigned len = 0;
        bool is_float = false;
        do {
            tmp++;
            len++;
            if (*tmp == '.') {
                // float cannot have mutliple floating points 
                if (is_float)
                    error_tok(WARN, "Bad number syntax");

                is_float = true;
                tmp++;
                len++;
            }
        } while (isdigit(*tmp));
        sf.col += len;
        return str_copy(len, false);
    }
    return NULL;
}

char *literal() {
    char *s = sf.cnt;
    if (*s == '\"' || *s == '\'') {
        char delim = *sf.cnt;
        if (!strchr(s + 1, delim))
            error_tok(WARN, "Unclosed quotes");
        
        if (*s =='\"') {
            unsigned len = 1;
            // '*s' acts as a delimiter "
            for (char *tmp = s + 1; *tmp != delim; *tmp++)
                len++;
            sf.cnt++;
            return str_copy(len - 1, true); // remove quotes
        }
        else { 
            // '*sf.cnt' acts as a delimiter ' 
            // check for negative values
            if (*(s + 2) == delim && (int) *(s + 1) < 127 && *(s + 1) != '\\') {
                sf.col += 3;
                sf.cnt++;
                return str_copy(1, true); // ascii char not escaped
            }
            else if (*(s + 3) == delim)
                return escape_char();
        }
        error_tok(WARN, "Not a char");
    }
    return NULL;
}

char *escape_char() {
    char *s = sf.cnt, *esc_char = malloc(sizeof *esc_char);
    switch (*(s + 2)) {
        case 'a': *esc_char = '\a'; break;
        case 'b': *esc_char = '\b'; break;
        case 'f': *esc_char = '\f'; break;
        case 'n': *esc_char = '\n'; break;
        case 'r': *esc_char = '\r'; break;
        case 't': *esc_char = '\t'; break;
        case 'v': *esc_char = '\v'; break;
        case '\\': *esc_char = '\\'; break; 
        case '\'': *esc_char = '\''; break;
        case '"': *esc_char = '\"'; break;
        case '0': *esc_char = '\0'; break;
        default: 
            error_tok(WARN, "Not an escape char");
    }
    sf.col += 4;
    return esc_char;
}

char *punctuator() {
    char *s = sf.cnt;
    size_t len = 0;
    for (uint8_t i = 0; i < PUNC_SIZE; i++) {
        len = strlen(punc[i]);
        if (!strncmp(s, punc[i], len)) {
            sf.col += len;
            sf.cnt += len;
            return punc[i];
        }
    }
    return NULL;
}

// check if the current token is an identifier; if it's, it returns it
char *identifier() {
    char *s = sf.cnt;
    // digits are allowed in identifier but not at the beginning
    if (isdigit(*s))  
        return NULL;
    // length of the identifier is necessary to copy it
    unsigned len = 0;
    // temporary variable to make the copy easier later
    char *tmp = s;
    while (isalnum(*tmp) || *tmp == '_') {
        len++;
        tmp++;
    }
    return str_copy(len, false);
}

// check if the given string is a keyword; if it's not, it's an identifier
bool keyword(char *s) {
    for (uint8_t i = 0; i < KW_SIZE; i++) {
        if (!strncmp(s, kw[i], strlen(kw[i])))
            return true;
    }
    return false;
}


void get_next_token() {
    char *tok_val = NULL;
    token_lbl tok_lbl = -1;
    unsigned hs; // hash for the symbol table

    // when any type of whitespace is encountered, just continue
    skip_whitespace();
    if (tok_val = number()) 
        tok_lbl = TK_NB;
    else if (tok_val = literal())
        tok_lbl = TK_LIT;
    else if (tok_val = punctuator())
        tok_lbl = TK_PUNC;
    else if (tok_val = identifier()) {
        if (keyword(tok_val))
            tok_lbl = TK_KW;
        else { 
            tok_lbl = TK_IDNT;
            unsigned hs = hash_djb2(tok_val);
            sb_t[hs] = add_entry_symb_tb(sb_t, tok_val, hs);
        }
    }
    else
        error_tok(ERR, "invalid token");
    tok = (token) {tok_lbl, sf.filename, sf.line, sf.col, tok_val};
}