#include <assert.h>

#include <mem/Mem.h>
#include <helpers/Types.h>
#include <ir/IR.h>
#include <backends/dummy/Dummy.h>

int main() {
    assert(MemEmpty());
    
    Node *root = Nodes.Mov(
        Nodes.Tmp(),
        Nodes.Add(
            Nodes.I32(1),
            Nodes.I32(2)));
    
    DummyMunch(root);
    
    NodeDeleteTree(root);
    
    assert(MemEmpty());
	return 0;
}
