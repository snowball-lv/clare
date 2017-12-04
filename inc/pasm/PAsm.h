#pragma once

#include <helpers/Types.h>
#include <ir/IR.h>

#include <stdint.h>
#include <stdio.h>

TYPE_DECL(PAsmModule)

typedef struct {
    int dummy;
    int id;
} PAsmVReg;

HEAP_DECL(PAsmVReg)

typedef struct {
    int dummy;
    PAsmVReg *vreg;
    const char *str;
    int32_t i32;
} PAsmOpr;

#define PASM_OP_MAX_OPRS    2

typedef struct {
    const char *fmt;
    PAsmOpr oprs[PASM_OP_MAX_OPRS];
    PAsmVReg *use[PASM_OP_MAX_OPRS];
    PAsmVReg *def[PASM_OP_MAX_OPRS];
} PAsmOp;

HEAP_DECL(PAsmOp)

void PAsmPrintModule(PAsmModule *mod);

void PAsmModuleAddOp(PAsmModule *mod, PAsmOp *op);

PAsmVReg *PAsmVRegFromTmp(Node *tmp);

void PAsmInit();
void PAsmDeinit();

PAsmVReg *NewPAsmVReg();

void PAsmAllocate(PAsmModule *mod);
