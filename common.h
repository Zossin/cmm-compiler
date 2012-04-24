#ifndef _COMMON_H_
#define _COMMON_H_

#define TRUE 1
#define FALSE 0
#define SYMBOL_NAME_LEN 32

typedef enum { Int, Float } basic_type;

typedef struct Type_ Type;
typedef struct FieldList_ FieldList;

struct Type_ {
    enum { Basic, Array, Structure } kind;
    union {
        basic_type basic;
        struct {
            Type *elem;
            int size;
        } array;
        FieldList *structure;
    } u;
};

struct FieldList_ {
    char id[SYMBOL_NAME_LEN];
    Type *type;
    FieldList *next;
};

typedef struct arg_node_ {
    Type *type;
    char id[SYMBOL_NAME_LEN];
    struct arg_node_ *next;
} arg_node;

unsigned char is_error_happened;

#endif
