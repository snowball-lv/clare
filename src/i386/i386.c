#include <i386/i386.h>

#include <helpers/Unused.h>
#include <mem/Mem.h>

struct VReg {
    int dummy;
};

VReg *NewVReg() {
    VReg *vreg = ALLOC(VReg);
    vreg->dummy = 0;
    return vreg;
}

void DeleteVReg(VReg *vreg) {
    MemFree(vreg);
}

struct Op {
    int dummy;
    VReg *dst;
    VReg *src;
};

static Op *NewOp() {
    Op *op = ALLOC(Op);
    op->dummy = 0;
    op->dst = 0;
    op->src = 0;
    return op;
}

static Op *NewOp2(VReg *dst, VReg *src) {
    Op *op = NewOp();
    op->dst = dst;
    op->src = src;
    return op;
}

void DeleteOp(Op *op) {
    if (op->dst != 0) DeleteVReg(op->dst);
    if (op->src != 0) DeleteVReg(op->src);
    MemFree(op);
}

static Op *Mov(VReg *dst, VReg *src) {
    return NewOp2(dst, src);
}

_i386 i386 = {
    .dummy = 0,
    .Mov = Mov
};
