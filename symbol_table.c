#include "symbol_table.h"
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

unsigned int hash_pjw(char *name) {
    unsigned int val = 0, i;
    for (; *name; ++ name) {
        val = (val << 2) + *name;
        if (i = val & ~0x3fff) val = (val ^ (i >> 2)) & 0x3fff;
    }
    return val;
}

void init_symbol_table() {
    scope_depth = 0;
    scope_stack[scope_depth].data = NULL;
    scope_stack[scope_depth].same_scope_next = NULL;
    int i;
    for (i = 0; i < NR_SYMBOL_TABLE; ++ i) {
        symbol_table[i].data = NULL;
        symbol_table[i].next = symbol_table[i].prev =  NULL;
    }
}

void enter_deeper_scope() {
    ++ scope_depth;
    scope_stack[scope_depth].data = NULL;
    scope_stack[scope_depth].same_scope_next = NULL;
}

void exit_top_scope() {
    hash_node *p_node = scope_stack[scope_depth].same_scope_next;
    while (p_node) {
        p_node->prev->next = p_node->next;
        if (p_node->next)
            p_node->next->prev = p_node->prev;
        p_node = p_node->same_scope_next;
    }
    --scope_depth;
}

symbol_node *get_symbol(char *name) {
    int index = hash_pjw(name);
    hash_node *p_node = symbol_table[index].next;
    while (p_node) {
        if (strcmp(name, p_node->data->key) == 0)
            return p_node->data;
        p_node = p_node->next;
    }
    return NULL;
}

void insert_symbol(symbol_node *p_symbol) {
    int index = hash_pjw(p_symbol->key);
    hash_node *new_node = (hash_node*)malloc(sizeof(hash_node));
    p_symbol->depth = scope_depth;
    new_node->data = p_symbol;
    new_node->next = symbol_table[index].next;
    new_node->prev = &(symbol_table[index]);
    if (symbol_table[index].next)
        symbol_table[index].next->prev = new_node;
    symbol_table[index].next = new_node;
    new_node->same_scope_next = scope_stack[scope_depth].same_scope_next;
    scope_stack[scope_depth].same_scope_next = new_node;
}


