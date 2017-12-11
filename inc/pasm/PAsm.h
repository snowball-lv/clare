#pragma once

#include <helpers/Types.h>
#include <ir/IR.h>
#include <collections/Set.h>
#include <collections/Map.h>
#include <backend/Backend.h>

#include <stdint.h>
#include <stdio.h>

// TODO: fix kludge for PAsmModule and Backend
// referencing each other

typedef struct Backend Backend;

TYPE_DECL(PAsmModule)

typedef struct {
    int id;
    int special;
} PAsmVReg;

HEAP_DECL(PAsmVReg)

typedef struct {
    int dummy;
    PAsmVReg *vreg;
    const char *str;
    int32_t i32;
    float f;
} PAsmOpr;

#define PASM_OP_MAX_OPRS    3

typedef struct {
    const char *fmt;
    PAsmOpr oprs[PASM_OP_MAX_OPRS];
    PAsmVReg *use[PASM_OP_MAX_OPRS];
    PAsmVReg *def[PASM_OP_MAX_OPRS];
} PAsmOp;

HEAP_DECL(PAsmOp)

void PAsmModuleAddOp(PAsmModule *mod, PAsmOp *op);

PAsmVReg *NewPAsmVReg();
PAsmVReg *NewSpecialPAsmVReg();

void PAsmAllocate(PAsmModule *mod); 

PAsmModule *PAsmModuleFromBackend(Backend *backend);

void PrintPAsmModule(PAsmModule *mod, FILE *output);
