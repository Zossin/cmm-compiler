#ifndef _SYNTAX_TREE_NODE_H_
#define _SYNTAX_TREE_NODE_H_

#include "common.h"

struct st_node;

typedef enum {
    INT_TOKEN, FLOAT_TOKEN, ID_TOKEN, SEMI_TOKEN, COMMA_TOKEN,
    ASSIGNOP_TOKEN, RELOP_TOKEN, PLUS_TOKEN, MINUS_TOKEN,
    STAR_TOKEN, DIV_TOKEN, AND_TOKEN, OR_TOKEN, DOT_TOKEN, NOT_TOKEN,
    TYPE_TOKEN, LP_TOKEN, RP_TOKEN, LB_TOKEN, RB_TOKEN, LC_TOKEN, RC_TOKEN,
    STRUCT_TOKEN, RETURN_TOKEN, IF_TOKEN, ELSE_TOKEN, WHILE_TOKEN,
    Program_SYNTAX, ExtDefList_SYNTAX, ExtDef_SYNTAX, ExtDecList_SYNTAX,
    Specifier_SYNTAX, StructSpecifier_SYNTAX, OptTag_SYNTAX, Tag_SYNTAX,
    VarDec_SYNTAX, FunDec_SYNTAX, VarList_SYNTAX, ParamDec_SYNTAX,
    CompSt_SYNTAX, StmtList_SYNTAX, Stmt_SYNTAX,
    DefList_SYNTAX, Def_SYNTAX, DecList_SYNTAX, Dec_SYNTAX,
    Exp_SYNTAX, Args_SYNTAX
} node_type;

typedef struct {
    Type *inh_type;
    Type *type;
    unsigned char is_in_struct;
    FieldList *structure;
    unsigned char is_legal;
    arg_node *args, *inh_args;
    char *id;
    Type *ret_type;
    unsigned char is_definition;
    unsigned char is_declared;
} attribute;

typedef struct st_node {
    char *name;
    node_type type;
    struct st_node *lchild, *next_sibling;
    unsigned char is_token;
    union {
        int int_val;
        float float_val;
        char *str_val;
        basic_type type_val;
    } value; 
    int lineno;
    attribute attr;
} syntax_tree_node;

syntax_tree_node *create_syntax_tree_node(char *name, node_type type, int child_num, ...);
void dfs_syntax_tree(int num_of_spaces, syntax_tree_node *p_node);
void destory_syntax_tree(syntax_tree_node *p_node);

syntax_tree_node *syntax_tree_root;

#endif
