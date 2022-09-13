#include "token.h"
#include "symbol_tb.h"


int main(int argc, char **argv) {
    FILE *sf = fopen(argv[1], "r");
    file_d f = {sf, argv[1], 1, 1};
    circ_buf c_buf = init_circ_buffer();
    symb_tb sb_t[BUCKET_SIZE] = {NULL};
    fill_circ_buffer(&f, &c_buf, 0);
    token tok;
    bool prev_is_wtsp_eq = false;
    do {
        tok = tokenize(&f, &c_buf, sb_t, &prev_is_wtsp_eq);
    } while (c_buf.cread_p->c != EOF);
    // token tok;
    // do {
    //     tok = tokenize(&sf, sb_t);
    // } while (*sf.cnt != '\0');
    // display_symb_tb(sb_t);
    free(c_buf.cnt);
    return 0;
}

circ_buf init_circ_buffer() {
    circ_buf_d *cnt = calloc(CIRC_BUF_LEN, sizeof *cnt);
    circ_buf_d *cread_p = cnt;
    return (circ_buf) {cread_p, cnt};
}

void fill_circ_buffer(file_d *f, circ_buf *c_buf, uint8_t offset) {
    bool prev_is_wtsp = (f->col == 1) ? false:
     IS_WHITESPACE(c_buf->cnt[CIRC_BUF_LEN - 1].c) ? true: false;
    for (uint8_t i = offset; i < CIRC_BUF_LEN; i++) {
        if (i > 0)
            prev_is_wtsp = IS_WHITESPACE(c_buf->cnt[i - 1].c) ? true: false;
        c_buf->cnt[i] = skip_whitespace(f, prev_is_wtsp);
        if (c_buf->cnt[i].c == EOF)
            break;
    }
    c_buf->cread_p = c_buf->cnt;
}

circ_buf_d skip_whitespace(file_d *f, bool prev_is_wtsp) {
    char c = getc(f->sf);
    while (IS_WHITESPACE(c) && prev_is_wtsp) {
        len_skipped(f, c);
        c = getc(f->sf);
    }
    len_skipped(f, c);
    return (circ_buf_d) {c, f->line, f->col};
}

void len_skipped(file_d *f, char c) {
    unsigned len = 0;
    len = (c == '\t' || c == '\v') ? 4: 1;
    if (c == '\n') {
        f->line++;
        f->col = 1;
    }
    else
        f->col += len;
}

void peek(file_d *f, circ_buf *c_buf) {
    uint8_t shift = c_buf->cread_p - c_buf->cnt;
    // shift the bytes after cread_p to the beginning
    for (uint8_t i = shift, k = 0; i < CIRC_BUF_LEN; i++, k++)
        c_buf->cnt[k] = c_buf->cnt[i];
    // fill the remaining bytes
    fill_circ_buffer(f, c_buf, CIRC_BUF_LEN - shift);
    c_buf->cread_p = c_buf->cnt;
} 


// void error_tok(str_file sf, err_kind kind, char *msg) {
//     fprintf(stderr, BOLD "%s" RES ":%u:%u: %s: %s", 
//         sf.filename, sf.line, sf.col,
//         kind == WARN ? WARN_CLR "warning" RES : ERR_CLR "error" RES,
//         msg);
//     char* err_l = err_line(sf);
//     fprintf(stderr, "\n%u |\t%s\n  |\t", sf.line, err_l);
//     for (uint8_t i = 0; *(err_l + i) != '\0'; i++) {
//         if (i > sf.col && isspace(*(err_l + i)))
//             break;
//         fprintf(stderr, i < sf.col ? " ":
//                         i == sf.col ? 
//                             ((kind == WARN) ? WARN_CLR "^" RES: ERR_CLR "^" RES):
//                             ((kind == WARN) ? WARN_CLR "~" RES: ERR_CLR "~" RES));
//     }
//     fprintf(stderr, "\n");
//     if (kind == ERR)
//         exit(1);
// }

// char *err_line(str_file sf) {
//     char *end_l = strchr(sf.cnt, '\n');
//     unsigned final_len = end_l - (sf.cnt - sf.col);
//     char *buffer = malloc(final_len);
//     memset(buffer, '\0', final_len);
//     strncpy(buffer, sf.cnt - sf.col, final_len);
//     return buffer;
// }

// // check if the current token is a punctuator; if it's, it returns it
bool compounded_strcmp(circ_buf_d *cnt, char *s) {
    for (uint8_t i = 0; i < strlen(s) + 1; i++) {
        if (cnt[i].c != *s)
            return false;
        s++;
    }
    return true;
}

char *str_copy(file_d *f, circ_buf *c_buf, unsigned len) {
    if (!len)
        return NULL;

    // index of cread_p
    uint8_t idx_cread_p = c_buf->cread_p - c_buf->cnt;
    // len + 1 to add end of string char
    char *buf = malloc(len + 1), *tmp = buf;
    memset(buf, '\0', len + 1);
    for (uint8_t i = idx_cread_p; i < idx_cread_p + len; i++, tmp++)
        *tmp = c_buf->cnt[i].c;

    if (idx_cread_p + len == CIRC_BUF_LEN)
        fill_circ_buffer(f, c_buf, 0);
    else
        c_buf->cread_p += len;

    return buf;
}

// As buffer content is an array of struct, comparison using strncmp is impossible
char *fill_buffer(file_d *f, circ_buf *c_buf, char *buf, unsigned len) {
    if (!buf)
        return str_copy(f, c_buf, len);

    char *tmp_buf = str_copy(f, c_buf, len);
    if (!tmp_buf)
        return buf;
    char *result = malloc(strlen(buf) + strlen(tmp_buf) + 1);
    strcpy(result, buf);
    strcat(result, tmp_buf);
    return result;
}



void circ_buf_display_cnt(circ_buf_d *cnt, unsigned len) {
   printf("'");
    for (uint8_t i = 0; i < len; i++)
        printf("%c", cnt[i].c);
    printf("'\n");
}


char *number(file_d *f, circ_buf *c_buf, bool prev_is_wtsp_eq) {
    /* all base 10 floats are supported */
    if (isdigit(c_buf->cread_p->c) || (c_buf->cread_p->c == '.' && prev_is_wtsp_eq)
        || c_buf->cread_p->c == '-' && prev_is_wtsp_eq) {
        circ_buf_d *tmp = c_buf->cread_p + 1;
        char *buf = NULL;
        unsigned len = 1;
        bool is_float = false;
        while (isdigit(tmp->c)) {
            UNDEFINED_SIZE_TOK(f, c_buf, buf, tmp, len);
            if (tmp->c == '.') {
                // float cannot have mutliple floating points 
                // if (is_float)
                //     error_tok(*sf, WARN, "Bad number syntax");

                is_float = true;
                tmp++;
                len++;
            }
        } ;
        buf = fill_buffer(f, c_buf, buf, len);
        return buf;
    }
    return NULL;
}

char *literal(file_d *f, circ_buf *c_buf) {
    if (c_buf->cread_p->c == '\"' || c_buf->cread_p->c  == '\'') {
        char delim = c_buf->cread_p->c;
        char *buf = NULL;
        if (delim =='\"') {
            unsigned len = 0;
            // do not consider the quotes
            CREAD_P_FORWARD(f, c_buf)
            circ_buf_d *tmp = c_buf->cread_p;
            while (tmp->c != delim) {
                UNDEFINED_SIZE_TOK(f, c_buf, buf, tmp, len);
                // TODO: EOF
                if (tmp->c == EOF || tmp->c == '\n')
                    exit(1);
            }
            buf = fill_buffer(f, c_buf, buf, len);
            CREAD_P_FORWARD(f, c_buf);
            return buf;

        }
        else {
            // do not consider the quotes
            CREAD_P_FORWARD(f, c_buf)
            INSUFFICIENT_SPACE_PEEK(f, c_buf, 2);
            if ((c_buf->cread_p + 2)->c == delim && (int) (c_buf->cread_p + 1)->c > 0
                && (int) (c_buf->cread_p + 1)->c < 127 && (c_buf->cread_p + 1)->c != '\\') {
                buf = str_copy(f, c_buf, 1); // ascii char not escaped
                CREAD_P_FORWARD(f, c_buf);
                return buf;
            }

            INSUFFICIENT_SPACE_PEEK(f, c_buf, 3);
            if ((c_buf->cread_p + 3)->c == delim) 
                return escape_char(f, c_buf->cread_p);
        }
        // error_tok(*sf, WARN, "Not a char");
    }
    return NULL;
}

char *escape_char(file_d *f, circ_buf_d *cread_p) {
    char *esc_char = malloc(sizeof *esc_char);
    switch ((cread_p + 2)->c) {
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
            ; // error_tok(*sf, WARN, "Not an escape char");
    }
    cread_p += 3;
    return esc_char;
}

char *punctuator(file_d *f, circ_buf *c_buf) {
    for (uint8_t i = 0; i < PUNC_SIZE; i++) {
        // peek forward elements if the size of the punc's element exceeds the available reading space of the buffer
        INSUFFICIENT_SPACE_PEEK(f, c_buf, strlen(punc[i]));
        if (compounded_strcmp(c_buf->cread_p, punc[i])) {
            c_buf->cread_p += strlen(punc[i]);
            return punc[i];
        }
    }
    return NULL;
}

// check if the current token is an identifier; if it's, it returns it
char *identifier(file_d *f, circ_buf *c_buf) {
   // digits are allowed in identifier but not at the beginning
    if (!isalpha(c_buf->cread_p->c))  
        return NULL;
    unsigned len = 1;
    circ_buf_d *tmp = c_buf->cread_p + 1;
    char *buf = NULL;
    while (isalnum(tmp->c) || tmp->c == '_')
        UNDEFINED_SIZE_TOK(f, c_buf, buf, tmp, len);

    buf = fill_buffer(f, c_buf, buf, len);
    return buf;
}

// check if the given string is a keyword; if it's not, it's an identifier
bool keyword(char *s) {
    unsigned len = strlen(s);
    for (uint8_t i = 0; i < KW_SIZE; i++) {
        if (len > strlen(kw[i]))
            return false;
        if (!strncmp(s, kw[i], strlen(kw[i])))
            return true;
    }
    return false;
}

token tokenize(file_d *f, circ_buf *c_buf, symb_tb sb_t[], bool *is_wtsp_eq) {
    char *tok_val = NULL;
    token_kind tok_kind = -1;
    unsigned hs; // hash for the symbol table
    // if cread_p has reached the end of the buffer
    if (c_buf->cread_p - c_buf->cnt == CIRC_BUF_LEN - 1)
        fill_circ_buffer(f, c_buf, 0);
    
    // if there is a whitespace: skip
    if (IS_WHITESPACE(c_buf->cread_p->c)) {
        *is_wtsp_eq = true;
        c_buf->cread_p++;
    }

    // get token
    if ((tok_val = number(f, c_buf, *is_wtsp_eq)))
        tok_kind = TK_NB;
    else if ((tok_val = literal(f, c_buf)))
        tok_kind = TK_LIT;
    else if ((tok_val = punctuator(f, c_buf)))
        tok_kind = TK_PUNC;
    else if ((tok_val = identifier(f, c_buf))) {
        if (keyword(tok_val))
            tok_kind = TK_KW;
        else { 
            tok_kind = TK_IDNT;
            unsigned hs = hash_djb2(tok_val);
            sb_t[hs] = add_entry_symb_tb(sb_t, tok_val, hs);
        }
    }
    *is_wtsp_eq = false;
    if (!tok_val)
        exit(1);
    printf("%s\n", tok_val);
    return (token) {tok_kind, f->filename, c_buf->cread_p->line, c_buf->cread_p->col, tok_val};
}