#include <assert.h>

#include <mem/Mem.h>

#include <stdlib.h>

int main() {
    assert(MemEmpty());

    assert(!system("./clare"));

    assert(MemEmpty());
	return 0;
}
