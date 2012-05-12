#include "common.h"
#include "syntax_tree_node.h"
#include "symbol_table.h"
#include "translate.h"
#include "inter_code.h"
#include <stdlib.h>
#include <string.h>

#define NR_CHILDREN 8

#define TRANSLATE_EXP_CASE_COND \
    Operand *label1 = new_label(), *label2 = new_label(); \
InterCodeList *code0 = make_code_list(gen_assign_code(place, new_const_op(0))); \
InterCodeList *code1 = translate_Cond(p_node, label1, label2); \
InterCodeList *code2 = link_inter_code(2, make_code_list(gen_label_code(label1)), (place == NULL) ? NULL : make_code_list(gen_assign_code(place, new_const_op(1)))); \
return link_inter_code(4, code0, code1, code2, make_code_list(gen_label_code(label2)));

#define TRANSLATE_EXP_CASE_ARIBINOP(operator) \
    Operand *t1 = new_temp(), *t2 = new_temp(); \
InterCodeList *code1 = translate_Exp(children[0], t1), *code2 = translate_Exp(children[2], t2); \
InterCodeList *code3 = (place == NULL) ? NULL : make_code_list(gen_binop_code(operator, place, t1, t2)); \
return link_inter_code(3, code1, code2, code3);

typedef struct OperandList_ OperandList;

struct OperandList_ {
    Operand *op;
    OperandList *next;
};

InterCodeList *translate_Exp(syntax_tree_node *p_node, Operand *place);

InterCodeList *translate_Args(syntax_tree_node *p_node, OperandList **p_arg_list) {
    syntax_tree_node *children[NR_CHILDREN];
    syntax_tree_node *child = p_node->lchild;
    int child_num = 0;
    while (child) {
        children[child_num ++] = child;
        child = child->next_sibling;
    }

    if (child_num == 1 && children[0]->type == Exp_SYNTAX) {
        Operand *t1 = new_temp();
        InterCodeList *code1 = translate_Exp(children[0], t1);

        //arg_list = t1 + arg_list
        OperandList *new_node = (OperandList*)malloc(sizeof(OperandList));
        new_node->op = t1;
        new_node->next = *p_arg_list;
        *p_arg_list = new_node;
        
        return code1;
    }
    else if (child_num == 3 && children[0]->type == Exp_SYNTAX && children[1]->type == COMMA_TOKEN && children[2]->type == Args_SYNTAX) {
        Operand *t1 = new_temp();
        InterCodeList *code1 = translate_Exp(children[0], t1);
        

        //arg_list = t1 + arg_list
        OperandList *new_node = (OperandList*)malloc(sizeof(OperandList));
        new_node->op = t1;
        new_node->next = *p_arg_list;
        *p_arg_list = new_node;

        InterCodeList *code2 = translate_Args(children[2], p_arg_list);
        return link_inter_code(2, code1, code2);
    }
    return NULL;
}

InterCodeList *translate_Cond(syntax_tree_node *p_node, Operand *label_true, Operand *label_false) {
    syntax_tree_node *children[NR_CHILDREN];
    syntax_tree_node *child = p_node->lchild;
    int child_num = 0;
    while (child) {
        children[child_num ++] = child;
        child = child->next_sibling;
    }

    if (child_num == 3 && children[0]->type == Exp_SYNTAX && children[1]->type == RELOP_TOKEN && children[2]->type == Exp_SYNTAX) {
        Operand *t1 = new_temp(), *t2 = new_temp();
        InterCodeList *code1 = translate_Exp(children[0], t1);
        InterCodeList *code2 = translate_Exp(children[2], t2);
        code_kind kind;
        if (strcmp(children[1]->value.str_val, "==") == 0)
            kind = Equal;
        else if (strcmp(children[1]->value.str_val, "!=") == 0)
            kind = NotEqual;
        else if (strcmp(children[1]->value.str_val, ">") == 0)
            kind = More;
        else if (strcmp(children[1]->value.str_val, "<") == 0)
            kind = Less;
        else if (strcmp(children[1]->value.str_val, ">=") == 0)
            kind = MoreEqual;
        else if (strcmp(children[1]->value.str_val, "<=") == 0)
            kind = LessEqual;
        InterCodeList *code3 = make_code_list(gen_binop_code(kind, label_true, t1, t2));
        return link_inter_code(4, code1, code2, code3, make_code_list(gen_command_code(Goto, label_false)));
    }
    else if (child_num == 2 && children[0]->type == NOT_TOKEN && children[1]->type == Exp_SYNTAX) {
        return translate_Cond(children[1], label_false, label_true);
    }
    else if (child_num == 3 && children[0]->type == Exp_SYNTAX && children[1]->type == AND_TOKEN && children[2]->type == Exp_SYNTAX) {
        Operand *label1 = new_label();
        InterCodeList *code1 = translate_Cond(children[0], label1, label_false);
        InterCodeList *code2 = translate_Cond(children[2], label_true, label_false);
        return link_inter_code(3, code1, make_code_list(gen_label_code(label1)), code2);
    }
    else if (child_num == 3 && children[0]->type == Exp_SYNTAX && children[1]->type == OR_TOKEN && children[2]->type == Exp_SYNTAX) {
        Operand *label1 = new_label();
        InterCodeList *code1 = translate_Cond(children[0], label_true, label1);
        InterCodeList *code2 = translate_Cond(children[2], label_true, label_false);
        return link_inter_code(3, code1, make_code_list(gen_label_code(label1)), code2);
    }
    else {
        Operand *t1 = new_temp();
        InterCodeList *code1 = translate_Exp(p_node, t1);
        InterCodeList *code2 = make_code_list(gen_binop_code(NotEqual, label_true, t1, new_const_op(0)));
        return link_inter_code(3, code1, code2, make_code_list(gen_command_code(Goto, label_false)));
    }
}

InterCodeList *translate_Exp(syntax_tree_node *p_node, Operand *place) {
    syntax_tree_node *children[NR_CHILDREN];
    syntax_tree_node *child = p_node->lchild;
    int child_num = 0;
    while (child) {
        children[child_num ++] = child;
        child = child->next_sibling;
    }

    if (child_num == 3 && children[0]->type == Exp_SYNTAX && children[1]->type == ASSIGNOP_TOKEN && children[2]->type == Exp_SYNTAX) {
        Operand *t1 = new_var_op(-1), *t2 = new_temp();
        InterCodeList *code1 = translate_Exp(children[0], t1), *code2 = translate_Exp(children[2], t2);
        InterCodeList *code3 = make_code_list(gen_assign_code(t1, t2));
        InterCodeList *code4 = (place == NULL) ? NULL : make_code_list(gen_assign_code(place, t1));
        return link_inter_code(4, code1, code2, code3, code4);
    }
    else if (child_num == 3 && children[0]->type == Exp_SYNTAX && children[1]->type == AND_TOKEN && children[2]->type == Exp_SYNTAX) {
        TRANSLATE_EXP_CASE_COND
    }
    else if (child_num == 3 && children[0]->type == Exp_SYNTAX && children[1]->type == OR_TOKEN && children[2]->type == Exp_SYNTAX) {
        TRANSLATE_EXP_CASE_COND
    }
    else if (child_num == 3 && children[0]->type == Exp_SYNTAX && children[1]->type == RELOP_TOKEN && children[2]->type == Exp_SYNTAX) {
        TRANSLATE_EXP_CASE_COND
    }
    else if (child_num == 2 && children[0]->type == NOT_TOKEN && children[1]->type == Exp_SYNTAX) {
        TRANSLATE_EXP_CASE_COND
    }
    else if (child_num == 3 && children[0]->type == Exp_SYNTAX && children[1]->type == PLUS_TOKEN && children[2]->type == Exp_SYNTAX) {
        TRANSLATE_EXP_CASE_ARIBINOP(Add)
    }
    else if (child_num == 3 && children[0]->type == Exp_SYNTAX && children[1]->type == MINUS_TOKEN && children[2]->type == Exp_SYNTAX) {
        TRANSLATE_EXP_CASE_ARIBINOP(Sub)
    }
    else if (child_num == 3 && children[0]->type == Exp_SYNTAX && children[1]->type == STAR_TOKEN && children[2]->type == Exp_SYNTAX) {
        TRANSLATE_EXP_CASE_ARIBINOP(Mul)
    }
    else if (child_num == 3 && children[0]->type == Exp_SYNTAX && children[1]->type == DIV_TOKEN && children[2]->type == Exp_SYNTAX) {
        TRANSLATE_EXP_CASE_ARIBINOP(Div)
    }
    else if (child_num == 3 && children[0]->type == LP_TOKEN && children[1]->type == Exp_SYNTAX && children[2]->type == RP_TOKEN) {
        return translate_Exp(children[1], place);
    }
    else if (child_num == 2 && children[0]->type == MINUS_TOKEN && children[1]->type == Exp_SYNTAX) {
        Operand *t1 = new_temp();
        InterCodeList *code1 = translate_Exp(children[1], t1);
        InterCodeList *code2 = (place == NULL) ? NULL : make_code_list(gen_binop_code(Sub, place, new_const_op(0), t1));
        return link_inter_code(2, code1, code2);
    }
    else if (child_num == 4 && children[0]->type == ID_TOKEN && children[1]->type == LP_TOKEN && children[2]->type == Args_SYNTAX && children[3]->type == RP_TOKEN || child_num == 3 && children[0]->type == ID_TOKEN && children[1]->type == LP_TOKEN && children[2]->type == RP_TOKEN) {
        symbol_node *func_node = get_symbol(children[0]->value.str_val);
        Operand *ret = (place == NULL) ? new_temp() : place;
        if (child_num == 3) {
            if (strcmp(children[0]->value.str_val, "read") == 0)
                return make_code_list(gen_command_code(Read, ret));
            return make_code_list(gen_call_code(ret, children[0]->value.str_val));
        }
        else {
            OperandList *arg_list = NULL;
            InterCodeList *code1 = translate_Args(children[2], &arg_list);
            if (strcmp(children[0]->value.str_val, "write") == 0)
                return link_inter_code(2, code1, make_code_list(gen_command_code(Write, arg_list->op)));
            InterCodeList *code2 = NULL;
            while (arg_list) {
                code2 = link_inter_code(2, make_code_list(gen_command_code(Arg, arg_list->op)), code2);
                arg_list = arg_list->next;
            }
            return link_inter_code(3, code1, code2, make_code_list(gen_call_code(ret, children[0]->value.str_val)));
        }
    }
    else if (child_num == 4 && children[0]->type == Exp_SYNTAX && children[1]->type == LB_TOKEN && children[2]->type == Exp_SYNTAX && children[3]->type == RB_TOKEN) {

    }
    else if (child_num == 3 && children[0]->type == Exp_SYNTAX && children[1]->type == DOT_TOKEN && children[2]->type == ID_TOKEN) {

    }
    else if (child_num == 1 && children[0]->type == ID_TOKEN) {
        symbol_node *var_node = get_symbol(children[0]->value.str_val);
        InterCodeList *ret;
        if (place == NULL)
            ret = NULL;
        else if (place->u.var_no < 0) {
            ret = NULL;
            place->u.var_no = var_node->u.var_val.var_op->u.var_no;
        }
        else
            ret = make_code_list(gen_assign_code(place, var_node->u.var_val.var_op));
        return ret;
    }
    else if (child_num == 1 && children[0]->type == INT_TOKEN) {
        int value = children[0]->value.int_val;
        return (place == NULL) ? NULL : make_code_list(gen_assign_code(place, new_const_op(value)));
    }
}

InterCodeList *translate_Stmt(syntax_tree_node *p_node) {
    syntax_tree_node *children[NR_CHILDREN];
    syntax_tree_node *child = p_node->lchild;
    int child_num = 0;
    while (child) {
        children[child_num ++] = child;
        child = child->next_sibling;
    }

    if (child_num == 2 && children[0]->type == Exp_SYNTAX && children[1]->type == SEMI_TOKEN) {
        return translate_Exp(children[0], NULL);
    }
    else if (child_num == 1 && children[0]->type == CompSt_SYNTAX) {
        return translate(children[0]);
    }
    else if (child_num == 3 && children[0]->type == RETURN_TOKEN && children[1]->type == Exp_SYNTAX && children[2]->type == SEMI_TOKEN) {
        Operand *t1 = new_temp();
        InterCodeList *code1 = translate_Exp(children[1], t1);
        InterCodeList *code2 = make_code_list(gen_command_code(Return, t1));
        return link_inter_code(2, code1, code2);
    }
    else if (child_num == 5 && children[0]->type == IF_TOKEN && children[1]->type == LP_TOKEN && children[2]->type == Exp_SYNTAX && children[3]->type == RP_TOKEN && children[4]->type == Stmt_SYNTAX) {
        Operand *label1 = new_label(), *label2 = new_label();
        InterCodeList *code1 = translate_Cond(children[2], label1, label2);
        InterCodeList *code2 = translate_Stmt(children[4]);
        return link_inter_code(4, code1, make_code_list(gen_label_code(label1)), code2, make_code_list(gen_label_code(label2)));
    }
    else if (child_num == 7 && children[0]->type == IF_TOKEN && children[1]->type == LP_TOKEN && children[2]->type == Exp_SYNTAX && children[3]->type == RP_TOKEN && children[4]->type == Stmt_SYNTAX && children[5]->type == ELSE_TOKEN && children[6]->type == Stmt_SYNTAX) {
        Operand *label1 = new_label(), *label2 = new_label(), *label3 = new_label();
        InterCodeList *code1 = translate_Cond(children[2], label1, label2);
        InterCodeList *code2 = translate_Stmt(children[4]);
        InterCodeList *code3 = translate_Stmt(children[6]);
        return link_inter_code(7, code1, make_code_list(gen_label_code(label1)), code2, make_code_list(gen_command_code(Goto, label3)), make_code_list(gen_label_code(label2)), code3, make_code_list(gen_label_code(label3)));
    }
    else if (child_num == 5 && children[0]->type == WHILE_TOKEN && children[1]->type == LP_TOKEN && children[2]->type == Exp_SYNTAX && children[3]->type == RP_TOKEN && children[4]->type == Stmt_SYNTAX) {
        Operand *label1 = new_label(), *label2 = new_label(), *label3 = new_label();
        InterCodeList *code1 = translate_Cond(children[2], label1, label3);
        InterCodeList *code2 = translate_Stmt(children[4]);
        return link_inter_code(6, make_code_list(gen_label_code(label1)), code1, make_code_list(gen_label_code(label2)), code2, make_code_list(gen_command_code(Goto, label1)), make_code_list(gen_label_code(label3)));
    }
    return NULL;
}

void translate_VarDec(syntax_tree_node *p_node) {
    syntax_tree_node *children[NR_CHILDREN];
    syntax_tree_node *child = p_node->lchild;
    int child_num = 0;
    while (child) {
        children[child_num ++] = child;
        child = child->next_sibling;
    }

    if (child_num == 1 && children[0]->type == ID_TOKEN) {
        if (p_node->attr.is_in_struct) {
            return;
        }
        symbol_node *new_symbol = (symbol_node*)malloc(sizeof(symbol_node));
        strcpy(new_symbol->key, children[0]->value.str_val);
        new_symbol->type = Var;
        new_symbol->u.var_val.type = p_node->attr.inh_type;
        new_symbol->u.var_val.var_op = new_temp();
        insert_symbol(new_symbol);       
    }
    else if (child_num == 4 && children[0]->type == VarDec_SYNTAX && children[1]->type == LB_TOKEN && children[2]->type == INT_TOKEN && children[3]->type == RB_TOKEN) {
        translate_VarDec(children[0]);
    }
}

InterCodeList *translate(syntax_tree_node *p_node) {
    if (p_node == NULL)
        return NULL;

    if (p_node->type == Stmt_SYNTAX) {
        return translate_Stmt(p_node);
    }
    if (p_node->type == VarDec_SYNTAX) {
        translate_VarDec(p_node);
        return;
    }

    syntax_tree_node *children[NR_CHILDREN];
    syntax_tree_node *child = p_node->lchild;
    int child_num = 0;
    while (child) {
        children[child_num ++] = child;
        child = child->next_sibling;
    }

    if (p_node->type == ParamDec_SYNTAX) {
        translate_VarDec(children[1]);
        return make_code_list(gen_command_code(Param, get_symbol(children[1]->attr.id)->u.var_val.var_op));
    }

    if (p_node->type == FunDec_SYNTAX) {
        syntax_tree_node *args = (child_num == 3) ? NULL : children[2];
        return link_inter_code(2, make_code_list(gen_func_code(children[0]->value.str_val)), translate(args));
    }

    if (p_node->type == Dec_SYNTAX) {
        translate_VarDec(children[0]);
        if (child_num == 3) { //Dec -> VarDec ASSIGNOP Exp
            Operand *op = get_symbol(children[0]->attr.id)->u.var_val.var_op;
            return translate_Exp(children[2], op);
        }
        else
            return NULL;
    }

    InterCodeList *list = NULL;
    int i;
    for (i = 0; i < child_num; ++ i)
        list = link_inter_code(2, list, translate(children[i]));
    return list;

}

