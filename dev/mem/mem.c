#include <assert.h>

#include <clare/mem/Mem.h>

int main() {
    assert(MemEmpty());

    void *ptrs[3];

    for (int i = 0; i < 3; i++) {
        ptrs[i] = MemAlloc(1);
        assert(!MemEmpty());
    }

    for (int i = 0; i < 3; i++) {
        assert(!MemEmpty());
        MemFree(ptrs[i]);
    }

    assert(MemEmpty());
	return 0;
}
