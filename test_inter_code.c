#include "common.h"
#include "inter_code.h"
#include <stdio.h>

int main(int argc, char **argv) {
    InterCodeList *list1 = make_code_list(gen_func_code("main"));
    list1 = link_inter_code(2, list1, make_code_list(gen_command_code(Read, new_var_op(1))));
    list1 = link_inter_code(2, list1, make_code_list(gen_assign_code(new_var_op(0), new_var_op(1))));
    list1 = link_inter_code(2, list1, make_code_list(gen_assign_code(new_var_op(2), new_const_op(0))));

    InterCodeList *list2 = link_inter_code(4, make_code_list(gen_binop_code(More, new_lbl_op(1), new_var_op(0), new_var_op(2))), make_code_list(gen_binop_code(Less, new_lbl_op(2), new_var_op(0), new_var_op(2))), make_code_list(gen_command_code(Write, new_var_op(2))), make_code_list(gen_command_code(Goto, new_lbl_op(3)))); 

    InterCodeList *list3 = link_inter_code(9, make_code_list(gen_label_code(new_lbl_op(1))), make_code_list(gen_assign_code(new_var_op(3), new_const_op(1))), make_code_list(gen_command_code(Write, new_var_op(3))), make_code_list(gen_command_code(Goto, new_lbl_op(3))), make_code_list(gen_label_code(new_lbl_op(2))), make_code_list(gen_assign_code(new_var_op(6), new_const_op(-1))), make_code_list(gen_command_code(Write, new_var_op(6))), make_code_list(gen_label_code(new_lbl_op(3))), make_code_list(gen_command_code(Return, new_var_op(2))));

    print_inter_code(fopen(argv[1], "w+"), link_inter_code(3, list1, list2, list3));
    
    return 0;
}
