#pragma once

#include <ir/IR.h>

typedef struct VReg VReg;

VReg *NewVReg();
void DeleteVReg(VReg *vreg);

typedef struct Op Op;

void DeleteOp(Op *op);

typedef struct {
    int dummy;
    Op *(*Mov)(VReg *dst, VReg *src);
} _i386;

extern _i386 i386;
