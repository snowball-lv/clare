#include <assert.h>

#include <mem/Mem.h>
#include <ir/IR.h>
#include <helpers/Unused.h>
#include <i386/Isel.h>

#include <stdio.h>

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
                
    Isel *isel = NewIsel();
    
    IselSelect(tree);
    
    DeleteIsel(isel);
    DeleteNodeTree(tree);
    
    assert(MemEmpty());
	return 0;
}
