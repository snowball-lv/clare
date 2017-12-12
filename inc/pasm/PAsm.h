#pragma once

#include <helpers/Types.h>
#include <ir/IR.h>
#include <collections/Set.h>
#include <collections/Map.h>
#include <backend/Backend.h>
#include <color/Coloring.h>

#include <stdint.h>
#include <stdio.h>

// TODO: fix kludge for PAsmModule and Backend
// referencing each other

typedef struct Backend Backend;

TYPE_DECL(PAsmModule)

typedef struct PAsmFunction PAsmFunction;
struct PAsmFunction {
    int dummy;
    List *header;
    List *body;
    List *footer;
    int stack_space;
    Coloring *coloring;
};
PAsmFunction *NewPAsmFunction();
void DeletePAsmFunction(PAsmFunction *self);

typedef struct PAsmVReg PAsmVReg;
struct PAsmVReg {
    int id;
    int special;
};

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

void PAsmModuleAddFunc(PAsmModule *mod, PAsmFunction *func);

PAsmVReg *NewPAsmVReg();
PAsmVReg *NewSpecialPAsmVReg();

void PAsmAllocate(PAsmModule *mod); 
int PAsmSpill(PAsmModule *mod); 

PAsmModule *PAsmModuleFromBackend(Backend *backend);

void PrintPAsmModule(PAsmModule *mod, FILE *output);
