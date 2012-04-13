#include <stdio.h>
#include "syntax_tree_node.h"

extern syntax_tree_node *syntax_tree_root;

unsigned char is_error_happened = 0;

int main(int argc, char **argv) {
    if (argc <= 1) return 1;
    FILE* f = fopen(argv[1], "r");
    if (!f) {
        perror(argv[1]);
        return 1;
    }
    yyrestart(f);
    yyparse();
    if (!is_error_happened)
        dfs_syntax_tree(0, syntax_tree_root);
    destory_syntax_tree(syntax_tree_root);
    return 0;
}
