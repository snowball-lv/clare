#pragma once

#include <ir/IR.h>
#include <helpers/Types.h>
#include <collections/List.h>

#include <stdint.h>

#define OP_TYPE_NONE    1

typedef struct {
    int dummy;
    int op_type;
} Op;

Op *MkOp(Op op);

typedef const char Label;

Label *NewLabel();

typedef struct {
    int dummy;
    int index;
} VReg;

VReg *NewVReg();
