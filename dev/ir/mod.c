#include <assert.h>

#include <mem/Mem.h>
#include <ir/IR.h>
#include <helpers/Unused.h>
#include <i386/Isel.h>
#include <i386/i386.h>
#include <collections/Set.h>
#include <ir/IR-2.h>

#include <stdio.h>

int main() {
    assert(MemEmpty());

    IRModule *mod = NewIRModule();
    DeleteIRModule(mod);
    
    assert(MemEmpty());
	return 0;
}
