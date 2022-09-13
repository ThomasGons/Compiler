#include "../inc/symbol_tb.h"

unsigned hash_djb2(char *s) {
    unsigned long hs = 5381, i;
    while ((i = *s++))
        hs += ((hs << 5) + i);
    return hs % BUCKET_SIZE;
}

symb_tb add_entry_symb_tb(symb_tb sb_t[], char *idnt_name, unsigned hs) {
    entry *new_entry = malloc(sizeof *new_entry);
    if (!new_entry) {
        perror("Error"); exit(1);
    }
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

bool lookup_symb_tb(symb_tb sb_t[], char* s) {
    unsigned hs = hash_djb2(s);
    if (!sb_t[hs])
        return false;
    symb_tb tmp = sb_t[hs];
    while (tmp->next) {
        if (!strcmp(tmp->data.name, s))
            return true;
        tmp = tmp->next;
    }
    return false;
}

void display_symb_tb(symb_tb sb_t[]) {
    symb_tb p = sb_t[0];
    for (unsigned i = 0; i < BUCKET_SIZE; i++) {
        printf("%d --> ", i);
        for (p = sb_t[i]; p; p = p->next)
            printf("%s, ", p->data.name);
        printf("\n");
    }
}
