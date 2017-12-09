#include <assert.h>

#include <mem/Mem.h>

#include <stdlib.h>

int main() {
    assert(MemEmpty());
    
    assert(!system("./clare             \
    -f clare-ir                         \
    -b i386                             \
    -i ../dev/cli/example.a.clare.ir    \
    -o output.asm"));

    assert(MemEmpty());
	return 0;
}
