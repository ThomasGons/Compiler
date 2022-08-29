#include "../inc/token.h"
#include "../inc/symbol_tb.h"


int main(int argc, char **argv) {
    // ./bin/exe <filename>
    if (argc != 2)
        ERROR("Too many arguments");    
    str_file s = read_file (argv[1]);
    symb_tb sb_t[BUCKET_SIZE] = {NULL};
    tokenize(s, sb_t);
    display_symb_tb(sb_t);
    return 0;
}

// The content of the file is retrieved and stored in a string
static str_file read_file(char *fname) {
    FILE * fp = fopen (fname, "r");
    if (!fp)
        ERROR("File could not be opened");

    // calculation of the number of characters in the file
    fseek(fp, 0, SEEK_END);
    long length = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    // buffer that will store the content of the file
    char *buffer = malloc (sizeof(char) *length);
    if (!buffer)
        ERROR("Couldn't allocate memory for the buffer");
    fread (buffer, 1, length, fp);
    fclose (fp);
    return (str_file) {fname, buffer, 1, 0};
}


/* static void error_tok(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
} */

/* TO DO: punctuators should be invalid when there are a repetition
    of undesired punctuator like "+++" or "===" */
// check if the current token is a punctuator; if it's, it returns it
char *punctuator(char *s) {
    for (uint8_t i = 0; i < 48; i++) {
        if (!strncmp(s, punc[i], strlen(punc[i])))
            return punc[i];
    }
    return NULL;
}

static char *str_copy(char *s, unsigned len) {
    if (!len)
        return NULL;

    // copy of the str;
    char *buffer = malloc(len);
    memset(buffer, '\0', len);
    strncpy(buffer, s, len);
    return buffer;
}


static char *literal(char *s, bool *is_escape) {
    if (*s == '"') {    // simple quote case remaining 
        *is_escape = false;
        unsigned len = 1;
        for (char *tmp = s + 1; *tmp != *s; *tmp++)
            len++;
        return str_copy(s + 1, len - 1); // remove quotes
    }
    else if (*s == '\'') {
        if (*(s + 2) == *s && (int) *(s + 1) < 127 && *(s + 1) != '\\') {
            is_escape = false;
            return str_copy(s + 1, 1); // ascii char not escaped
        }
        else if (*(s + 3) == *s) {
            switch (*(s + 2)) {
                case 'a': return "\a";
                case 'b': return "\b";
                case 'f': return "\f";
                case 'n': return "\n";
                case 'r': return "\r";
                case 't': return "\t";
                case 'v': return "\v";
                case '\\': return "\\";
                case '\'': return "\'";
                case '"': return "\"";
                case '0': return "\0";
                default: ERROR("not a char");
            }
        }
        ERROR("not a char");
    }
    return NULL;
}

// check if the given string is a keyword; if it's not, it's an identifier
static bool keyword(char *s) {
    for (uint8_t i = 0; i < 32; i++) {
        if (!strncmp(s, kw[i], strlen(kw[i])))
            return true;
    }
    return false;
}

// check if the current token is an identifier; if it's, it returns it
static char *identifier(char *s) {
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
    return str_copy(s, len);
}

static char *number(char *s) {
    /* all base 10 floats are supported */
    if (isdigit(*s) || (*s == '.' && (*(s - 1) == ' ' || *(s - 1) == '='))  
        || (*s == '-' && (*(s - 1) == ' ' || *(s - 1) == '='))) {
        char *tmp = s;
        unsigned len = 0;
        bool is_float = false;
        do {
            tmp++;
            len++;
            if (*tmp == '.') {
                // float cannot have mutliple floating points 
                if (is_float)
                    ERROR("Bad syntax for a number");
                is_float = true;
                tmp++;
                len++;
            }
        } while (isdigit(*tmp));
        return str_copy(s, len);
    }
    return NULL;
}

/* TO DO: the structure of the function will change because we want a stream of tokens.
 Each token once obtained must be returned to the parser so the previous token
 is overwritten by the new one  */ 
extern token tokenize(str_file fs, symb_tb sb_t[]) {
    token tok;
    // void pointer to retrieved the token value regardless of its type
    char *tok_val = NULL;
    unsigned hs; // hash for the symbol table
    do {
        /* It is important to note that the directives and comments of the preprocessor
         are absent at this stage in this phase because they have been processed and deleted
        by the previous phase: the preprocessing */

        // when any type of whitespace is encountered, just continue
        if (isspace(*fs.cnt) || *fs.cnt == '\a' || *fs.cnt == '\b') {
            // line and column are updated if it's a new line
            if (*fs.cnt == '\n') {
                fs.line++;
                fs.col = 0;
            }
            fs.cnt++;
            continue;
        }
        if (tok_val = number(fs.cnt)) {
            tok = (token) {TK_NB, fs.filename, fs.line, fs.col, tok_val};
            fs.col++;
            fs.cnt += strlen(tok_val);
            continue;
        }
        if (tok_val = punctuator(fs.cnt)) {
            tok = (token) {TK_PUNC, fs.filename, fs.line, fs.col, tok_val};
            fs.col++;
            // increment pointer address according to the length of the token found
            fs.cnt += strlen(tok_val);
            continue;
        }
        bool is_escape = true;
        if (tok_val = literal(fs.cnt, &is_escape)) {
            tok = (token) {TK_LIT, fs.filename, fs.line, fs.col, tok_val};
            fs.col++;
            fs.cnt += (strlen(tok_val) + 2 + is_escape);  // add quotes that are not present
            continue;
        }
        if (tok_val = identifier(fs.cnt)) {
            // an identifier can be a keyword
            bool is_keyword = keyword(tok_val);
            tok = (token) { is_keyword ? TK_KW: TK_IDNT, fs.filename, fs.line,
                fs.col, tok_val};
            if (!is_keyword) {
                unsigned hs = hash_djb2(tok_val);
                sb_t[hs] = add_entry_symb_tb(sb_t, tok_val, hs);
            }
            fs.col++;
            fs.cnt += strlen(tok_val);
            continue;
        }
        /* later it will be necessary to return an error for an invalid token
            but for the moment not all types of tokens are implemented. */
        *fs.cnt++;
    // don't forget that it's a string;
    } while (*fs.cnt != '\0');
}