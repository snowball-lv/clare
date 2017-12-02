

#include <helpers/Unused.h>
#include <ir/IR.h>
#include <mem/Mem.h>
#include <pasm/PAsm.h>

#include <stdio.h>
#include <assert.h>

static IRModule *SourceToIR();
static PAsmModule *IRToPAsm(IRModule *irMod);

int main(int argc, char **argv) {
    assert(MemEmpty());
    
    UNUSED(argc);
    UNUSED(argv);
    printf("--- clare ---\n");
    
    IRModule *irMod = SourceToIR();
    PAsmModule *pasmMod = IRToPAsm(irMod);
    DeleteIRModule(irMod);

    // TODO
    
    DeletePAsmModule(pasmMod);
    
    assert(MemEmpty());
    return 0;
}

static IRModule *SourceToIR() {
    return NewIRModule();
}

static PAsmModule *IRToPAsm(IRModule *irMod) {
    UNUSED(irMod);
    PAsmModule *mod = NewPAsmModule();
    return mod;
}
