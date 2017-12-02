#pragma once

#include <helpers/Types.h>

#include <stdint.h>

TYPE_DECL(PAsmModule)

typedef struct {
    int dummy;
    int id;
} PAsmVReg;

typedef struct {
    int dummy;
    PAsmVReg *vreg;
    int32_t i32;
} PAsmOpr;

#define PASM_OP_MAX_OPRS    1

typedef struct {
    const char *fmt;
    PAsmOpr oprs[PASM_OP_MAX_OPRS];
    PAsmVReg *use[PASM_OP_MAX_OPRS];
    PAsmVReg *def[PASM_OP_MAX_OPRS];
} PAsmOp;

void PAsmPrintOp(PAsmOp *op);
