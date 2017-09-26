#include <assert.h>

#include <mem/Mem.h>
#include <ir/IR.h>

#include <stdio.h>

#define UNUSED(x) (void)(x)

void Tile(Node *root) {

    #define RULE(pattern, action)           \
        if (NodeMatches(root, pattern)) {   \
            action;                         \
            return;                         \
        }

        #include "test.rules"

    #undef RULE

    printf("No rules match node: %s\n", NodeTypeName(root));
    assert(0);
}

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

    Tile(tree);

    assert(MemEmpty());
	return 0;
}
