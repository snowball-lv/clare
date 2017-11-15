#include <assert.h>

#include <mem/Mem.h>
#include <helpers/Types.h>
#include <ir/IR.h>

#define NODE(...)   HEAP(Node, __VA_ARGS__)

int main() {
    assert(MemEmpty());
    
    NODE({ .type = NT(None) });
    NODE({ .type = NT(Any) });
    
    // assert(MemEmpty());
	return 0;
}
