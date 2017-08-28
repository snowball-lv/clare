#include <i386/i386.h>

#include <mem/Mem.h>

struct i386_R32 {
    int dummy;
};

typedef i386_R32 R32;

struct i386_Arg {
    int type;
    union {
        R32 *r32;
    };
};

typedef i386_Arg Arg;

struct i386_Instr {
    int type;
    Arg *dst;
    Arg *src;
};

typedef i386_Instr Instr;

#define I386_I_NONE     0
#define I386_I_PUSH     1
#define I386_I_MOV      2

static Instr *NewInstr() {
    Instr *instr = ALLOC(Instr);
    instr->type = I386_I_NONE;
    instr->dst = 0;
    instr->src = 0;
    return instr;
}

static Instr *InstrFromType(int type) {
    Instr *instr = NewInstr();
    instr->type = type;
    return instr;
}

static Instr *InstrFromSrc(int type, Arg *src) {
    Instr *instr = InstrFromType(type);
    instr->src = src;
    return instr;
}

static Instr *InstrFromDstSrc(int type, Arg *dst, Arg *src) {
    Instr *instr = InstrFromSrc(type, src);
    instr->dst = dst;
    return instr;
}

static Instr *push(Arg *src) {
    return InstrFromSrc(I386_I_MOV, src);
}

static Instr *mov(Arg *dst, Arg *src) {
    return InstrFromDstSrc(I386_I_MOV, dst ,src);
}

_i386 i386 = {
    .push = push,
    .mov = mov
};
