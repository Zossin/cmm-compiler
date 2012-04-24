#ifndef _COMMON_H_
#define _COMMON_H_

#define TRUE 1
#define FALSE 0

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
    char *name;
    Type *type;
    FieldList *next;
};

unsigned char is_error_happened;

#endif
