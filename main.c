#include <stdio.h>
#include "syntax_tree_node.h"
#include "symbol_table.h"

extern syntax_tree_node *syntax_tree_root;

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
    if (!is_error_happened)
        sdt(syntax_tree_root);
    return 0;
}
