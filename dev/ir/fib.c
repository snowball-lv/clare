#include <assert.h>

#include <ir/IR.h>
#include <helpers/Unused.h>

#include <stdio.h>

#include <Backends.h>

int Fibonacci(int n) {
    if (n == 0) {
        return 0;
    } else if (n == 1) {
        return 1;
    } else {
        return (Fibonacci(n - 1) + Fibonacci(n - 2));
    }
} 

Node *FiBBody() {
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
    
    assert(MemEmpty());
	return 0;
}
