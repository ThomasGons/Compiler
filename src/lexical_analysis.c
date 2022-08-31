#include "../inc/token.h"
#include "../inc/symbol_tb.h"


int main(int argc, char **argv) {
    str_file sf = read_file(argv[1]);
    symb_tb sb_t[BUCKET_SIZE] = {NULL};
    token tok;
    do {
        tok = tokenize(&sf, sb_t);
    } while (*sf.cnt != '\0');
    // display_symb_tb(sb_t);
    return 0;
}

static void error_tok(str_file sf, err_kind kind, char *msg) {
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

static char *err_line(str_file sf) {
    char *end_l = strchr(sf.cnt, '\n');
    unsigned final_len = end_l - (sf.cnt - sf.col);
    char *buffer = malloc(final_len);
    memset(buffer, '\0', final_len);
    strncpy(buffer, sf.cnt - sf.col, final_len);
    return buffer;
}


// The content of the file is retrieved and stored in a string
static str_file read_file(char *fname) {
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
    return (str_file) {fname, buffer, 1, 1}; // init to line 1 column 1
}


// check if the current token is a punctuator; if it's, it returns it
static void skip_whitespace(str_file *sf) {
    char *s = sf->cnt;
    unsigned len = 0;
    while (isspace(*s) || *s == '\a' || *s == '\b') {
        // line and column are updated if it's a new line
        len = (*s == '\t' || *s == '\v') ? 4: 1;
        if (*s == '\n') {
            sf->line++;
            sf->col = 0;
        }
        else  {
            sf->col += len;
        }
        s += len;
    }
    sf->cnt = s;
}

static char *str_copy(str_file *sf, unsigned len, bool is_lit) {
    if (!len)
        return NULL;

    // copy of the str;
    char *buffer = malloc(len);
    memset(buffer, '\0', len);
    strncpy(buffer, sf->cnt, len);
    sf->col += (len + (is_lit ? 1: 0));   // add the closing quote
    sf->cnt += (len + (is_lit ? 1: 0));
    return buffer;
}

static char *number(str_file *sf) {
    /* all base 10 floats are supported */
    char *s = sf->cnt;
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
                    error_tok(*sf, WARN, "Bad number syntax");

                is_float = true;
                tmp++;
                len++;
            }
        } while (isdigit(*tmp));
        sf->col += len;
        return str_copy(sf, len, false);
    }
    return NULL;
}

static char *literal(str_file *sf) {
    char *s = sf->cnt;
    if (*s == '\"' || *s == '\'') {
        char delim = *sf->cnt;
        if (!strchr(s + 1, delim))
            error_tok(*sf, WARN, "Unclosed quotes");
        
        if (*s =='\"') {
            unsigned len = 1;
            // '*s' acts as a delimiter "
            for (char *tmp = s + 1; *tmp != delim; *tmp++)
                len++;
            sf->cnt++;
            return str_copy(sf, len - 1, true); // remove quotes
        }
        else { 
            // '*sf->cnt' acts as a delimiter ' 
            // check for negative values
            if (*(s + 2) == delim && (int) *(s + 1) < 127 && *(s + 1) != '\\') {
                sf->col += 3;
                sf->cnt++;
                return str_copy(sf, 1, true); // ascii char not escaped
            }
            else if (*(s + 3) == delim)
                return escape_char(sf);
        }
        error_tok(*sf, WARN, "Not a char");
    }
    return NULL;
}

static char *escape_char(str_file *sf) {
    char *s = sf->cnt, *esc_char = malloc(sizeof *esc_char);
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
            error_tok(*sf, WARN, "Not an escape char");
    }
    sf->col += 4;
    return esc_char;
}

char *punctuator(str_file *sf) {
    char *s = sf->cnt;
    size_t len = 0;
    for (uint8_t i = 0; i < PUNC_SIZE; i++) {
        len = strlen(punc[i]);
        if (!strncmp(s, punc[i], len)) {
            sf->col += len;
            sf->cnt += len;
            return punc[i];
        }
    }
    return NULL;
}

// check if the current token is an identifier; if it's, it returns it
static char *identifier(str_file *sf) {
    char *s = sf->cnt;
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
    return str_copy(sf, len, false);
}

// check if the given string is a keyword; if it's not, it's an identifier
static bool keyword(char *s) {
    for (uint8_t i = 0; i < KW_SIZE; i++) {
        if (!strncmp(s, kw[i], strlen(kw[i])))
            return true;
    }
    return false;
}


extern token tokenize(str_file *sf, symb_tb sb_t[]) {
    char *tok_val = NULL;
    token_lbl tok_lbl = -1;
    unsigned hs; // hash for the symbol table

    // when any type of whitespace is encountered, just continue
    skip_whitespace(sf);
    if (tok_val = number(sf)) 
        tok_lbl = TK_NB;
    else if (tok_val = literal(sf))
        tok_lbl = TK_LIT;
    else if (tok_val = punctuator(sf))
        tok_lbl = TK_PUNC;
    else if (tok_val = identifier(sf)) {
        if (keyword(tok_val))
            tok_lbl = TK_KW;
        else { 
            tok_lbl = TK_IDNT;
            unsigned hs = hash_djb2(tok_val);
            sb_t[hs] = add_entry_symb_tb(sb_t, tok_val, hs);
        }
    }
    else
        error_tok(*sf, ERR, "invalid token");
    
    return (token) {tok_lbl, sf->filename, sf->line, sf->col, tok_val};
}