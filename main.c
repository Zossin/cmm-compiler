#include <stdio.h>
#include <malloc.h>
#include "syntax_tree_node.h"
#include "symbol_table.h"
#include "semantic_analysis.h"

int main(int argc, char **argv) {
    if (argc <= 1) return 1;
    FILE* f = fopen(argv[1], "r");
    if (!f) {
        perror(argv[1]);
        return 1;
    }
    is_error_happened = FALSE;
    yyrestart(f);
    yyparse();
    if (!is_error_happened) {
        init_symbol_table();
        enter_deeper_scope();

        p_int_type = (Type*)malloc(sizeof(Type));
        p_float_type = (Type*)malloc(sizeof(Type));
        p_int_type->kind = p_float_type->kind = Basic;
        p_int_type->u.basic = Int;
        p_float_type->u.basic = Float;

        insert_type(p_int_type);
        insert_type(p_float_type);

        sdt(syntax_tree_root);
        check_undef_func();
        exit_top_scope();
    }
    destroy_syntax_tree();
    return 0;
}
