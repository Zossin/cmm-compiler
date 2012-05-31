#ifndef _GEN_CODE_H_
#define _GEN_CODE_H_

#include "inter_code.h"
#include <stdio.h>
#include "common.h"

void gen_code(FILE *file, InterCodeList *codes);

#define NR_VAR_PER_STACK_FRAME 100

int offset_now;

typedef struct {
    int var_id;
    BOOL is_modified;
} reg_info;

#define NR_REGS 18

reg_info regs[NR_REGS];

#endif