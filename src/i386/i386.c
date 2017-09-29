#include <i386/i386.h>

#include <helpers/Unused.h>
#include <mem/Mem.h>

#include <stdio.h>
#include <stdint.h>

static int VREG_COUNTER = 1;

TYPE_DEF(VReg, { 
    int id; 
}, {
    self->id = VREG_COUNTER;
    VREG_COUNTER++;
}, {})

#define OP_TYPE_NONE    1
#define OP_TYPE_MOV     2
#define OP_TYPE_MOV_I32 3
#define OP_TYPE_ADD     4

TYPE_DEF(Op, {
    int type;
    VReg *dst;
    VReg *src;
    int32_t i32;
}, {
    self->type = OP_TYPE_NONE;
    self->dst = 0;
    self->src = 0;
    self->i32 = 0;
}, {})

VReg *OpDst(Op *op) {
    return op->dst;
}

VReg *OpSrc(Op *op) {
    return op->src;
}

static Op *OpFromType(int type) {
    Op *op = NewOp();
    op->type = type;
    return op;
}

static Op *NewOp2(int type, VReg *dst, VReg *src) {
    Op *op = OpFromType(type);
    op->dst = dst;
    op->src = src;
    return op;
}

static Op *Mov(VReg *dst, VReg *src) {
    return NewOp2(OP_TYPE_MOV, dst, src);
}

static Op *MovI32(VReg *dst, int32_t i32) {
    Op *op = NewOp2(OP_TYPE_MOV_I32, dst, 0);
    op->i32 = i32;
    return op;
}

static Op *Add(VReg *dst, VReg *src) {
    return NewOp2(OP_TYPE_ADD, dst, src);
}

static void PrintOp(Op *op) {
    switch (op->type) {
        
        case OP_TYPE_MOV: 
            printf(
                "mov $%d, $%d\n",
                op->dst->id,
                op->src->id);
        break;
        
        case OP_TYPE_MOV_I32: 
            printf(
                "mov $%d, %d\n",
                op->dst->id,
                op->i32);
        break;
        
        case OP_TYPE_ADD: 
            printf(
                "add $%d, $%d\n",
                op->dst->id,
                op->src->id);
        break;
    }
}

static void Print(List *ops) {
    printf(";print start\n");
    LIST_EACH(ops, Op *, op, {
        PrintOp(op);
    });
    printf(";print end\n");
}

_i386 i386 = {
    .dummy = 0,
    .Mov = Mov,
    .MovI32 = MovI32,
    .Add = Add,
    .Print = Print
};
