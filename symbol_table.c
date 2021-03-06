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
    scope_depth = -1;
    int i;
    for (i = 0; i < NR_SYMBOL_TABLE; ++ i) {
        symbol_table[i].data = NULL;
        symbol_table[i].next = symbol_table[i].prev =  NULL;
    }
}

void add_read_write_func() {
    symbol_node *read_func = (symbol_node*)malloc(sizeof(symbol_node));
    strcpy(read_func->key, "read");
    read_func->type = Func;
    read_func->u.func_val.ret_type = p_int_type;
    read_func->u.func_val.args = NULL;
    read_func->u.func_val.is_defined = TRUE;

    symbol_node *write_func = (symbol_node*)malloc(sizeof(symbol_node));
    strcpy(write_func->key, "write");
    write_func->type = Func;
    write_func->u.func_val.ret_type = p_int_type;
    write_func->u.func_val.args = (arg_node*)malloc(sizeof(arg_node));
    write_func->u.func_val.args->type = p_int_type;
    write_func->u.func_val.args->next = NULL;
    write_func->u.func_val.is_defined = TRUE;

    insert_symbol(read_func);
    insert_symbol(write_func);
}

void enter_deeper_scope() {
    ++ scope_depth;
    scope_stack[scope_depth].data = NULL;
    scope_stack[scope_depth].same_scope_next = NULL;
//    type_stack[scope_depth].data = NULL;
//    type_stack[scope_depth].next = NULL;
}

void exit_top_scope() {
    hash_node *p_node = scope_stack[scope_depth].same_scope_next;
    while (p_node) {
        p_node->prev->next = p_node->next;
        if (p_node->next)
            p_node->next->prev = p_node->prev;
        hash_node *tmp_node = p_node;
        if (tmp_node->data->type == Func) {
            arg_node *arg = tmp_node->data->u.func_val.args;
            while (arg) {
                arg_node *tmp_arg = arg;
                arg = arg->next;
                free(tmp_arg);
            }
        }
        p_node = p_node->same_scope_next;
        free(tmp_node->data);
        free(tmp_node);
    }

    /*
    type_list *p_type = type_stack[scope_depth].next;
    while (p_type) {
        type_list *tmp_type = p_type;
        p_type = p_type->next;
        if (tmp_type->data->kind == Structure) {
            FieldList *field = tmp_type->data->u.structure;
            while (field) {
                FieldList *tmp_field = field;
                field = field->next;
                free(tmp_field);
            }
        }
        free(tmp_type->data);
        free(tmp_type);
    }
    */

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

/*
void insert_type(Type *data) {
    type_list *new_type = (type_list*)malloc(sizeof(type_list));
    new_type->data = data;
    new_type->next = type_stack[scope_depth].next;
    type_stack[scope_depth].next = new_type;
}
*/

