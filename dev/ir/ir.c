#include <assert.h>

#include <mem/Mem.h>
#include <ir/IR.h>

#include <stdio.h>

int main() {
    assert(MemEmpty());

    ir_dummy();

    printf("ir dummy value: %d\n", IR.dummy);

    Tmp *tmp = IR.NewTmp();

    IR.Asgn(
        tmp,
        IR.Add(
            IR.I32(2),
            IR.I32(3)));

    IR.DeleteTmp(tmp);

    assert(MemEmpty());
	return 0;
}
