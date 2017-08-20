#include <assert.h>
#include <string.h>

#include <mem/Mem.h>
#include <pasm/i386_IPasm.h>

int main() {
    assert(MemEmpty());

    const char *v = i386_IPasm.version();
    assert(strcmp(v, "i386-pasm-dev") == 0);

    assert(MemEmpty());
	return 0;
}
