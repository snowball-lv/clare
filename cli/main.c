

#include <helpers/Unused.h>
#include <ir/IR.h>
#include <mem/Mem.h>
#include <pasm/PAsm.h>
#include <backends/Backends.h>
#include <collections/List.h>

#include <stdio.h>
#include <assert.h>

static IRModule *SourceToIR();
static PAsmModule *IRToPasm(IRModule *irMod, Backend *backend);

int main(int argc, char **argv) {
    assert(MemEmpty());
    
    PAsmInit();
    
    UNUSED(argc);
    UNUSED(argv);
    printf("--- clare ---\n");
    
    IRModule *irMod = SourceToIR();
    
    Backend *backend = GetBackend("i386");
    backend->Init();
    
    PAsmModule *pasmMod = IRToPasm(irMod, backend);
    
    // TODO
    PAsmPrintModule(pasmMod);
    PAsmAllocate(pasmMod);
    PAsmPrintModule(pasmMod);
    
    DeletePAsmModule(pasmMod);
    DeleteIRModule(irMod);
    
    PAsmDeinit();
    
    backend->Deinit();
    
    assert(MemEmpty());
    return 0;
}

static PAsmModule *IRToPasm(IRModule *irMod, Backend *backend) {
    
    PAsmModule *pasmMod = PAsmModuleFromBackend(backend);
    
    List *funcs = IRModuleFunctions(irMod);
    LIST_EACH(funcs, IRFunction *, func, {
        backend->Select(pasmMod, func);
    });
    DeleteList(funcs);
    
    return pasmMod;
}

static IRModule *SourceToIR() {
    IRModule *irMod = NewIRModule();
    IRFunction *func = IRModuleNewFunction(irMod, "sum3");
    
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
        
    printf("--- sum3 ---\n");
    IRPrintTree(body);
    printf("------------\n");

    IRFunctionSetBody(func, body);
    return irMod;
}
