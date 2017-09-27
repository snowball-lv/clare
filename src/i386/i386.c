#include <i386/i386.h>

#include <helpers/Unused.h>
#include <mem/Mem.h>

TYPE_DEF(VReg, { int dummy; }, {}, {})

TYPE_DEF(Op, {
    VReg *dst;
    VReg *src;
}, {
    self->dst = 0;
    self->src = 0;
}, {
    if (self->dst != 0) DeleteVReg(self->dst);
    if (self->src != 0) DeleteVReg(self->src);
})

static Op *NewOp2(VReg *dst, VReg *src) {
    Op *op = NewOp();
    op->dst = dst;
    op->src = src;
    return op;
}

static Op *Mov(VReg *dst, VReg *src) {
    return NewOp2(dst, src);
}

_i386 i386 = {
    .dummy = 0,
    .Mov = Mov
};
