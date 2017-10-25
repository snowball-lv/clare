#pragma once

#include <ir/IR.h>
#include <helpers/Types.h>
#include <collections/List.h>

#include <stdint.h>

typedef const char Label;

Label *NewLabel();

typedef struct {
    Label *label;
} Opr;

HEAP_DECL(Opr)

#define MAX_OPRS 1

typedef struct {
    int dummy;
    const char *fmt;
    void *oprs[MAX_OPRS];
} Op;

HEAP_DECL(Op)

typedef struct {
    int dummy;
    int index;
} VReg;

VReg *NewVReg();

void OpPrintf(Op *op);
