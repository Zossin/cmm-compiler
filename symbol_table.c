#include "symbol_table.h"

unsigned int hash_pjw(char *name) {
    unsigned int val = 0, i;
    for (; *name; ++ name) {
        val = (val << 2) + *name;
        if (i = val & ~0x3fff) val = (val ^ (i >> 2)) & 0x3fff;
    }
    return val;
}
