#include <assert.h>

#include <mem/Mem.h>
#include <ir/IR.h>
#include <helpers/Unused.h>
#include <i386/Isel.h>
#include <i386/i386.h>
#include <collections/Set.h>

#include <stdio.h>

int Fibonacci(int n) {
    if (n == 0) {
        return 0;
    } else if (n == 1) {
        return 1;
    } else {
        return (Fibonacci(n - 1) + Fibonacci(n - 2));
    }
} 

int main() {
    assert(MemEmpty());

    IRModule *mod = NewIRModule();
    IRFunc *fib = FuncFromName("Fibonacci");
    
    Node *n = IR.Tmp();
    
    Node *body = IR.Branch(
        IR.Eq(n, IR.I32(0)),
        IR.Ret(IR.I32(0)),
        IR.Branch(
            IR.Eq(n, IR.I32(1)),
            IR.Ret(IR.I32(1)),
            IR.Ret(IR.Add(
                IR.Call(fib),
                IR.Call(fib)))));
            
    DeleteNodeTree(body);
    
    DeleteIRFunc(fib);
    DeleteIRModule(mod);
    
    assert(MemEmpty());
	return 0;
}
