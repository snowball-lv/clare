#include <pasm/PAsm.h>

#include <helpers/Unused.h>
#include <collections/List.h>
#include <collections/Graph.h>
#include <collections/Map.h>
#include <color/RIG.h>
#include <color/Coloring.h>
#include <backend/Backend.h>

#include <string.h>
#include <stdio.h>
#include <assert.h>

HEAP_DEF(PAsmOp)

static void CollectVRegs(List *ops, Set *vregs) {
    LIST_EACH(ops, PAsmOp *, op, {
        for (int i = 0; i < PASM_OP_MAX_OPRS; i++) {
            PAsmOpr *opr = &op->oprs[i];
            if (opr->vreg != 0 && !opr->vreg->special) {
                SetAdd(vregs, opr->vreg);
            }
        }
    });
}

PAsmFunction *NewPAsmFunction() {
    
    PAsmFunction *self = ALLOC(PAsmFunction);
    
    // ctor
    self->header = NewList();
    self->body = NewList();
    self->footer = NewList();

    self->stack_space = 0;

    self->coloring = 0;
    
    self->locations = NewMap();

    return self;
}

void DeletePAsmFunction(PAsmFunction *self) {
    
    // dtor
    Set *vregs = NewSet();
    CollectVRegs(self->header, vregs);
    CollectVRegs(self->body, vregs);
    CollectVRegs(self->footer, vregs);
    SET_EACH(vregs, PAsmVReg *, vreg, {
        MemFree(vreg);
    });
    DeleteSet(vregs);
    
    LIST_EACH(self->header, PAsmOp *, op, {
        MemFree(op);
    });
    DeleteList(self->header);
    
    LIST_EACH(self->body, PAsmOp *, op, {
        MemFree(op);
    });
    DeleteList(self->body);
    
    LIST_EACH(self->footer, PAsmOp *, op, {
        MemFree(op);
    });
    DeleteList(self->footer);
    
    if (self->coloring != 0) {
        DeleteColoring(self->coloring);
    }
    
    if (self->locations) {
        DeleteMap(self->locations);
    }
    
    MemFree(self);
}

void PAsmFunctionSetBody(PAsmFunction *func, List *body) {
    func->body = body;
}

TYPE_DEF(PAsmModule, {
    Backend *backend;
    List *funcs;
}, {
    self->backend = 0;
    self->funcs = NewList();
}, {
    LIST_EACH(self->funcs, PAsmFunction *, func, {
        DeletePAsmFunction(func);
    });
    DeleteList(self->funcs);
})

void PAsmModuleAddFunc(PAsmModule *mod, PAsmFunction *func) {
    ListAdd(mod->funcs, func);
}

static void PAsmPrintOp(PAsmOp *op, Coloring *coloring, FILE *output) {
    
    UNUSED(output);
    
    #define printf(...)     fprintf(output, __VA_ARGS__)
    
    const char *fmt = op->fmt;
    const char *ptr = fmt;
    int opr_counter = 0;
    
    #define OPR     (&op->oprs[opr_counter])
    
    #define RULE(spec, ...) {                               \
        if (strncmp(spec, ptr, strlen(spec)) == 0) {        \
            __VA_ARGS__;                                    \
            opr_counter++;                                  \
            ptr += strlen(spec);                            \
            continue;                                       \
        }                                                   \
    }
    
    while (1) {
        
        size_t spn = strcspn(ptr, "$");
        printf("%.*s", (int) spn, ptr);
        ptr += spn;
        
        RULE("$vr", {
            if (coloring != 0 && ColoringIsColored(coloring, OPR->vreg)) {
                const char *color = ColoringGetColor(coloring, OPR->vreg);
                printf("%s", color);
            } else {
                printf("$vr%d", OPR->vreg->id);
            }
        });
        
        RULE("$id", {
            printf("$vr%d", OPR->vreg->id);
        });
        
        RULE("$i32", {
            printf("%d", OPR->i32);
        });
        
        RULE("$f", {
            printf("%f", OPR->f);
        });
        
        RULE("$str", {
            printf("%s", OPR->str);
        });
        
        RULE("$loc", {
            printf("loc($vr%d)", OPR->vreg->id);
        });
        
        printf("%s", ptr);
        break;
    }
    
    #undef RULE
    #undef OPR
    
    #undef printf
}

HEAP_DEF(PAsmVReg)

static int _VRegIDCounter = 0;

static int NewVRegID() {
    return ++_VRegIDCounter;
}

PAsmVReg *NewPAsmVReg() {
    PAsmVReg *vreg = HEAP(PAsmVReg, {
        .id = NewVRegID()
    });
    return vreg;
}

PAsmVReg *NewSpecialPAsmVReg() {
    PAsmVReg *vreg = NewPAsmVReg();
    vreg->special = 1;
    return vreg;
}

static const char *SPILL = "[spill]";

static List *Blockify(List *ops) {
    
    UNUSED(ops);

    List *blocks = NewList();
    List *current = 0;
    
    LIST_EACH(ops, PAsmOp *, op, {
        if (current == 0) {
            current = NewList();
            ListAdd(current, op);
        } else {
            if (op->is_label) {
                ListAdd(blocks, current);
                current = NewList();
                ListAdd(current, op);
            } else if (op->is_jump || op->is_ret) {
                ListAdd(current, op);
                ListAdd(blocks, current);
                current = 0;
            } else {
                ListAdd(current, op);
            }
        }
    });

    assert(current == 0);
    
    return blocks;
}

TYPE_DECL(CFG)
TYPE_DEF(CFG, {
    // fields
    int dummy;
}, {
    // ctor
}, {
    // dtor
})

static CFG *MakeCFG(List *blocks) {
    
    CFG *cfg = NewCFG();
    Map *map = NewMap();
    
    // fill lookup map
    LIST_EACH(blocks, List *, block, {
        LIST_FIRST(block, PAsmOp *, leader, {
            if (leader->is_label) {
                void *key = (void *)(intptr_t)leader->label_id;
                MapPut(map, key, block);
            }
        });
    });
    
    DeleteMap(map);
    return cfg;
}

void PAsmAllocateFunction2(PAsmModule *mod, PAsmFunction *func) {
    
    UNUSED(mod);
    UNUSED(func);
    
    printf("-------- blockify\n");
    printf("\n");
    
    int counter = 0;
    
    List *blocks = Blockify(func->body);
    LIST_EACH(blocks, List *, block, {
        printf("---- block %d\n", counter);
        
        LIST_EACH(block, PAsmOp *, op, {
            PAsmPrintOp(op, func->coloring, stdout);
        });
        
        printf("---- /block %d\n", counter++);
        printf("\n");
        DeleteList(block);
    });
    
    printf("-------- /blockify\n");
    
    CFG *cfg = MakeCFG(blocks);
    DeleteCFG(cfg);
    
    DeleteList(blocks);
}

static void PAsmAllocateFunction(PAsmModule *mod, PAsmFunction *func) {
    
    PAsmAllocateFunction2(mod, func);
    
    List *ops = func->body;
    Set *live = NewSet();
    RIG *rig = NewRIG();
    
    LIST_REV(ops, PAsmOp *, op, {
    
        for (int i = 0; i < PASM_OP_MAX_OPRS; i++) {
            PAsmVReg *vreg = op->def[i];
            if (vreg != 0) {
                // def'd
                SetRemove(live, vreg);
                RIGAdd(rig, vreg);
            }
        }
    
        for (int i = 0; i < PASM_OP_MAX_OPRS; i++) {
            PAsmVReg *vreg = op->use[i];
            if (vreg != 0) {
                // used
                SetAdd(live, vreg);
                RIGAdd(rig, vreg);
            }
        }
    
        SET_EACH(live, PAsmVReg *, a, {
            SET_EACH(live, PAsmVReg *, b, {
                if (a != b) {
                    RIGConnect(rig, a, b);
                }
            });
        });
    });
    
    DeleteSet(live);
    
    Backend *backend = mod->backend;
    
    Coloring *coloring = ColorRIG(
        rig,
        backend->Colors(),
        backend->Precoloring(),
        (void *)SPILL);
    
    DeleteRIG(rig);
    
    func->coloring = coloring;
}

void PAsmAllocate(PAsmModule *mod) {
    LIST_EACH(mod->funcs, PAsmFunction *, func, {
        PAsmAllocateFunction(mod, func);
    });
}

PAsmModule *PAsmModuleFromBackend(Backend *backend) {
    PAsmModule *mod = NewPAsmModule();
    mod->backend = backend;
    return mod;
}

static void PrintPAsmFunction(PAsmModule *mod, PAsmFunction *func, FILE *output) {
    
    Backend *backend = mod->backend;
    
    #define PAsmPrintOp(op)     PAsmPrintOp(op, func->coloring, output)
    
    LIST_EACH(func->header, PAsmOp *, op, {
        PAsmPrintOp(op);
    });
    
    List *prologue = backend->GenPrologue(func);
    LIST_EACH(prologue, PAsmOp *, op, {
        PAsmPrintOp(op);
        MemFree(op);
    });
    DeleteList(prologue);
    
    LIST_EACH(func->body, PAsmOp *, op, {
        PAsmPrintOp(op);
    });
    
    List *epilogue = backend->GenEpilogue(func);
    LIST_EACH(epilogue, PAsmOp *, op, {
        PAsmPrintOp(op);
        MemFree(op);
    });
    DeleteList(epilogue);
    
    LIST_EACH(func->footer, PAsmOp *, op, {
        PAsmPrintOp(op);
    });
    
    #undef PAsmPrintOp
}

void PrintPAsmModule(PAsmModule *mod, FILE *output) {
    // LIST_EACH(mod->ops, PAsmOp *, op, {
    //     PAsmPrintOp(mod, op, output);
    // });
    LIST_EACH(mod->funcs, PAsmFunction *, func, {
        PrintPAsmFunction(mod, func, output);
    });
}

static void SubVReg(PAsmOp *op, PAsmVReg *old, PAsmVReg *tmp) {
    for (int i = 0; i < PASM_OP_MAX_OPRS; i++) {
        PAsmOpr *opr = &op->oprs[i];
        if (opr->vreg != 0 && opr->vreg == old) {
            opr->vreg = tmp;
        }
    }
}

static int PAsmSpillFunction(PAsmModule *mod, PAsmFunction *func) {
    
    int has_spilled = 0;
    
    List *old_ops = func->body;
    func->body = NewList();
    
    Coloring *coloring = func->coloring;
    Backend *backend = mod->backend;
    
    LIST_EACH(old_ops, PAsmOp *, op, {
    
        Map *map = NewMap();
    
        for (int i = 0; i < PASM_OP_MAX_OPRS; i++) {
            
            PAsmVReg *use = op->use[i];
            if (use != 0) {
                void *color = ColoringGetColor(coloring, use);
                if (color == SPILL && !MapContains(map, use)) {
                    MapPut(map, use, NewPAsmVReg());
                    has_spilled = 1;
                }
            }
            
            PAsmVReg *def = op->def[i];
            if (def != 0) {
                void *color = ColoringGetColor(coloring, def);
                if (color == SPILL && !MapContains(map, def)) {
                    MapPut(map, def, NewPAsmVReg());
                    has_spilled = 1;
                }
            }
        }
        
        for (int i = 0; i < PASM_OP_MAX_OPRS; i++) {
            PAsmVReg *use = op->use[i];
            if (use != 0) {
                void *color = ColoringGetColor(coloring, use);
                if (color == SPILL) {
                    PAsmVReg *tmp = MapGet(map, use);
                    backend->LoadVReg(func, use, tmp);
                    op->use[i] = tmp;
                    SubVReg(op, use, tmp);
                }
            }
        }
    
        ListAdd(func->body, op);
        
        for (int i = 0; i < PASM_OP_MAX_OPRS; i++) {
            PAsmVReg *def = op->def[i];
            if (def != 0) {
                void *color = ColoringGetColor(coloring, def);
                if (color == SPILL) {
                    PAsmVReg *tmp = MapGet(map, def);
                    backend->StoreVReg(func, def, tmp);
                    op->def[i] = tmp;
                    SubVReg(op, def, tmp);
                }
            }
        }
        
        DeleteMap(map);
    });
    
    DeleteList(old_ops);
    
    return has_spilled;
}

int PAsmSpill(PAsmModule *mod) {
    
    int has_spilled = 0;
    
    LIST_EACH(mod->funcs, PAsmFunction *, func, {
        has_spilled |= PAsmSpillFunction(mod, func);
    });
    
    if (has_spilled) {
        LIST_EACH(mod->funcs, PAsmFunction *, func, {
            DeleteColoring(func->coloring);
            func->coloring = 0;
        });
    }
    
    return has_spilled;
}
