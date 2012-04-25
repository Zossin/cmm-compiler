#include "symbol_table.h"
#include "syntax_tree_node.h"
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

unsigned int hash_pjw(char *name) {
    unsigned int val = 0, i;
    for (; *name; ++ name) {
        val = (val << 2) + *name;
        if (i = val & ~0x3fff) val = (val ^ (i >> 2)) & 0x3fff;
    }
    return val;
}

void print_error(int type, int lineno, char *tips) {
    is_error_happened = TRUE;
    fprintf(stdout, "Error type %d at line %d: %s\n", type, lineno, tips); 
}

unsigned char is_same_type(Type *a, Type *b) {
    if (a->kind != b->kind)
        return FALSE;
    if (a->kind == Basic)
        return a->u.basic == b->u.basic;
    else if (a->kind == Array)
        return is_same_type(a->u.array.elem, b->u.array.elem);
    else { //Structure
        FieldList *a_field = a->u.structure, *b_field = b->u.structure;
        while (a_field && b_field) {
            if (!is_same_type(a_field->type, b_field->type))
                return FALSE;
            a_field = a_field->next;
            b_field = b_field->next;
        }
        if (a_field || b_field)
            return FALSE;
        return TRUE;
    }
}

symbol_node *get_symbol(char *name) {
    int index = hash_pjw(name);
    hash_node *p_node = symbol_table[index].head;
    if (p_node == NULL)
        return NULL;
    while (p_node) {
        if (strcmp(name, p_node->data->key) == 0)
            return p_node->data;
        p_node = p_node->next;
    }
    return NULL;
}

void insert_symbol(symbol_node *p_symbol) {
    int index = hash_pjw(p_symbol->key);
    hash_node *new_node = (hash_node*)malloc(sizeof(hash_node));
    new_node->data = p_symbol;
    new_node->next = symbol_table[index].head;
    symbol_table[index].head = new_node;
}

void check_undef_func() {
    int i;
    for (i = 0; i < NR_SYMBOL_TABLE; ++ i) {
        hash_node *p_node = symbol_table[i].head;
        while (p_node) {
            if (p_node->data->type == Func) {
                if (!p_node->data->u.func_val.is_defined)
                    print_error(18, p_node->data->u.func_val.lineno, "Function undefined");
            } 
            p_node = p_node->next;
        }
    }
}

extern void sdt(syntax_tree_node *p_node);

void check_type_match(syntax_tree_node *p_node, syntax_tree_node **children) {
    sdt(children[0]);
    sdt(children[2]);
    if (children[0]->attr.is_legal && children[2]->attr.is_legal) {
        if (is_same_type(children[0]->attr.type, children[2]->attr.type)) {
            p_node->attr.type = children[0]->attr.type;
            p_node->attr.is_legal = TRUE;
        }
        else {
            print_error(7, children[1]->lineno, "Type mismatched");
            p_node->attr.is_legal = FALSE;
        }
    }
    else
        p_node->attr.is_legal = FALSE;
}

#define NR_CHILDREN 8

void sdt(syntax_tree_node *p_node) {
    if (p_node == NULL)
        return;

    syntax_tree_node *children[NR_CHILDREN];
    syntax_tree_node *child = p_node->lchild;
    int child_num = 0;
    while (child) {
        children[child_num ++] = child;
        child = child->next_sibling;
    }

    if (p_node->type == ExtDef_SYNTAX) {
        if (child_num == 3 && children[0]->type == Specifier_SYNTAX && children[1]->type == ExtDecList_SYNTAX && children[2]->type == SEMI_TOKEN) {
            sdt(children[0]);
            children[1]->attr.inh_type = children[0]->attr.type;
            sdt(children[1]);
        }
        else if (child_num == 2 && children[0]->type == Specifier_SYNTAX && children[1]->type == SEMI_TOKEN) { 
            sdt(children[0]);
        }
        else if (child_num == 3 && children[0]->type == Specifier_SYNTAX && children[1]->type == FunDec_SYNTAX && children[2]->type == SEMI_TOKEN) {
            sdt(children[0]);
            children[1]->attr.inh_type = children[0]->attr.type;
            children[1]->attr.is_definition = FALSE;
            sdt(children[1]);
        }
        else if (child_num == 3 && children[0]->type == Specifier_SYNTAX && children[1]->type == FunDec_SYNTAX && children[2]->type == CompSt_SYNTAX) {
            sdt(children[0]);
            children[1]->attr.inh_type = children[0]->attr.type;
            children[1]->attr.is_definition = TRUE;
            sdt(children[1]);
            children[2]->attr.ret_type = children[0]->attr.type;
            sdt(children[2]);
        }
    }
    else if (p_node->type == Def_SYNTAX) {
        if (child_num == 3 && children[0]->type == Specifier_SYNTAX && children[1]->type == DecList_SYNTAX && children[2]->type == SEMI_TOKEN) {
            sdt(children[0]);
            children[1]->attr.inh_type = children[0]->attr.type;
            children[1]->attr.is_in_struct = p_node->attr.is_in_struct;
            sdt(children[1]);
            if (p_node->attr.is_in_struct) {
                p_node->attr.structure = children[1]->attr.structure;
            }
        }
    }
    else if (p_node->type == DecList_SYNTAX) {
        children[0]->attr.is_in_struct = p_node->attr.is_in_struct;
        children[0]->attr.inh_type = p_node->attr.inh_type;
        sdt(children[0]);
        if (child_num == 1 && children[0]->type == Dec_SYNTAX) {

        }
        else if (child_num == 3 && children[0]->type == Dec_SYNTAX && children[1]->type == COMMA_TOKEN && children[2]->type == DecList_SYNTAX) {
            children[2]->attr.is_in_struct = p_node->attr.is_in_struct;
            children[2]->attr.inh_type = p_node->attr.inh_type;
            sdt(children[2]);
            children[0]->attr.structure->next = children[2]->attr.structure;
        }
        p_node->attr.structure = children[0]->attr.structure;
    }
    else if (p_node->type == Dec_SYNTAX) {
        children[0]->attr.is_in_struct = p_node->attr.is_in_struct;
        children[0]->attr.inh_type = p_node->attr.inh_type;
        sdt(children[0]);
        p_node->attr.structure = children[0]->attr.structure;
        if (child_num == 1 && children[0]->type == VarDec_SYNTAX) {

        }
        else if (child_num == 3 && children[0]->type == VarDec_SYNTAX && children[1]->type == ASSIGNOP_TOKEN && children[2]->type == Exp_SYNTAX) {
            sdt(children[2]);
            if (p_node->attr.is_in_struct) {
                print_error(15, children[1]->lineno, "Cannot initialize a field of structure");
                return;
            }
            if (children[2]->attr.is_legal) {
                if (is_same_type(children[0]->attr.type, children[2]->attr.type)) 
                    p_node->attr.type = children[0]->attr.type;
                else 
                    print_error(5, children[1]->lineno, "Type mismatched");
            }
        }
    }
    else if (p_node->type == Exp_SYNTAX) {
        if (child_num == 3 && children[0]->type == Exp_SYNTAX && children[1]->type == ASSIGNOP_TOKEN && children[2]->type == Exp_SYNTAX) {
            sdt(children[0]);
            sdt(children[2]);
            syntax_tree_node *grand_children[4], *grand_child = children[0]->lchild;
            int grand_child_num = 0;
            while (grand_child) {
                grand_children[grand_child_num ++] = grand_child;
                grand_child = grand_child->next_sibling;
            }
            if (!(grand_child_num == 1 && grand_children[0]->type == ID_TOKEN || grand_child_num == 4 && grand_children[0]->type == Exp_SYNTAX && grand_children[1]->type == LB_TOKEN && grand_children[2]->type == Exp_SYNTAX && grand_children[3]->type == RB_TOKEN || grand_child_num == 3 && grand_children[0]->type == Exp_SYNTAX && grand_children[1]->type == DOT_TOKEN && grand_children[2]->type == ID_TOKEN)) {
                print_error(6, children[0]->lineno, "Assign to right value");
                p_node->attr.is_legal = FALSE;
                return;
            }
            if (children[0]->attr.is_legal && children[2]->attr.is_legal) {
                if (is_same_type(children[0]->attr.type, children[2]->attr.type)) {
                    p_node->attr.type = children[0]->attr.type;
                    p_node->attr.is_legal = TRUE;
                }
                else {
                    print_error(5, children[1]->lineno, "Type mismatched");
                    p_node->attr.is_legal = FALSE;
                }
            }
            else
                p_node->attr.is_legal = FALSE;
        }
        else if (child_num == 3 && children[0]->type == Exp_SYNTAX && children[1]->type == AND_TOKEN && children[2]->type == Exp_SYNTAX) {
            check_type_match(p_node, children);
        }
        else if (child_num == 3 && children[0]->type == Exp_SYNTAX && children[1]->type == OR_TOKEN && children[2]->type == Exp_SYNTAX) {
            check_type_match(p_node, children);
        }
        else if (child_num == 3 && children[0]->type == Exp_SYNTAX && children[1]->type == RELOP_TOKEN && children[2]->type == Exp_SYNTAX) {
            check_type_match(p_node, children);
        }
        else if (child_num == 3 && children[0]->type == Exp_SYNTAX && children[1]->type == PLUS_TOKEN && children[2]->type == Exp_SYNTAX) {
            check_type_match(p_node, children);
        }
        else if (child_num == 3 && children[0]->type == Exp_SYNTAX && children[1]->type == MINUS_TOKEN && children[2]->type == Exp_SYNTAX) {
            check_type_match(p_node, children);
        }
        else if (child_num == 3 && children[0]->type == Exp_SYNTAX && children[1]->type == STAR_TOKEN && children[2]->type == Exp_SYNTAX) {
            check_type_match(p_node, children);
        }
        else if (child_num == 3 && children[0]->type == Exp_SYNTAX && children[1]->type == DIV_TOKEN && children[2]->type == Exp_SYNTAX) {
            check_type_match(p_node, children);
        }
        else if (child_num == 3 && children[0]->type == LP_TOKEN && children[1]->type == Exp_SYNTAX && children[2]->type == RP_TOKEN) {
            sdt(children[1]);
            p_node->attr.is_legal = children[1]->attr.is_legal;
            p_node->attr.type = children[1]->attr.type;
        }
        else if (child_num == 2 && children[0]->type == MINUS_TOKEN && children[1]->type == Exp_SYNTAX) {
            sdt(children[1]);
            p_node->attr.is_legal = children[1]->attr.is_legal;
            p_node->attr.type = children[1]->attr.type;
        }
        else if (child_num == 2 && children[0]->type == NOT_TOKEN && children[1]->type == Exp_SYNTAX) {
            sdt(children[1]);
            p_node->attr.is_legal = children[1]->attr.is_legal;
            p_node->attr.type = children[1]->attr.type;
        }
        else if (child_num == 4 && children[0]->type == ID_TOKEN && children[1]->type == LP_TOKEN && children[2]->type == Args_SYNTAX && children[3]->type == RP_TOKEN || child_num == 3 && children[0]->type == ID_TOKEN && children[1]->type == LP_TOKEN && children[2]->type == RP_TOKEN) {
            syntax_tree_node *args = (child_num == 4) ? children[2] : NULL;
            symbol_node *func_node;
            if (func_node = get_symbol(children[0]->value.str_val)) {
                if (func_node->type == Var) {
                    print_error(11, children[0]->lineno, "Use a variable as function.");
                    p_node->attr.is_legal = FALSE;
                }
                else  {
                    p_node->attr.type = func_node->u.func_val.ret_type;

                    if (args) {
                        args->attr.inh_args = func_node->u.func_val.args;
                        sdt(args);
                        p_node->attr.is_legal = args->attr.is_legal;
                    }
                    else {
                        if (func_node->u.func_val.args) {
                            print_error(9, p_node->lineno, "Argument number mismatched.");
                            p_node->attr.is_legal = FALSE;
                        }
                        else
                            p_node->attr.is_legal = TRUE;
                    }
                }
            }
            else {
                char tmp_str[100];
                strcpy(tmp_str, "Undefined function \"");
                print_error(2, children[0]->lineno, strcat(strcat(tmp_str, children[0]->value.str_val), "\""));
                p_node->attr.is_legal = FALSE;
            }
        }
        else if (child_num == 4 && children[0]->type == Exp_SYNTAX && children[1]->type == LB_TOKEN && children[2]->type == Exp_SYNTAX && children[3]->type == RB_TOKEN) {
            sdt(children[0]);
            if (!children[0]->attr.is_legal) {
                p_node->attr.is_legal = FALSE;
                return;
            }
            if (children[0]->attr.type->kind != Array) {
                print_error(10, children[0]->lineno, "Use a not-array variable as array");
                p_node->attr.is_legal = FALSE;
                return;
            }
            sdt(children[2]);
            if (!children[2]->attr.is_legal) {
                p_node->attr.is_legal = FALSE;
                return;
            }
            if (children[2]->attr.type->kind == Basic && children[2]->attr.type->u.basic == Int) {
                p_node->attr.type = children[0]->attr.type->u.array.elem;
                p_node->attr.is_legal = TRUE;
            }
            else {
                print_error(12, children[2]->lineno, "Array index is not integer");
                p_node->attr.is_legal = FALSE;
            }
        }
        else if (child_num == 3 && children[0]->type == Exp_SYNTAX && children[1]->type == DOT_TOKEN && children[2]->type == ID_TOKEN) {
            sdt(children[0]);
            if (children[0]->attr.type->kind != Structure) {
                print_error(13, children[1]->lineno, "Left side of DOT is not a structure");
                p_node->attr.is_legal = FALSE;
                return;
            }
            FieldList *field = children[0]->attr.type->u.structure;
            while (field) {
                if (strcmp(field->id, children[2]->value.str_val) == 0)
                    break;
                field = field->next;
            }

            if (field) {
                p_node->attr.type = field->type;
                p_node->attr.is_legal = TRUE;
            }
            else {
                print_error(14, children[2]->lineno, "Structure field undefined.");
                p_node->attr.is_legal = FALSE;
            }
        }
        else if (child_num == 1 && children[0]->type == ID_TOKEN) {
            symbol_node *var_node;
            if (var_node = get_symbol(children[0]->value.str_val)) {
                p_node->attr.type = var_node->u.var_val.type;
                p_node->attr.is_legal = TRUE;
            }
            else {
                char tmp_str[100];
                strcpy(tmp_str, "Undefined variable \"");
                print_error(1, children[0]->lineno, strcat(strcat(tmp_str, children[0]->value.str_val), "\""));
                p_node->attr.is_legal = FALSE;
            }
        }
        else if (child_num == 1 && children[0]->type == INT_TOKEN) {
            p_node->attr.type = (Type*)malloc(sizeof(Type));
            p_node->attr.type->kind = Basic;
            p_node->attr.type->u.basic = Int;
            p_node->attr.is_legal = TRUE;
        }
        else if (child_num == 1 && children[0]->type == FLOAT_TOKEN) {
            p_node->attr.type = (Type*)malloc(sizeof(Type));
            p_node->attr.type->kind = Basic;
            p_node->attr.type->u.basic = Float;
            p_node->attr.is_legal = TRUE;
        }
    }
    else if (p_node->type == Args_SYNTAX) {
        sdt(children[0]);
        if (p_node->attr.inh_args == NULL) {
            print_error(9, children[0]->lineno, "Argument number mismatched.");
            p_node->attr.is_legal = FALSE;
            return;
        }
        if (!is_same_type(p_node->attr.inh_args->type, children[0]->attr.type)) {
            print_error(9, children[0]->lineno, "Argument type mismatched.");
            p_node->attr.is_legal = FALSE;
            return;
        }
        
        if (child_num == 3 && children[0]->type == Exp_SYNTAX && children[1]->type == COMMA_TOKEN && children[2]->type == Args_SYNTAX) {
            children[2]->attr.inh_args = p_node->attr.inh_args->next;     
            sdt(children[2]);
            p_node->attr.is_legal = children[2]->attr.is_legal;
        }
        else if (child_num == 1 && children[0]->type == Exp_SYNTAX) {
            if (p_node->attr.inh_args->next) {
                print_error(9, p_node->lineno, "Argument number mismatched.");
                p_node->attr.is_legal = FALSE;
            }
            else
                p_node->attr.is_legal = TRUE;
        }
    }
    else if (p_node->type == Specifier_SYNTAX) {
        if (child_num == 1 && children[0]->type == TYPE_TOKEN) {
            p_node->attr.type = (Type*)malloc(sizeof(Type));
            p_node->attr.type->kind = Basic;
            p_node->attr.type->u.basic = children[0]->value.type_val;
            p_node->attr.is_legal = TRUE;
        }
        else if (child_num == 1 && children[0]->type == StructSpecifier_SYNTAX) {
            sdt(children[0]);
            p_node->attr.type = children[0]->attr.type;
            p_node->attr.is_legal = children[0]->attr.is_legal;
        }
    }
    else if (p_node->type == StructSpecifier_SYNTAX) {
        if (child_num == 5 && children[0]->type == STRUCT_TOKEN && children[1]->type == OptTag_SYNTAX && children[2]->type == LC_TOKEN && children[3]->type == DefList_SYNTAX && children[4]->type == RC_TOKEN) {
            if (get_symbol(children[1]->lchild->value.str_val) == NULL) {
                symbol_node *new_symbol = (symbol_node*)malloc(sizeof(symbol_node));
                strcpy(new_symbol->key, children[1]->lchild->value.str_val);
                new_symbol->type = Struct;
                insert_symbol(new_symbol);
                p_node->attr.type = (Type*)malloc(sizeof(Type));
                p_node->attr.type->kind = Structure;
                new_symbol->u.struct_val.structure = p_node->attr.type;
                
                children[3]->attr.is_in_struct = TRUE;
                sdt(children[3]);
                FieldList *field = children[3]->attr.structure;
                while (field) {
                    FieldList *pre_field = children[3]->attr.structure;
                    while (pre_field != field) {
                        if (strcmp(pre_field->id, field->id) == 0) {
                            print_error(15, field->lineno, "Field redefined.");
                            break;
                        }
                        pre_field = pre_field->next;
                    }

                    field = field->next;
                }
                p_node->attr.type->u.structure = children[3]->attr.structure;
                p_node->attr.is_legal = TRUE;
            }
            else {
                print_error(16, children[1]->lineno, "Structure redefined.");
                p_node->attr.is_legal = FALSE;
            }
        }
        else if (child_num == 2 && children[0]->type == STRUCT_TOKEN && children[1]->type == Tag_SYNTAX) {
            symbol_node *struct_node;
            if (struct_node = get_symbol(children[1]->lchild->value.str_val)) {
                p_node->attr.type = struct_node->u.struct_val.structure;
                p_node->attr.is_legal = TRUE;
            }
            else {
                print_error(17, children[1]->lineno, "Undefined structure");
                p_node->attr.is_legal = FALSE;
            }
        }
    }
    else if (p_node->type == ExtDecList_SYNTAX) {
        if (child_num == 3 && children[0]->type == VarDec_SYNTAX && children[1]->type == COMMA_TOKEN && children[2]->type == ExtDecList_SYNTAX) {
            children[0]->attr.inh_type = p_node->attr.inh_type;
            sdt(children[0]);
            children[1]->attr.inh_type = p_node->attr.inh_type;
            sdt(children[1]);
        }
        else if (child_num == 1 && children[0]->type == VarDec_SYNTAX) {
            children[0]->attr.inh_type = p_node->attr.inh_type;
            sdt(children[0]);
        }
    }
    else if (p_node->type == VarDec_SYNTAX) {
        if (child_num == 1 && children[0]->type == ID_TOKEN) {
            p_node->attr.type = p_node->attr.inh_type;
            p_node->attr.id = children[0]->value.str_val;
            if (p_node->attr.is_in_struct) {
                p_node->attr.structure = (FieldList*)malloc(sizeof(FieldList));
                p_node->attr.structure->next = NULL;
                p_node->attr.structure->type = p_node->attr.type;
                p_node->attr.structure->lineno = children[0]->lineno;
                strcpy(p_node->attr.structure->id, children[0]->value.str_val);
                return;
            }
            if (get_symbol(children[0]->value.str_val) == NULL) {
                symbol_node *new_symbol = (symbol_node*)malloc(sizeof(symbol_node));
                strcpy(new_symbol->key, children[0]->value.str_val);
                new_symbol->type = Var;
                new_symbol->u.var_val.type = p_node->attr.inh_type;
                insert_symbol(new_symbol);       
            }
            else {
                print_error(3, children[0]->lineno, "Variable redefined.");
            }
        }
        else if (child_num == 4 && children[0]->type == VarDec_SYNTAX && children[1]->type == LB_TOKEN && children[2]->type == INT_TOKEN && children[3]->type == RB_TOKEN) {
            Type *type_node = (Type*)malloc(sizeof(Type));
            type_node->kind = Array;
            type_node->u.array.elem = p_node->attr.inh_type;
            type_node->u.array.size = children[2]->value.int_val;
            children[0]->attr.inh_type = type_node;
            children[0]->attr.is_in_struct = p_node->attr.is_in_struct;
            sdt(children[0]);
            p_node->attr.type = children[0]->attr.type;
            p_node->attr.id = children[0]->attr.id;
        }
    }
    else if (p_node->type == FunDec_SYNTAX) {
        symbol_node *func_node = get_symbol(children[0]->value.str_val);
        if (p_node->attr.is_definition && (func_node && func_node->type != Func || func_node && func_node->type == Func && func_node->u.func_val.is_defined)) {
            print_error(4, children[0]->lineno, "Function redefined.");
            return;
        }
        if (func_node == NULL) {
            func_node = (symbol_node*)malloc(sizeof(symbol_node));
            strcpy(func_node->key, children[0]->value.str_val);
            func_node->type= Func;
            func_node->u.func_val.ret_type = p_node->attr.inh_type;
            func_node->u.func_val.lineno = p_node->lineno;
            p_node->attr.is_declared = FALSE;
            insert_symbol(func_node);
        }
        else {
            if (!is_same_type(func_node->u.func_val.ret_type, p_node->attr.inh_type)) {
                print_error(19, children[0]->lineno, "Return type conflict");
            }
            p_node->attr.is_declared = TRUE;
        }
        if (child_num == 4 && children[0]->type == ID_TOKEN && children[1]->type == LP_TOKEN && children[2]->type == VarList_SYNTAX && children[3]->type == RP_TOKEN) {
            if (p_node->attr.is_declared) {
                children[2]->attr.args = func_node->u.func_val.args;
            }
            children[2]->attr.is_definition = p_node->attr.is_definition;
            children[2]->attr.is_declared = p_node->attr.is_declared;
            sdt(children[2]);
            if (!p_node->attr.is_declared)
                func_node->u.func_val.args = children[2]->attr.args;
        }
        else if (child_num == 3 && children[0]->type == ID_TOKEN && children[1]->type == LP_TOKEN && children[2]->type == RP_TOKEN) {
            if (p_node->attr.is_declared) {
                if (func_node->u.func_val.args)
                    print_error(19, children[0]->lineno, "Argument number conflict");
            }
            else {
                func_node->u.func_val.args = NULL;
            }
        }
        if (p_node->attr.is_definition)
            func_node->u.func_val.is_defined = TRUE;
    }
    else if (p_node->type == VarList_SYNTAX) {
        children[0]->attr.is_declared = p_node->attr.is_declared;
        children[0]->attr.is_definition = p_node->attr.is_definition;
        if (p_node->attr.is_declared) {
            if (p_node->attr.args == NULL) {
                print_error(19, children[0]->lineno, "Argument number conflict");
                return;
            }
            children[0]->attr.type = p_node->attr.args->type;
        }
        sdt(children[0]);
        if (!p_node->attr.is_declared) {
            p_node->attr.args = (arg_node*)malloc(sizeof(arg_node));
            p_node->attr.args->type = children[0]->attr.type;
        }
        if (child_num == 3 && children[0]->type == ParamDec_SYNTAX && children[1]->type == COMMA_TOKEN && children[2]->type == VarList_SYNTAX) {
            if (p_node->attr.is_declared) {
                children[2]->attr.args = p_node->attr.args->next;
            }
            children[2]->attr.is_declared = p_node->attr.is_declared;
            children[2]->attr.is_definition = p_node->attr.is_definition;
            sdt(children[2]);
            if (!p_node->attr.is_declared)
                p_node->attr.args->next = children[2]->attr.args;
        }
        else if (child_num == 1 && children[0]->type == ParamDec_SYNTAX) {
            if (p_node->attr.is_declared) {
                if (p_node->attr.args->next)
                    print_error(19, children[0]->lineno, "Argument number conflict");
            }
            else
                p_node->attr.args->next = NULL;
        }
    }
    else if (p_node->type == ParamDec_SYNTAX) {
        if (child_num == 2 && children[0]->type == Specifier_SYNTAX && children[1]->type == VarDec_SYNTAX) {
            sdt(children[0]);
            children[1]->attr.inh_type = children[0]->attr.type;
            sdt(children[1]);
            if (p_node->attr.is_declared) {
                if (!is_same_type(p_node->attr.type, children[1]->attr.type)) {
                    print_error(19, children[0]->lineno, "Argument type conflict");
                }
            }
            else 
                p_node->attr.type = children[1]->attr.type;
        }
    }
    else if (p_node->type == CompSt_SYNTAX) {
        if (child_num == 4 && children[0]->type == LC_TOKEN && children[1]->type == DefList_SYNTAX && children[2]->type == StmtList_SYNTAX && children[3]->type == RC_TOKEN) {
            children[1]->attr.is_in_struct = FALSE;
            sdt(children[1]);
            children[2]->attr.ret_type = p_node->attr.ret_type;
            sdt(children[2]);
        }
    }
    else if (p_node->type == StmtList_SYNTAX) {
        if (child_num == 2 && children[0]->type == Stmt_SYNTAX && children[1]->type == StmtList_SYNTAX) {
            children[0]->attr.ret_type = p_node->attr.ret_type;
            sdt(children[0]);
            children[1]->attr.ret_type = p_node->attr.ret_type;
            sdt(children[1]);
        }
        else if (child_num == 0) {

        }
    }
    else if (p_node->type == DefList_SYNTAX) {
        if (child_num == 2 && children[0]->type == Def_SYNTAX && children[1]->type == DefList_SYNTAX) {
            children[0]->attr.is_in_struct = p_node->attr.is_in_struct;
            sdt(children[0]);
            children[1]->attr.is_in_struct = p_node->attr.is_in_struct;
            sdt(children[1]);
            if (p_node->attr.is_in_struct) {
                FieldList *field = p_node->attr.structure = children[0]->attr.structure;
                while (field->next)
                    field = field->next;
                field->next = children[1]->attr.structure;
            }
        }
        else if (child_num == 0) {
            if (p_node->attr.is_in_struct) {
                p_node->attr.structure = NULL;
            }
        }
    }
    else if (p_node->type == Stmt_SYNTAX) {
        if (child_num == 3 && children[0]->type == RETURN_TOKEN && children[1]->type == Exp_SYNTAX && children[2]->type == SEMI_TOKEN) {
            sdt(children[1]);
            if (!is_same_type(children[1]->attr.type, p_node->attr.ret_type)) {
                print_error(8, children[1]->lineno, "Return type mismatched.");
            }
        }
        else {
            int i;
            for (i = 0; i < child_num; ++ i)
                sdt(children[i]);
        }
    }
    else {
        int i;
        for (i = 0; i < child_num; ++ i)
            sdt(children[i]);
    }
}
