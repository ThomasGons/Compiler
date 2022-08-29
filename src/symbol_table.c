#include "../inc/symbol_tb.h"

unsigned hash_djb2(char *s) {
    unsigned long hs = 5381, i;
    while (i = *s++)
        hs = ((hs << 5) + hs + i);
    return hs % BUCKET_SIZE;
}

symb_tb add_entry_symb_tb(symb_tb sb_t[], char *idnt_name, unsigned hs) {
    entry *new_entry = malloc(sizeof *new_entry);
    if (!new_entry)
        exit(1);
    *new_entry = (entry) {.data.name = idnt_name, NULL};
    if (!sb_t[hs]) 
        return new_entry;
    if (!strcmp(sb_t[hs]->data.name, idnt_name))
        return sb_t[hs];
    
    symb_tb tmp = sb_t[hs];
    while (tmp->next) {
        if (!strcmp(tmp->data.name, idnt_name))
            return sb_t[hs];
        tmp = tmp->next;
    }
    tmp->next = new_entry;
    return sb_t[hs];
}

void display_symb_tb(symb_tb sb_t[]) {
    symb_tb p = sb_t[0];
    int uni = 0, duo = 0, tri = 0, quad = 0, more = 0;
    unsigned count;
    for (unsigned i = 0; i < BUCKET_SIZE; i++) {
        printf("%d --> ", i);
        for (p = sb_t[i], count = 1; p; p = p->next, count++)
            printf("%s, ", p->data.name);
        printf("\n");
    }
}