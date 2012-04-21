#include <stdarg.h>
#include <malloc.h>
#include "syntax_tree_node.h"
#include <string.h>
#include <stdio.h>

extern int yylineno;

syntax_tree_node *create_syntax_tree_node(char *name, node_type type, int child_num, ...) {
    syntax_tree_node *p_node = (syntax_tree_node*)malloc(sizeof(syntax_tree_node));
    p_node->lchild = p_node->next_sibling = 0;

    va_list args;
    
    va_start(args, child_num);
    syntax_tree_node *tmp_node = 0;
    if (child_num > 0) {
        tmp_node = p_node->lchild = va_arg(args, syntax_tree_node*);
    }
    int i;
    for (i = 1; i < child_num; ++ i) {
        tmp_node = tmp_node->next_sibling = va_arg(args, syntax_tree_node*);
    }
    va_end(args);

    p_node->is_token = FALSE;
    p_node->name = name;
    p_node->type = type;
    if (child_num == 0) {
        p_node->lineno = yylineno;
    }
    else {
        p_node->lineno = p_node->lchild->lineno;
    }

    return p_node;
}

void dfs_syntax_tree(int num_of_spaces, syntax_tree_node *p_node) {
    if (p_node == 0)
        return;
    if (!p_node->is_token && p_node->lchild == 0) {
        dfs_syntax_tree(num_of_spaces, p_node->next_sibling);
        return;
    }
    int i;
    for (i = 0; i < num_of_spaces; ++ i) {
        fprintf(stdout, " ");
    }
    if (p_node->is_token) {
        if (strcmp(p_node->name, "INT") == 0) {
            fprintf(stdout, "%s: %d\n", p_node->name, p_node->value.int_val);
        }
        else if (strcmp(p_node->name, "FLOAT") == 0) {
            fprintf(stdout, "%s: %f\n", p_node->name, p_node->value.float_val);
        }
        else if (strcmp(p_node->name, "ID") == 0) {
            fprintf(stdout, "%s: %s\n", p_node->name, p_node->value.str_val);
        }
        else if (strcmp(p_node->name, "TYPE") == 0) {
            if (p_node->value.type_val == Int)
                fprintf(stdout, "%s: int\n", p_node->name);
            if (p_node->value.type_val == Float)
                fprintf(stdout, "%s: float\n", p_node->name);
        }
        else {
            fprintf(stdout, "%s\n", p_node->name);
        }
    }
    else {
        fprintf(stdout, "%s (%d)\n", p_node->name, p_node->lineno);
        dfs_syntax_tree(num_of_spaces + 2, p_node->lchild);
    }
    dfs_syntax_tree(num_of_spaces, p_node->next_sibling);
}

void destory_syntax_tree(syntax_tree_node *p_node) {
    if (p_node == 0)
        return;
    destory_syntax_tree(p_node->lchild);
    destory_syntax_tree(p_node->next_sibling);
    free(p_node);
}
