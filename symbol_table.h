#ifndef _SYMBOL_TABLE_H_
#define _SYMBOL_TABLE_H_

#define NR_SYMBOL_TABLE 0x4000
#define SYMBOL_NAME_LEN 32

typedef struct Type_ Type;
typedef struct FieldList_ FieldList;

struct Type_ {
    enum { Basic, Array, Structure } kind;
    union {
        int basic;
        struct {
            Type *elem;
            int size;
        } array;
        FieldList *structure;
    } u;
};

struct FieldList_ {
    char *name;
    Type *type;
    FieldList *next;
};

typedef struct arg_node_ {
    Type *type;
    struct arg_node_ *next;
} arg_node;

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
    symbol_node data;
    struct hash_node_ *next;
} hash_node;

typedef struct {
    hash_node *ptr;
} hash_head;

hash_head symbol_table[NR_SYMBOL_TABLE];

unsigned int hash_pjw(char *name);

#endif
