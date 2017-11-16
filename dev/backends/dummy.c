#include <assert.h>

#include <mem/Mem.h>
#include <helpers/Types.h>
#include <ir/IR.h>
#include <backends/dummy/Dummy.h>

void DeleteTree(Node *root) {
    for (int i = 0; i < IR_MAX_KIDS; i++) {
        Node *child = root->kids[i];
        if (child != 0) {
            DeleteTree(child);
        }
    }
    MemFree(root);
}

int main() {
    assert(MemEmpty());
    
    Node *root = Nodes.Mov(
        Nodes.Tmp(),
        Nodes.Add(
            Nodes.I32(1),
            Nodes.I32(2)));
    
    DummyMunch(root);
    
    DeleteTree(root);
    
    assert(MemEmpty());
	return 0;
}
