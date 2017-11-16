#include <assert.h>

#include <mem/Mem.h>
#include <helpers/Types.h>
#include <ir/IR.h>
#include <backends/dummy/Dummy.h>

#include <stdio.h>

int main() {
    assert(MemEmpty());
    
    Node *a = Nodes.Tmp();
    Node *b = Nodes.Tmp();
    
    Node *root = Nodes.Seq(
        Nodes.Mov(a, Nodes.Add(
                Nodes.I32(1),
                Nodes.I32(2))),
        Nodes.Seq(
            Nodes.Mov(b, Nodes.Add(
                    Nodes.I32(3),
                    Nodes.I32(4))),
            Nodes.Add(a, b)));
    
    DummyMunch(root);
    
    NodeDeleteTree(root);
    
    printf("vm top: %d\n", DummyVMTop());
    
    assert(MemEmpty());
	return 0;
}
