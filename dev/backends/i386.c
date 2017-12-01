#include <assert.h>

#include <mem/Mem.h>
#include <helpers/Types.h>
#include <ir/IR.h>
#include <color/Coloring.h>
#include <color/RIG.h>
#include <collections/Set.h>
#include <collections/Map.h>
#include <collections/List.h>
#include <helpers/Unused.h>

#include <backends/dummy/Dummy.h>
#include <backends/i386/i386.h>

#include <stdio.h>

static void generate_i386(Node *tree);

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
            Nodes.Mov(Nodes.Tmp(), Nodes.Add(
                a,
                b))));
                
    generate_i386(root);
    
    NodeDeleteTree(root);
                
    assert(MemEmpty());
	return 0;
}

static void generate_i386(Node *tree) {
    UNUSED(tree);
}
