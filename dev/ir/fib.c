#include <assert.h>

#include <mem/Mem.h>
#include <ir/IR.h>
#include <helpers/Unused.h>
#include <i386/Isel.h>
#include <i386/i386.h>
#include <collections/Set.h>
#include <collections/List.h>

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

static Node *FiBBody() {
    return IR.Branch(IR.Eq(IR.Arg(0), IR.I32(0)),
        IR.Ret(IR.I32(0)),
        IR.Branch(IR.Eq(IR.Arg(0), IR.I32(1)),
            IR.Ret(IR.I32(1)),
            IR.Ret(IR.Add(
                IR.Call("Fibonacci", TO_LIST(
                    IR.Add(IR.Arg(0), IR.I32(-1))
                )),
                IR.Call("Fibonacci", TO_LIST(
                    IR.Add(IR.Arg(0), IR.I32(-2))
                ))))));
}

int main() {
    assert(MemEmpty());

    Node *body = FiBBody();
    UNUSED(body);
    
    Isel *isel = NewIsel();
    List *ops = IselSelect(isel, body);
    
    printf("-----------------------------\n");
    
    printf("op count: %d\n", ListSize(ops));
    LIST_EACH(ops, Op *, op, {
        OpPrintf(op);
    });
    
    assert(MemEmpty());
	return 0;
}
