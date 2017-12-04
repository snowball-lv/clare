#include <backends/i386/i386.new.h>

#include <helpers/Unused.h>
#include <pasm/PAsm.h>
#include <helpers/Unused.h>

#include <stdio.h>
#include <assert.h>

static PAsmVReg *EAX = 0;
static PAsmVReg *EBX = 0;

static void _Init() {
    EAX = NewPAsmVReg(); EAX->backend = 1;
    EBX = NewPAsmVReg(); EBX->backend = 1;
}

static void _Deinit() {
    MemFree(EAX);
    MemFree(EBX);
}

#define MANGLE(name)     i386_v2_ ## name
#define RULE_FILE       <backends/i386/i386.new.rules>
#define RET_TYPE        PAsmVReg *
#define RET_DEFAULT     0
    #include <ir/muncher.new.def>
#undef RET_DEFAULT
#undef RET_TYPE
#undef RULE_FILE
#undef MANGLE

static void _Select(PAsmModule *mod, IRFunction *func) {
    #define OP(...)     HEAP(PAsmOp, __VA_ARGS__)
    #define EMIT(...)   PAsmModuleAddOp(mod, OP(__VA_ARGS__))
    
    const char *name = IRFunctionName(func);
    
    EMIT({ 
        .fmt = ";--- start of function: [$str] ---\n",
        .oprs[0] = { .str = name }
    });
    
    EMIT({ .fmt = "section .text\n" });
    EMIT({ 
        .fmt = "global $str\n",
        .oprs[0] = { .str = name }
    });
    EMIT({ 
        .fmt = "$str:\n",
        .oprs[0] = { .str = name }
    });
    
    // TODO
    List *ops = i386_v2_Munch(IRFunctionBody(func));
    LIST_EACH(ops, PAsmOp *, op, {
        PAsmModuleAddOp(mod, op);
    });
    DeleteList(ops);
    
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
    .Init = _Init,
    .Deinit = _Deinit
};
