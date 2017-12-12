#include <backends/i386/i386.h>

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

static PAsmVReg *EDX = 0;
static PAsmVReg *ECX = 0;

static Map *_TmpToVregMap = 0;

static void PAsmInit() {
    _TmpToVregMap = NewMap();
}

static void PAsmDeinit() {
    DeleteMap(_TmpToVregMap);
}

static PAsmVReg *PAsmVRegFromTmp(Node *tmp) {
    if (!MapContains(_TmpToVregMap, tmp)) {
        MapPut(_TmpToVregMap, tmp, NewPAsmVReg());
    }
    return MapGet(_TmpToVregMap, tmp);
}

static void _Init() {
    
    PAsmInit();
    
    EAX = NewSpecialPAsmVReg();
    EBP = NewSpecialPAsmVReg();
    ESP = NewSpecialPAsmVReg();
    
    EDX = NewSpecialPAsmVReg();
    ECX = NewSpecialPAsmVReg();
    
    _ColorSet = NewSet();
    
    SetAdd(_ColorSet, "eax");
    // SetAdd(_ColorSet, "ebx");
    SetAdd(_ColorSet, "ecx");
    SetAdd(_ColorSet, "edx");
    
    // SetAdd(_ColorSet, "esi");
    // SetAdd(_ColorSet, "edi");
    // SetAdd(_ColorSet, "ebp");
    // SetAdd(_ColorSet, "esp");
    
    _PrecoloringMap = NewMap();
    
    MapPut(_PrecoloringMap, EAX, "eax");
    MapPut(_PrecoloringMap, EBP, "ebp");
    MapPut(_PrecoloringMap, ESP, "esp");
    
    MapPut(_PrecoloringMap, EDX, "edx");
    MapPut(_PrecoloringMap, ECX, "ecx");
}

static void _Deinit() {
    
    MemFree(EAX);
    MemFree(EBP);
    MemFree(ESP);
    
    MemFree(EDX);
    MemFree(ECX);
        
    DeleteSet(_ColorSet);
    DeleteMap(_PrecoloringMap);
    
    PAsmDeinit();
}

static int _Label_Counter = 1;

static int NextLabel() {
    return _Label_Counter++;
}

#define MANGLE(name)     i386_v2_ ## name
#define RULE_FILE       <backends/i386/i386.rules>
#define RET_TYPE        PAsmVReg *
#define RET_DEFAULT     0
    #include <ir/muncher.def>
#undef RET_DEFAULT
#undef RET_TYPE
#undef RULE_FILE
#undef MANGLE

// #define MANGLE(name)     i386_rodata_ ## name
// #define RULE_FILE       <backends/i386/i386.rodata.rules>
// #define RET_TYPE        void *
// #define RET_DEFAULT     0
//     #include <ir/muncher.def>
// #undef RET_DEFAULT
// #undef RET_TYPE
// #undef RULE_FILE
// #undef MANGLE

static Set *_Colors() {
    return _ColorSet;
}

static Map *_Precoloring() {
    return _PrecoloringMap;
}

static PAsmFunction *IRToPAsmFunction(PAsmModule *mod, IRFunction *func) {
    
    UNUSED(mod);
    UNUSED(func);
    
    const char *name = IRFunctionName(func);
    
    PAsmFunction *pasmFunc = NewPAsmFunction();
    
    ListAdd(pasmFunc->header, HEAP(PAsmOp, {
        .fmt = "\n"
    }));
    ListAdd(pasmFunc->header, HEAP(PAsmOp, {
        .fmt = ";-------- start of function: [$str]\n",
        .oprs[0] = { .str = name }
    }));
    ListAdd(pasmFunc->header, HEAP(PAsmOp, {
        .fmt = "section .text\n"
    }));
    ListAdd(pasmFunc->header, HEAP(PAsmOp, {
        .fmt = "global $str\n",
        .oprs[0] = { .str = name }
    }));
    ListAdd(pasmFunc->header, HEAP(PAsmOp, {
        .fmt = "$str:\n",
        .oprs[0] = { .str = name }
    }));
    
    List *ops = i386_v2_Munch(IRFunctionBody(func));
    LIST_EACH(ops, PAsmOp *, op, {
        ListAdd(pasmFunc->body, op);
    });
    DeleteList(ops);
    
    ListAdd(pasmFunc->footer, HEAP(PAsmOp, {
        .fmt = ";-------- end of function: [$str]\n",
        .oprs[0] = { .str = name }
    }));
    ListAdd(pasmFunc->footer, HEAP(PAsmOp, {
        .fmt = "\n"
    }));
    
    return pasmFunc;
}

static PAsmModule *_IRToPAsmModule(IRModule *irMod) {

    PAsmModule *pasmMod = PAsmModuleFromBackend(&i386_Backend);
    
    List *funcs = IRModuleFunctions(irMod);
    LIST_EACH(funcs, IRFunction *, func, {
        PAsmFunction *pasmFunc = IRToPAsmFunction(pasmMod, func);
        PAsmModuleAddFunc(pasmMod, pasmFunc);
    });
    DeleteList(funcs);
    
    return pasmMod;
}

static void _LoadVReg(PAsmFunction *func, PAsmVReg *spill, PAsmVReg *tmp) {
    
    #define OP(...)     HEAP(PAsmOp, __VA_ARGS__)
    #define EMIT(...)   ListAdd(func->body, OP(__VA_ARGS__))
    
    int offset = 0;
    if (MapContains(func->locations, spill)) {
        offset = (int) (intptr_t) MapGet(func->locations, spill);
    } else {
        offset = func->stack_space + 4;
        MapPut(func->locations, spill, (void *) (intptr_t) offset);
        func->stack_space = offset;
    }
    
    EMIT({
        .fmt = "mov $vr, [$vr - $i32]    ;load $id\n",
        .oprs[0] = { .vreg = tmp },
        .oprs[1] = { .vreg = EBP },
        .oprs[2] = { .i32 = offset },
        .oprs[3] = { .vreg = spill },
        .def = { tmp },
        .use = { EBP }
    });
    
    #undef EMIT
    #undef OP
}

static void _StoreVReg(PAsmFunction *func, PAsmVReg *spill, PAsmVReg *tmp) {
    
    #define OP(...)     HEAP(PAsmOp, __VA_ARGS__)
    #define EMIT(...)   ListAdd(func->body, OP(__VA_ARGS__))
    
    
    int offset = 0;
    if (MapContains(func->locations, spill)) {
        offset = (int) (intptr_t) MapGet(func->locations, spill);
    } else {
        offset = func->stack_space + 4;
        MapPut(func->locations, spill, (void *) (intptr_t) offset);
        func->stack_space = offset;
    }
    
    EMIT({
        .fmt = "mov [$vr - $i32], $vr    ;store $id\n",
        .oprs[0] = { .vreg = EBP },
        .oprs[1] = { .i32 = offset },
        .oprs[2] = { .vreg = tmp },
        .oprs[3] = { .vreg = spill },
        .use = { EBP, tmp }
    });
    
    #undef EMIT
    #undef OP
}

static List *_GenPrologue(PAsmFunction *func) {
    
    List *ops = NewList();
    
    #define OP(...)     HEAP(PAsmOp, __VA_ARGS__)
    #define EMIT(...)   ListAdd(ops, OP(__VA_ARGS__))
    
    UNUSED(func);
    
    EMIT({ .fmt = ";-------- prologue\n" });
    
    EMIT({ .fmt = "push ebp\n" });
    EMIT({ .fmt = "mov ebp, esp\n" });
    EMIT({ 
        .fmt = "sub esp, $i32\n",
        .oprs[0] = { .i32 = func->stack_space }
    });
    
    EMIT({ .fmt = ";-------- /prologue\n" });
    
    return ops;
    
    #undef EMIT
    #undef OP
}

static List *_GenEpilogue(PAsmFunction *func) {
    
    List *ops = NewList();

    #define OP(...)     HEAP(PAsmOp, __VA_ARGS__)
    #define EMIT(...)   ListAdd(ops, OP(__VA_ARGS__))

    UNUSED(func);

    EMIT({ .fmt = ";-------- epilogue\n" });

    EMIT({ .fmt = "mov esp, ebp\n" });
    EMIT({ .fmt = "pop ebp\n" });
    EMIT({ .fmt = "ret\n" });

    EMIT({ .fmt = ";-------- /epilogue\n" });

    return ops;

    #undef EMIT
    #undef OP
}

Backend i386_Backend = {
    .dummy = 0,
    // .Select = _Select,
    .Init = _Init,
    .Deinit = _Deinit,
    .Colors = _Colors,
    .Precoloring = _Precoloring,
    .IRToPAsmModule = _IRToPAsmModule,
    
    .LoadVReg = _LoadVReg,
    .StoreVReg = _StoreVReg,
    
    .GenPrologue = _GenPrologue,
    .GenEpilogue = _GenEpilogue,
};
