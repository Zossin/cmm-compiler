#include "symbol_table.h"
#include "syntax_tree_node.h"
#include <stdlib.h>
#include <malloc.h>

unsigned int hash_pjw(char *name) {
    unsigned int val = 0, i;
    for (; *name; ++ name) {
        val = (val << 2) + *name;
        if (i = val & ~0x3fff) val = (val ^ (i >> 2)) & 0x3fff;
    }
    return val;
}

symbol_node *get_symbol(char *name) {
    int index = hash_pjw(name);
    hash_node *p_node = symbol_table[index].head;
    if (p_node == NULL)
        return NULL;
    while (p_node) {
        if (strcmp(name, p_node->data.key) == 0)
            return &(p_node->data);
        p_node = p_node->next;
    }
    return NULL;
}

void insert_symbol(symbol_node *p_symbol) {
    int index = hash_pjw(p_symbol->key);
    hash_node *new_node = (hash_node*)malloc(sizeof(hash_node));
    new_node->data = *p_symbol;
    new_node->next = symbol_table[index].head;
    symbol_table[index].head = new_node;
}

#define NR_CHILDREN 4

void sdt(syntax_tree_node *p_node) {
    syntax_tree_node *children[NR_CHILDREN];
    syntax_tree_node *child = p_node->lchild;
    int child_num = 0;
    while (child) {
        children[child_num ++] = child;
        child = child->next_sibling;
    }

    if (p_node->type == ExtDef_SYNTAX) {
        if (children[0]->type == Specifier_SYNTAX && children[1]->type == ExtDecList_SYNTAX && children[2]->type == SEMI_TOKEN) {
            sdt(children[0]);
            children[1]->attr.inh_type = children[0]->attr.type;
            sdt(children[1]);
        }
        else if (children[0]->type == Specifier_SYNTAX && children[1]->type == SEMI_TOKEN) { 

        }
        else if (children[0]->type == Specifier_SYNTAX && children[1]->type == FunDec_SYNTAX && children[2]->type == CompSt_SYNTAX) {

        }
    }
    else if (p_node->type == Def_SYNTAX) {

    }
    else if (p_node->type == Exp_SYNTAX) {

    }
    else if (p_node->type == Specifier_SYNTAX) {
        if (children[0]->type == TYPE_TOKEN) {
            p_node->attr.type->kind = Basic;
            p_node->attr.type->u.basic == children[0]->value.type_val;
        }
        else if (children[0]->type == StructSpecifier_SYNTAX) {

        }
    }
}
