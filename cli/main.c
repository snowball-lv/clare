

#include <helpers/Unused.h>
#include <ir/IR.h>
#include <mem/Mem.h>
#include <pasm/PAsm.h>
#include <backends/Backends.h>

#include <stdio.h>
#include <assert.h>

static IRModule *SourceToIR();
static PAsmModule *IRToPasm(IRModule *irMod, Backend *backend);

int main(int argc, char **argv) {
    assert(MemEmpty());
    
    UNUSED(argc);
    UNUSED(argv);
    printf("--- clare ---\n");
    
    IRModule *irMod = SourceToIR();
    
    Backend *backend = GetBackend("i386");
    PAsmModule *pasmMod = IRToPasm(irMod, backend);
    
    DeleteIRModule(irMod);
    
    // TODO
    
    DeletePAsmModule(pasmMod);
    
    assert(MemEmpty());
    return 0;
}

static PAsmModule *IRToPasm(IRModule *irMod, Backend *backend) {
    UNUSED(irMod);
    UNUSED(backend);
    PAsmModule *pasmMod = NewPAsmModule();
    return pasmMod;
}

static IRModule *SourceToIR() {
    IRModule *irMod = NewIRModule();
    IRFunction *func = IRModuleNewFunction(irMod, "sum");
    
    Node *a = IR.Tmp();
    Node *b = IR.Tmp();
    Node *c = IR.Tmp();
    
    Node *body = IR.Seq(
        IR.Seq(
            IR.Mov(a, IR.Arg(0)),
            IR.Seq(
                IR.Mov(b, IR.Arg(1)),
                IR.Mov(c, IR.Arg(2)))),
        IR.Ret(IR.Add(a, IR.Add(b, c))));

    IRFunctionSetBody(func, body);
    return irMod;
}
