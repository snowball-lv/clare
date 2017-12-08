#include <backends/i386/i386.new.h>

#include <helpers/Unused.h>
#include <pasm/PAsm.h>
#include <helpers/Unused.h>

#include <stdio.h>
#include <assert.h>

static Set *_ColorSet = 0;
static Map *_PrecoloringMap = 0;

static PAsmVReg *EAX = 0;
static PAsmVReg *EBP = 0;
static PAsmVReg *ESP = 0;

static void _Init() {
    
    EAX = NewSpecialPAsmVReg();
    EBP = NewSpecialPAsmVReg();
    ESP = NewSpecialPAsmVReg();
    
    _ColorSet = NewSet();
    
    SetAdd(_ColorSet, "eax");
    SetAdd(_ColorSet, "ebx");
    SetAdd(_ColorSet, "ecx");
    SetAdd(_ColorSet, "edx");
    
    SetAdd(_ColorSet, "esi");
    SetAdd(_ColorSet, "edi");
    SetAdd(_ColorSet, "ebp");
    SetAdd(_ColorSet, "esp");
    
    _PrecoloringMap = NewMap();
    
    MapPut(_PrecoloringMap, EAX, "eax");
    MapPut(_PrecoloringMap, EBP, "ebp");
    MapPut(_PrecoloringMap, ESP, "esp");
}

static void _Deinit() {
    
    MemFree(EAX);
    MemFree(EBP);
    MemFree(ESP);
        
    DeleteSet(_ColorSet);
    DeleteMap(_PrecoloringMap);
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
    
    EMIT({ 
        .fmt = "push $vr\n",
        .oprs[0] = { .vreg = EBP },
        .use = { EBP }
    });
    EMIT({
        .fmt = "mov $vr, $vr\n",
        .oprs[0] = { .vreg = EBP },
        .oprs[1] = { .vreg = ESP },
        .use = { ESP },
        .def = { EBP }
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

static Set *_Colors() {
    return _ColorSet;
}

static Map *_Precoloring() {
    return _PrecoloringMap;
}

static PAsmModule *_IRToPAsmModule(IRModule *irMod) {

    Backend *backend = &i386_Backend;
    PAsmModule *pasmMod = PAsmModuleFromBackend(backend);
    
    #define OP(...)     HEAP(PAsmOp, __VA_ARGS__)
    #define EMIT(...)   PAsmModuleAddOp(pasmMod, OP(__VA_ARGS__))
    
    EMIT({ .fmt = ";--- start of module ---\n" });
    EMIT({ .fmt = "\n" });
    
    List *funcs = IRModuleFunctions(irMod);
    LIST_EACH(funcs, IRFunction *, func, {
        backend->Select(pasmMod, func);
    });
    DeleteList(funcs);
    
    EMIT({ .fmt = "\n" });
    EMIT({ .fmt = ";--- end of module ---\n" });
    
    return pasmMod;
    
    #undef EMIT
    #undef OP
}

Backend i386_Backend = {
    .dummy = 0,
    .Select = _Select,
    .Init = _Init,
    .Deinit = _Deinit,
    .Colors = _Colors,
    .Precoloring = _Precoloring,
    .IRToPAsmModule = _IRToPAsmModule
};
