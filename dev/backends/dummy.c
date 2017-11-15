#include <assert.h>

#include <mem/Mem.h>
#include <helpers/Types.h>
#include <ir/IR.h>
#include <backends/dummy/Dummy.h>

int main() {
    assert(MemEmpty());
    
    Node *root = Nodes.Any();
    DummyMunch(root);
    MemFree(root);
    
    assert(MemEmpty());
	return 0;
}
