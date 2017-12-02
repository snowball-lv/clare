

#include <helpers/Unused.h>
#include <ir/IR.h>
#include <mem/Mem.h>
#include <pasm/PAsm.h>
#include <backends/Backend.h>
#include <backends/i386/i386.h>

#include <stdio.h>
#include <assert.h>

static IRModule *SourceToIR();
static Backend *GetBackend();

int main(int argc, char **argv) {
    assert(MemEmpty());
    
    UNUSED(argc);
    UNUSED(argv);
    printf("--- clare ---\n");
    
    IRModule *irMod = SourceToIR();
    
    Backend *backend = GetBackend();
    PAsmModule *pasmMod = PAsmSelect(irMod, backend);

    DeleteIRModule(irMod);

    PAsmPrintModule(pasmMod);
    
    DeletePAsmModule(pasmMod);
    
    assert(MemEmpty());
    return 0;
}

static IRModule *SourceToIR() {
    return NewIRModule();
}

static Backend *GetBackend() {
    return &i386_Backend;
}
