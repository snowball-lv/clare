#pragma once

#include <ir/IR.h>
#include <helpers/Type.h>

TYPE_DECL(VReg)
TYPE_DECL(Op)

typedef struct {
    int dummy;
    Op *(*Mov)(VReg *dst, VReg *src);
} _i386;

extern _i386 i386;
