#ifndef _SYMBOL_TABLE_H_
#define _SYMBOL_TABLE_H_

#include "common.h"

#define NR_SYMBOL_TABLE 0x4000

typedef struct {
    Type *type;      
} var_symbol;

typedef struct {
    FieldList *structure;
} struct_symbol;

typedef struct {
    int argc;
    arg_node *args;
    Type *ret_type;
} func_symbol;

typedef struct {
    char key[SYMBOL_NAME_LEN];
    enum { Var, Struct, Func } type;
    union {
        var_symbol var_val;
        struct_symbol struct_val;
        func_symbol func_val;
    } u;
} symbol_node;

typedef struct hash_node_ {
    symbol_node *data;
    struct hash_node_ *next;
} hash_node;

typedef struct {
    hash_node *head;
} hash_head;

hash_head symbol_table[NR_SYMBOL_TABLE];

unsigned int hash_pjw(char *name);

#endif
