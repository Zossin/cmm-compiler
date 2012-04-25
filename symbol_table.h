#ifndef _SYMBOL_TABLE_H_
#define _SYMBOL_TABLE_H_

#include "common.h"

#define NR_SYMBOL_TABLE 0x4000

typedef struct {
    Type *type;      
} var_symbol;

typedef struct {
    Type *structure;
} struct_symbol;

typedef struct {
    arg_node *args;
    Type *ret_type;
    unsigned char is_defined;
    int lineno;
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
    struct hash_node_ *prev, *next, *same_scope_next;
    int depth;
} hash_node;

hash_node symbol_table[NR_SYMBOL_TABLE];

int scope_depth;

#define MAX_SCOPE_DEPTH 20

hash_node scope_stack[MAX_SCOPE_DEPTH];

unsigned int hash_pjw(char *name);

#endif
