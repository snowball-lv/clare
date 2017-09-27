#include <assert.h>

#include <mem/Mem.h>
#include <ir/IR.h>

#include <stdio.h>
#include <i386/Muncher.h>
#include <helpers/Unused.h>

int main() {
    assert(MemEmpty());

    assert(IR.dummy == 1337);

    Node *tree = IR.Mov(
        IR.Tmp(),
        IR.Add(
            IR.I32(1),
            IR.Add(
                IR.I32(2),
                IR.I32(3))));
    
    DeleteNodeTree(tree);
    
    // I386_Munch(tree);

    assert(MemEmpty());
	return 0;
}
