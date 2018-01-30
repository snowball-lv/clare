#include <clare/backends/i386/i386.h>

#include <clare/helpers/Unused.h>
#include <clare/helpers/Error.h>
#include <clare/pasm/PAsm.h>
#include <clare/helpers/Unused.h>

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct {
    List *ops;
    Map *map;
    int label_counter;
    List *data_labels;
    List *header;
    List *footer;
} MunchState;

static void InitState(MunchState *state) {
    state->ops = NewList();
    state->map = NewMap();
    state->label_counter = 1;
    state->data_labels = NewList();
    state->footer = NewList();
    state->header = NewList();
}

static void DeinitState(MunchState *state) {
    DeleteList(state->ops);
    DeleteMap(state->map);
    LIST_EACH(state->data_labels, char *, str, {
        MemFree(str);
    });
    DeleteList(state->data_labels);
    DeleteList(state->footer);
    DeleteList(state->header);
}

static int NextLabel(MunchState *state) {
    return state->label_counter++;
}

static Set *_ColorSet = 0;
static Map *_PrecoloringMap = 0;

static PAsmVReg *EAX = 0;
static PAsmVReg *ECX = 0;
static PAsmVReg *EDX = 0;

static PAsmVReg *EBX = 0;
static PAsmVReg *ESI = 0;
static PAsmVReg *EDI = 0;

static PAsmVReg *GetOrCreateVReg(MunchState *state, Node *tmp) {
    if (!MapContains(state->map, tmp)) {
        MapPut(state->map, tmp, NewPAsmVReg());
    }
    return MapGet(state->map, tmp);
}

static void _Init() {
    
    EAX = NewSpecialPAsmVReg();
    ECX = NewSpecialPAsmVReg();
    EDX = NewSpecialPAsmVReg();
    
    EBX = NewSpecialPAsmVReg();
    ESI = NewSpecialPAsmVReg();
    EDI = NewSpecialPAsmVReg();
    
    _ColorSet = NewSet();
    
    SetAdd(_ColorSet, "eax");
    SetAdd(_ColorSet, "ecx");
    SetAdd(_ColorSet, "edx");
    
    SetAdd(_ColorSet, "ebx");
    SetAdd(_ColorSet, "esi");
    SetAdd(_ColorSet, "edi");
    
    _PrecoloringMap = NewMap();
    
    MapPut(_PrecoloringMap, EAX, "eax");
    MapPut(_PrecoloringMap, EDX, "edx");
    MapPut(_PrecoloringMap, ECX, "ecx");
    
    MapPut(_PrecoloringMap, EBX, "ebx");
    MapPut(_PrecoloringMap, ESI, "esi");
    MapPut(_PrecoloringMap, EDI, "edi");
}

static void _Deinit() {
    
    MemFree(EAX);
    MemFree(EDX);
    MemFree(ECX);
    
    MemFree(EBX);
    MemFree(ESI);
    MemFree(EDI);
        
    DeleteSet(_ColorSet);
    DeleteMap(_PrecoloringMap);
}

#define MANGLE(name)    i386_text_ ## name
#define RULE_FILE       <clare/backends/i386/i386.text.rules>
#define RET_TYPE        PAsmVReg *
#define RET_DEFAULT     0
#define STATE_T         MunchState *
#define EMIT(op)        ListAdd(state->ops, op)
#include <clare/ir/muncher.def>
#undef EMIT
#undef STATE_T
#undef RET_DEFAULT
#undef RET_TYPE
#undef RULE_FILE
#undef MANGLE

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
    pasmFunc->name = name;
    
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
    
    MunchState state;
    InitState(&state);
    i386_text_Munch(IRFunctionBody(func), &state);
    
    LIST_EACH(state.ops, PAsmOp *, op, {
        ListAdd(pasmFunc->body, op);
    });
    LIST_EACH(state.footer, PAsmOp *, op, {
        ListAdd(pasmFunc->footer, op);
    });
    LIST_EACH(state.header, PAsmOp *, op, {
        ListAdd(pasmFunc->header, op);
    });
    
    DeinitState(&state);
    
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
        .fmt = "mov $vr, [ebp - $i32]    ;load $id\n",
        .oprs[0] = { .vreg = tmp },
        .oprs[1] = { .i32 = offset },
        .oprs[2] = { .vreg = spill },
        .def = { tmp }
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
        .fmt = "mov [ebp - $i32], $vr    ;store $id\n",
        .oprs[0] = { .i32 = offset },
        .oprs[1] = { .vreg = tmp },
        .oprs[2] = { .vreg = spill },
        .use = { tmp }
    });
    
    #undef EMIT
    #undef OP
}

static void *PRegColor(PAsmVReg *preg) {
    return MapGet(_PrecoloringMap, preg);
}

static List *_GenPrologue(PAsmFunction *func) {
    
    List *ops = NewList();
    
    #define OP(...)     HEAP(PAsmOp, __VA_ARGS__)
    #define EMIT(...)   ListAdd(ops, OP(__VA_ARGS__))
    
    Set *definedPRegs = NewSet();

    Coloring *coloring = func->coloring;
    LIST_EACH(func->body, PAsmOp *, op, {
        for (int i = 0; i < PASM_OP_MAX_OPRS; i++) {
            PAsmVReg *vreg = op->def[i];
            if (vreg != 0) {
                SetAdd(definedPRegs, ColoringGetColor(coloring, vreg));
            }
        }
    });
    
    EMIT({ .fmt = ";-------- prologue\n" });
    
    EMIT({ .fmt = "push ebp\n" });
    EMIT({ .fmt = "mov ebp, esp\n" });
    
    EMIT({ 
        .fmt = "sub esp, $i32\n",
        .oprs[0] = { .i32 = func->stack_space }
    });
    
    // save callee-saved registers used in this function
    if (SetContains(definedPRegs, PRegColor(EBX))) {
        EMIT({ .fmt = "push ebx\n" });
    }
    if (SetContains(definedPRegs, PRegColor(ESI))) {
        EMIT({ .fmt = "push esi\n" });
    }
    if (SetContains(definedPRegs, PRegColor(EDI))) {
        EMIT({ .fmt = "push edi\n" });
    }
    
    EMIT({ .fmt = ";-------- /prologue\n" });
    
    
    DeleteSet(definedPRegs);
    
    return ops;
    
    #undef EMIT
    #undef OP
}

static List *_GenEpilogue(PAsmFunction *func) {
    
    List *ops = NewList();

    #define OP(...)     HEAP(PAsmOp, __VA_ARGS__)
    #define EMIT(...)   ListAdd(ops, OP(__VA_ARGS__))
    
    Set *definedPRegs = NewSet();
    
    Coloring *coloring = func->coloring;
    LIST_EACH(func->body, PAsmOp *, op, {
        for (int i = 0; i < PASM_OP_MAX_OPRS; i++) {
            PAsmVReg *vreg = op->def[i];
            if (vreg != 0) {
                SetAdd(definedPRegs, ColoringGetColor(coloring, vreg));
            }
        }
    });

    EMIT({ .fmt = ";-------- epilogue\n" });
    
    EMIT({ .fmt = ".epilogue:\n" });
    
    // restore callee-saved registers used in this function
    // reverse order from prologue
    if (SetContains(definedPRegs, PRegColor(EDI))) {
        EMIT({ .fmt = "pop edi\n" });
    }
    if (SetContains(definedPRegs, PRegColor(ESI))) {
        EMIT({ .fmt = "pop esi\n" });
    }
    if (SetContains(definedPRegs, PRegColor(EBX))) {
        EMIT({ .fmt = "pop ebx\n" });
    }
    
    EMIT({ .fmt = "mov esp, ebp\n" });
    EMIT({ .fmt = "pop ebp\n" });
    EMIT({ .fmt = "ret\n" });

    EMIT({ .fmt = ";-------- /epilogue\n" });
    
    DeleteSet(definedPRegs);
    
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
