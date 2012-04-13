#ifndef _SYNTAX_TREE_NODE_H_
#define _SYNTAX_NODE_TREE_H_

struct st_node;

typedef struct st_node {
    char *name;
    struct st_node *lchild, *next_sibling;
    unsigned char is_token;
    union {
        int int_val;
        float float_val;
        char *str_val;
        enum {Int, Float} type_val;
    } value; 
    int lineno;
} syntax_tree_node;

syntax_tree_node *create_syntax_tree_node(char *name, int child_num, ...);
void dfs_syntax_tree(int num_of_spaces, syntax_tree_node *p_node);
void destory_syntax_tree(syntax_tree_node *p_node);

#define TRUE 1
#define FALSE 0

#endif
