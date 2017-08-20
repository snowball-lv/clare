#include <assert.h>
#include <string.h>

#include <mem/Mem.h>
#include <pasm/IPasm.h>
#include <pasm/i386_IPasm.h>

int main() {
    assert(MemEmpty());

    IPasm *ipasm = &i386_IPasm;

    const char *v = ipasm->version();
    assert(strcmp(v, "i386-pasm-dev") == 0);

    void *nop = i386.nop();
    ipasm->DeleteInstr(nop);

    assert(MemEmpty());
	return 0;
}
