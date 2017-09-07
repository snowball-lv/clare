#include <assert.h>

#include <mem/Mem.h>
#include <ir/IR.h>

#include <stdio.h>

int main() {
    assert(MemEmpty());

    ir_dummy();

    printf("ir dummy value: %d\n", IR.dummy);

    Exp *e = IR.Mov(
        IR.NewTmp(),
        IR.Add(
            IR.I32(2),
            IR.Add(
                IR.I32(3),
                IR.I32(4))));

    IR.DeleteTree(e);

    assert(MemEmpty());
	return 0;
}
