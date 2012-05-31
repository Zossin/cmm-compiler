#include "inter_code.h"
#include "gen_code.h"
#include "var_table.h"
#include <stdio.h>
#include <malloc.h>

#define IS_RELOP(code) ((code)->kind == Equal || (code)->kind == NotEqual || (code)->kind == More || (code)->kind == Less || (code)->kind == MoreEqual || (code)->kind == LessEqual)

#define ASSEM_HDR ".data\n"\
"_prompt: .asciiz \"Enter an integer:\"\n"\
"_ret: .asciiz \"\\n\"\n"\
".globl main\n"\
"\n"\
".text\n"\
"read:\n"\
"\tli $v0, 4\n"\
"\tla $a0, _prompt\n"\
"\tsyscall\n"\
"\tli $v0, 5\n"\
"\tsyscall\n"\
"\tjr $ra\n"\
"\n"\
"write:\n"\
"\tli $v0, 1\n"\
"\tsyscall\n"\
"\tli $v0, 4\n"\
"\tla $a0, _ret\n"\
"\tsyscall\n"\
"\tmove $v0, $0\n"\
"\tjr $ra\n"\
"\n"

char *reg_names[] = {"$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7", "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", "$t8", "$t9", "$a0", "$a1", "$a2", "$a3"};

char *print_reg(int reg_id) {
    return reg_names[reg_id];
}

void spill_reg(FILE *file, int reg_id) {
    if (reg_id >= NR_REGS)
        return;
    var_node *p_var = get_var_node(regs[reg_id].var_id);
    if (p_var->is_param_reg)
        return;
    fprintf(file, "\tsw %s, %d($fp)\n", print_reg(reg_id), -p_var->offset - 4);
}

void free_reg(FILE *file, int reg_id) {
    if (reg_id >= NR_REGS)
        return;
    if (regs[reg_id].var_id < 0)
        return;
    if (regs[reg_id].is_modified) {
        spill_reg(file, reg_id);
    }
    regs[reg_id].var_id = -1;
}

void set_reg_modified(int reg_id) {
    if (reg_id >= NR_REGS)
        return;
    regs[reg_id].is_modified = TRUE;
}

int allocate_reg(FILE *file, int var_id) {
    int i;
    for (i = 0; i < NR_REGS; ++ i)
        if (regs[i].var_id < 0) {
            regs[i].var_id = var_id;
            regs[i].is_modified = FALSE;
            return i;
        }

    spill_reg(file, 0);
    regs[0].var_id = var_id;
    regs[0].is_modified = FALSE;

    return 0;
}

int store_const_into_reg(FILE *file, int val) {
    int reg = allocate_reg(file, CONST_TEMP_REG);
    fprintf(file, "\tli %s, %d\n", print_reg(reg), val);

    return reg;
}

int ensure_reg(FILE *file, int var_id) {
    var_node *p_var = get_var_node(var_id);

    if (p_var == NULL)
        return -1;

    if (p_var->is_param_reg)
        return NR_REGS + p_var->offset;
    
    int i;
    for (i = 0; i < NR_REGS; ++ i)
        if (regs[i].var_id == var_id)
            return i;

    int reg = allocate_reg(file, var_id);
    fprintf(file, "\tlw %s, %d($fp)\n", print_reg(reg), -p_var->offset - 4);

    return reg;
}

void print_mips_code(FILE *file, InterCodeList *codes) {
    InterCode *code = codes->code;

    if (code->kind == LabelDec)
        fprintf(file, "label%d :\n", code->u.label.dest->u.var_no);
    else if (code->kind == Assign) {
        if (code->u.assign.right->kind == Variable && code->u.assign.left->kind == Constant) {
            int regx = allocate_reg(file, code->u.assign.right->u.var_no);
            fprintf(file, "\tli %s, %d\n", print_reg(regx), code->u.assign.left->u.value);
            set_reg_modified(regx);
            free_reg(file, regx);
        }
        else if (code->u.assign.right->kind == Variable && code->u.assign.left->kind == Variable) {
            int regy = ensure_reg(file, code->u.assign.left->u.var_no);
            int regx = allocate_reg(file, code->u.assign.right->u.var_no);
            fprintf(file, "\tmove %s, %s\n", print_reg(regx), print_reg(regy));
            set_reg_modified(regx);
            free_reg(file, regx);
            free_reg(file, regy);
        }
        else if (code->u.assign.right->kind == Variable && code->u.assign.left->kind == Address) {
            int regx, regy;

            regx = ensure_reg(file, code->u.assign.right->u.var_no);
            regy = ensure_reg(file, code->u.assign.left->u.var_no);

            free_reg(file, regx);free_reg(file, regy);

            fprintf(file, "\tlw %s, 0(%s)\n", print_reg(regx), print_reg(regy));
        }
        else if (code->u.assign.right->kind == Address) {
            int regx, regy;

            if (code->u.assign.left->kind == Variable)
                regy = ensure_reg(file, code->u.assign.left->u.var_no);
            else if (code->u.assign.left->kind == Constant)
                regy = store_const_into_reg(file, code->u.assign.left->u.var_no);

            regx = ensure_reg(file, code->u.assign.right->u.var_no);

            free_reg(file, regx);free_reg(file, regy);

            fprintf(file, "\tsw %s, 0(%s)\n", print_reg(regy), print_reg(regx));
        }
        else {
            fprintf(file, "\tUnexpected assign inter code!\n");
        }

    }
    else if (code->kind == Add || code->kind == Sub || code->kind  == Mul || code->kind == Div) {
        if (code->kind == Add && code->u.binop.op1->kind == Reference) {
            var_node *p_var = get_var_node(code->u.binop.op1->u.var_no);
            int regx = allocate_reg(file, code->u.binop.result->u.var_no);
            fprintf(file, "\taddi %s, $fp, %d\n", print_reg(regx), -p_var->offset - code->u.binop.op2->u.value);
            set_reg_modified(regx);
            free_reg(file, regx);
        }
        else {
            int regy, regz;

            if (code->u.binop.op1->kind == Constant)
                regy = store_const_into_reg(file, code->u.binop.op1->u.value);
            else if (code->u.binop.op1->kind == Variable)
                regy = ensure_reg(file, code->u.binop.op1->u.var_no);

            if (code->u.binop.op2->kind == Constant)
                regz = store_const_into_reg(file, code->u.binop.op2->u.value);
            else if (code->u.binop.op2->kind == Variable)
                regz = ensure_reg(file, code->u.binop.op2->u.var_no);

            free_reg(file, regy);free_reg(file, regz);

            int regx = allocate_reg(file, code->u.binop.result->u.var_no);

            if (code->kind == Add)
                fprintf(file, "\tadd %s, %s, %s\n", print_reg(regx), print_reg(regy), print_reg(regz));
            else if (code->kind == Sub)
                fprintf(file, "\tsub %s, %s, %s\n", print_reg(regx), print_reg(regy), print_reg(regz));
            else if (code->kind == Mul)
                fprintf(file, "\tmul %s, %s, %s\n", print_reg(regx), print_reg(regy), print_reg(regz));
            else if (code->kind == Div) {
                fprintf(file, "\tdiv %s, %s\n", print_reg(regy), print_reg(regz));
                fprintf(file, "\tmflo %s\n", print_reg(regx));
            }

            set_reg_modified(regx);
            free_reg(file, regx);
        }
    }
    else if (IS_RELOP(code)) {
        int regx, regy;

        if (code->u.binop.op1->kind == Constant)
            regx = store_const_into_reg(file, code->u.binop.op1->u.value);
        else if (code->u.binop.op1->kind == Variable)
            regx = ensure_reg(file, code->u.binop.op1->u.var_no);

        if (code->u.binop.op2->kind == Constant)
            regy = store_const_into_reg(file, code->u.binop.op2->u.value);
        else if (code->u.binop.op2->kind == Variable)
            regy = ensure_reg(file, code->u.binop.op2->u.var_no);

        free_reg(file, regx);free_reg(file, regy);

        fprintf(file, "\t");

        if (code->kind == Equal)
            fprintf(file, "beq");
        else if (code->kind == NotEqual)
            fprintf(file, "bne");
        else if (code->kind == More)
            fprintf(file, "bgt");
        else if (code->kind == Less)
            fprintf(file, "blt");
        else if (code->kind == MoreEqual)
            fprintf(file, "bge");
        else if (code->kind == LessEqual)
            fprintf(file, "ble");


        fprintf(file, " %s, %s, label%d\n", print_reg(regx), print_reg(regy), code->u.binop.result->u.var_no);

    }
    else if (code->kind == Return) {
        int regx;

        if (code->u.command.op->kind == Constant)
            regx = store_const_into_reg(file, code->u.command.op->u.value);
        else if (code->u.command.op->kind == Variable)
            regx = ensure_reg(file, code->u.command.op->u.var_no);

        free_reg(file, regx);
        
        fprintf(file, "\tmove $v0, %s\n", print_reg(regx));
        fprintf(file, "\tjr $ra\n");
    }
    else if (code->kind == Goto) {
        fprintf(file, "\tj label%d\n", code->u.command.op->u.var_no);
    }
    else if (code->kind == Arg) {
        fprintf(file, "ARG ");

        if (code->u.command.op->kind == Reference)
            fprintf(file, "&t%d", code->u.command.op->u.var_no);
        else if (code->u.command.op->kind == Variable)
            fprintf(file, "t%d", code->u.command.op->u.var_no);
        else if (code->u.command.op->kind == Constant)
            fprintf(file, "#%d", code->u.command.op->u.value);

        fprintf(file, "\n");
    } 
    else if (code->kind == Call) {
        fprintf(file, "\tjal %s\n", code->u.call.func_name);
        int regx = allocate_reg(file, code->u.call.result->u.var_no);
        fprintf(file, "\tmove %s, $v0\n", print_reg(regx));
        set_reg_modified(regx);
        free_reg(file, regx);
    }
    else if (code->kind == Read) {
        int regx = allocate_reg(file, code->u.command.op->u.var_no);
        fprintf(file, "\tjal read\n");
        fprintf(file, "\tmove %s, $v0\n", print_reg(regx));
        set_reg_modified(regx);
        free_reg(file, regx);
    }
    else if (code->kind == Write) {
        int regx;
        if (code->u.command.op->kind == Variable)
            regx = ensure_reg(file, code->u.command.op->u.var_no);
        else if (code->u.command.op->kind == Constant)
            regx = store_const_into_reg(file, code->u.command.op->u.value);
        free_reg(file, regx);
        fprintf(file, "\tmove $a0, %s\n", print_reg(regx));
        fprintf(file, "\tjal write\n");
    }

}

BOOL is_first_instr(InterCodeList *codes) {
    return codes->code->kind == FunCode || codes->code->kind == Call || codes->code->kind == LabelDec || IS_RELOP(codes->prev->code) || codes->prev->code->kind == Goto || codes->prev->code->kind == Call;
}

void init_basic_block(InterCodeList *codes) {

}

void init_regs() {
    int i;
    for (i = 0; i < NR_REGS; ++ i)
        regs[i].var_id = -1;
}

void ensure_var(int var_id, int size) {
    var_node *p_var = get_var_node(var_id);

    if (p_var == NULL) {
        insert_var_node(var_id, FALSE, offset_now);
        offset_now += size;
    }
}

BOOL is_in_func = FALSE;

void init_func_var(FILE *file, InterCodeList *codes) {
    offset_now = 0;
    is_in_func = TRUE;
    init_var_table();
    init_regs();

    if (codes && codes->code->kind == FunCode) {
        fprintf(file, "%s:\n", codes->code->u.func.func_name);

        codes = codes->next;
        
        int param_num = 0;

        while (codes && codes->code->kind != FunCode) {
            InterCode *code = codes->code;
            if (code->kind == Param) {
                if (param_num < 4) {
                    insert_var_node(code->u.command.op->u.var_no, TRUE, param_num);
                }
                else {
                    insert_var_node(code->u.command.op->u.var_no, FALSE, -param_num * 4);
                }
                ++ param_num;
            }
            else if (code->kind == Add || code->kind == Sub || code->kind == Mul || code->kind == Div) {
                if (code->u.binop.result->kind == Variable) {
                    ensure_var(code->u.binop.result->u.var_no, 4);
                }
            }
            else if (code->kind == Assign) {
                if (code->u.assign.right->kind == Variable) {
                    ensure_var(code->u.assign.right->u.var_no, 4);
                }
            }
            else if (code->kind == Read) {
                if (code->u.command.op->kind == Variable) {
                    ensure_var(code->u.command.op->u.var_no, 4);
                }
            }
            else if (code->kind == Dec) {
                ensure_var(code->u.dec.addr->u.var_no, code->u.dec.size->u.value);
            }

            codes = codes->next;
        }
    }

    fprintf(file, "\tsw $ra, -4($sp)\n");
    fprintf(file, "\tsw $fp, -8($sp)\n");
    fprintf(file, "\taddi $fp, $sp, -8\n");
    fprintf(file, "\tsubu $sp, $sp, %d\n", -8 - offset_now);
}

void end_of_func(FILE *file) {
    fprintf(file, "\taddi $sp, $fp, 8\n");
    fprintf(file, "\tlw $ra, -4($sp)\n");
    fprintf(file, "\tlw $fp, -8($sp)\n");
    fprintf(file, "\n");
    is_in_func = FALSE;
}

InterCodeList *codes_next(FILE *file, InterCodeList *codes) {
    InterCodeList *ret;
    if (codes->code->kind == Arg) {
        while (codes->code->kind == Arg)
            codes = codes->next;
        ret = codes;
    }
    else
        ret = codes->next;

    if (ret == NULL)
        end_of_func(file);

    if (ret && ret->code->kind == FunCode) {
        if (is_in_func) {
            end_of_func(file);
        }

        init_func_var(file, ret);
    }

    if (ret && is_first_instr(ret)) {
        init_basic_block(ret);
    }

    return ret;
}

void gen_code(FILE *file, InterCodeList *codes) {
    if (codes) { //make it not loop linked list
        InterCodeList *tail = codes->prev;
        codes->prev = NULL;
        tail->next = NULL;
    }

    fprintf(file, ASSEM_HDR);

    init_func_var(file, codes);
    init_basic_block(codes);

    while (codes) {
        print_mips_code(file, codes);
        codes = codes_next(file, codes);
    }
}
