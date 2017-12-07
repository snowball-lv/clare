#include <assert.h>

#include <mem/Mem.h>

#include <stdlib.h>

int main() {
    assert(MemEmpty());
    
    assert(!system("./clare -f clare-ir -b i386 example.a.clare.ir"));

    assert(MemEmpty());
	return 0;
}
