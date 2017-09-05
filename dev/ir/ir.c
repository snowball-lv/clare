#include <assert.h>

#include <mem/Mem.h>
#include <ir/IR.h>

int main() {
    assert(MemEmpty());

    ir_dummy();

    assert(MemEmpty());
	return 0;
}
