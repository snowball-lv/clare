#pragma once

#include <ir/IR.h>
#include <helpers/Types.h>
#include <collections/List.h>

#include <stdint.h>

typedef const char Label;

Label *NewLabel();
Label *NewLabelNamed(const char *name);

typedef union {
    Label *label;
    int index;
    int32_t i32;
} Opr;

HEAP_DECL(Opr)

#define MAX_OPRS 2

typedef struct {
    int dummy;
    const char *fmt;
    Opr oprs[MAX_OPRS];
} Op;

HEAP_DECL(Op)

typedef struct {
    int dummy;
    int index;
} VReg;

VReg *NewVReg();

void OpPrintf(Op *op);
