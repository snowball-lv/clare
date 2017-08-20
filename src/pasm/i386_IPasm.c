#include <pasm/i386_IPasm.h>

#include <mem/Mem.h>

struct i386_Instr {
    int dummy;
};

typedef i386_Instr Instr;

static Instr *NewInstr() {
    Instr *instr = ALLOC(Instr);
    instr->dummy = 0;
    return instr;
}

static void DeleteInstr(void *instr) {
    MemFree(instr);
}

/*** i386 instructions gen ***/

static Instr *nop() {
    return NewInstr();
}

_i386 i386 = {
    .nop = nop
};

/*** IPasm implementations ***/

static const char *version() {
    return "i386-pasm-dev";
}

IPasm i386_IPasm = {
    .version = version,
    .DeleteInstr = DeleteInstr
};