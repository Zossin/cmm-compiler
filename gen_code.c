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

char *print_reg(int reg_id) {
    char *buffer = (char*)malloc(4);
    sprintf(buffer, "$%s%d", reg_id < 8 ? "t" : "s", reg_id < 8 ? reg_id : reg_id - 8);
    return buffer;
}

void spill_reg(FILE *file, int reg_id) {
    var_node *p_var = get_var_node(regs[reg_id].var_id);
    fprintf(file, "\tsw %s, %d($fp)\n", print_reg(reg_id), -p_var->offset - 4);
}

void free_reg(FILE *file, int reg_id) {
    if (regs[reg_id].var_id < 0)
        return;
    if (regs[reg_id].is_modified) {
        spill_reg(file, reg_id);
    }
    regs[reg_id].var_id = -1;
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

int ensure_reg(FILE *file, int var_id) {
    var_node *p_var = get_var_node(var_id);

    if (p_var == NULL)
        return -1;

    if (p_var->is_param_reg)
        return p_var->offset;
    
    int i;
    for (i = 0; i < NR_REGS; ++ i)
        if (regs[i].var_id == var_id)
            return i;

    int reg = allocate_reg(file, var_id);
    fprintf(file, "\tlw %s, %d($fp)\n", print_reg(reg), -p_var->offset - 4);
}

void print_mips_code(FILE *file, InterCodeList *codes) {
    InterCode *code = codes->code;

    if (code->kind == LabelDec)
        fprintf(file, "label%d :\n", code->u.label.dest->u.var_no);
    else if (code->kind == Assign) {
        if (code->u.assign.right->kind == Variable && code->u.assign.left->kind == Constant) {
            fprintf(file, "");
        }
        else {
            fprintf(file, "\tUnexpected assign inter code!\n");
        }

    }
    else if (code->kind == Add || code->kind == Sub || code->kind  == Mul || code->kind == Div) {
        fprintf(file, "t%d := ", code->u.binop.result->u.var_no);

        if (code->u.binop.op1->kind == Reference)
            fprintf(file, "&t%d", code->u.binop.op1->u.var_no);
        else if (code->u.binop.op1->kind == Variable)
            fprintf(file, "t%d", code->u.binop.op1->u.var_no);
        else if (code->u.binop.op1->kind == Constant)
            fprintf(file, "#%d", code->u.binop.op1->u.value);

        fprintf(file, " ");

        if (code->kind == Add)
            fprintf(file, "+");
        else if (code->kind == Sub)
            fprintf(file, "-");
        else if (code->kind == Mul)
            fprintf(file, "*");
        else if (code->kind == Div)
            fprintf(file, "/");

        fprintf(file, " ");

        if (code->u.binop.op2->kind == Reference)
            fprintf(file, "&t%d", code->u.binop.op2->u.var_no);
        else if (code->u.binop.op2->kind == Variable)
            fprintf(file, "t%d", code->u.binop.op2->u.var_no);
        else if (code->u.binop.op2->kind == Constant)
            fprintf(file, "#%d", code->u.binop.op2->u.value);

        fprintf(file, "\n");
    }
    else if (IS_RELOP(code)) {
        fprintf(file, "IF ");

        if (code->u.binop.op1->kind == Variable)
            fprintf(file, "t%d", code->u.binop.op1->u.var_no);
        else if (code->u.binop.op1->kind == Constant)
            fprintf(file, "#%d", code->u.binop.op1->u.value);

        fprintf(file, " ");

        if (code->kind == Equal)
            fprintf(file, "==");
        else if (code->kind == NotEqual)
            fprintf(file, "!=");
        else if (code->kind == More)
            fprintf(file, ">");
        else if (code->kind == Less)
            fprintf(file, "<");
        else if (code->kind == MoreEqual)
            fprintf(file, ">=");
        else if (code->kind == LessEqual)
            fprintf(file, "<=");

        fprintf(file, " ");

        if (code->u.binop.op2->kind == Variable)
            fprintf(file, "t%d", code->u.binop.op2->u.var_no);
        else if (code->u.binop.op2->kind == Constant)
            fprintf(file, "#%d", code->u.binop.op2->u.value);

        fprintf(file, " GOTO label%d\n", code->u.binop.result->u.var_no);

    }
    else if (code->kind == Return) {
        fprintf(file, "RETURN ");
        if (code->u.command.op->kind == Variable)
            fprintf(file, "t%d", code->u.command.op->u.var_no);
        else if (code->u.command.op->kind == Constant)
            fprintf(file, "#%d", code->u.command.op->u.value);
        fprintf(file, "\n");
    }
    else if (code->kind == Goto) 
        fprintf(file, "GOTO label%d\n", code->u.command.op->u.var_no);
    else if (code->kind == Dec) 
        fprintf(file, "DEC t%d %d\n", code->u.dec.addr->u.var_no, code->u.dec.size->u.value);
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
    else if (code->kind == Call) 
        fprintf(file, "t%d := CALL %s\n", code->u.call.result->u.var_no, code->u.call.func_name);
    else if (code->kind == Param)
        fprintf(file, "PARAM t%d\n", code->u.command.op->u.var_no);
    else if (code->kind == Read)
        fprintf(file, "READ t%d\n", code->u.command.op->u.var_no);
    else if (code->kind == Write)
        fprintf(file, "WRITE t%d\n", code->u.command.op->u.var_no);

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

        while (codes && codes->code->kind != FunCode) {
            InterCode *code = codes->code;
            
            if (code->kind == Add || code->kind == Sub || code->kind == Mul || code->kind == Div) {
                if (code->u.binop.result->kind == Variable) {
                    ensure_var(code->u.binop.result->u.var_no, 4);
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
    fprintf(file, "\tsubu $fp, -8($sp)\n");
    fprintf(file, "\tsubu $sp, %d($sp)\n", -8 - offset_now);
}

void end_of_func(FILE *file) {
    fprintf(file, "\taddi $sp, 8($fp)\n");
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
        ret = ret->next;
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
