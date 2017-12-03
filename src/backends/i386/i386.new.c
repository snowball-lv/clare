#include <backends/i386/i386.new.h>

#include <helpers/Unused.h>
#include <pasm/PAsm.h>
#include <helpers/Unused.h>

#include <stdio.h>

static void _Select(PAsmModule *mod, IRFunction *func) {
    #define OP(...)     HEAP(PAsmOp, __VA_ARGS__)
    #define EMIT(...)   PAsmModuleAddOp(mod, OP(__VA_ARGS__))
    
    const char *name = IRFunctionName(func);
    
    EMIT({ 
        .fmt = ";--- start of function: [$str] ---\n",
        .oprs[0] = { .str = name }
    });
    
    // TODO
    
    EMIT({
        .fmt = ";--- end of function: [$str] ---\n",
        .oprs[0] = { .str = name }
    });
    
    #undef EMIT
    #undef OP
}

Backend i386_Backend = {
    .dummy = 0,
    .Select = _Select,
};
