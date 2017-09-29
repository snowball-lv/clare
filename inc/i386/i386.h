#pragma once

#include <ir/IR.h>
#include <helpers/Type.h>
#include <collections/List.h>

#include <stdint.h>

TYPE_DECL(VReg)
TYPE_DECL(Op)

VReg *OpDst(Op *op);
VReg *OpSrc(Op *op);

typedef struct {
    
    int dummy;
    
    Op *(*Mov)(VReg *dst, VReg *src);
    Op *(*MovI32)(VReg *dst, int32_t i32);
    Op *(*Add)(VReg *dst, VReg *src);
    
    void (*Print)(List *ops);
    
} _i386;

extern _i386 i386;
