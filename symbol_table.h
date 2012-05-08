#ifndef _SYMBOL_TABLE_H_
#define _SYMBOL_TABLE_H_

#include "common.h"

#define NR_SYMBOL_TABLE 0x4000

typedef struct {
    Type *type;      
    int tmp_var_id; //for intermediate representation generating
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
    int depth;
} symbol_node;

typedef struct hash_node_ {
    symbol_node *data;
    struct hash_node_ *prev, *next, *same_scope_next;
} hash_node;

hash_node symbol_table[NR_SYMBOL_TABLE];

int scope_depth;

#define MAX_SCOPE_DEPTH 20

hash_node scope_stack[MAX_SCOPE_DEPTH];

typedef struct type_list_ {
    Type *data;
    struct type_list_ *next;
} type_list;

type_list type_stack[MAX_SCOPE_DEPTH];

Type *p_int_type, *p_float_type;

unsigned int hash_pjw(char *name);
void init_symbol_table();
void enter_deeper_scope();
void exit_top_scope();
symbol_node *get_symbol(char *name);
void insert_symbol(symbol_node *p_symbol);

void insert_type(Type *data);

#endif
