#include <pasm/PAsm.h>

#include <helpers/Unused.h>
#include <collections/List.h>
#include <collections/Map.h>
#include <color/RIG.h>
#include <color/Coloring.h>

#include <string.h>
#include <stdio.h>

int dummy;

HEAP_DEF(PAsmOp)

static void CollectVRegs(List *ops, Set *vregs) {
    LIST_EACH(ops, PAsmOp *, op, {
        for (int i = 0; i < PASM_OP_MAX_OPRS; i++) {
            PAsmOpr *opr = &op->oprs[i];
            if (opr->vreg != 0) {
                SetAdd(vregs, opr->vreg);
            }
        }
    });
}

TYPE_DEF(PAsmModule, {
    int dummy;
    List *ops;
    Coloring *coloring;
}, {
    self->ops = NewList();
}, {
    
    Set *vregs = NewSet();
    CollectVRegs(self->ops, vregs);
    SET_EACH(vregs, PAsmVReg *, vreg, {
        MemFree(vreg);
    });
    DeleteSet(vregs);
    
    LIST_EACH(self->ops, PAsmOp *, op, {
        MemFree(op);
    });
    DeleteList(self->ops);
    
    if (self->coloring != 0) {
        DeleteColoring(self->coloring);
    }
})

void PAsmModuleAddOp(PAsmModule *mod, PAsmOp *op) {
    ListAdd(mod->ops, op);
}

static void PAsmPrintOp(PAsmModule *mod, PAsmOp *op) {
    
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
            if (mod->coloring != 0) {
                const char *color = ColoringGetColor(mod->coloring, OPR->vreg);
                printf("%s", color);
            } else {
                printf("$vr%d", OPR->vreg->id);
            }
        });
        
        RULE("$i32", {
            printf("%d", OPR->i32);
        });
        
        RULE("$str", {
            printf("%s", OPR->str);
        });
        
        printf("%s", ptr);
        break;
    }
    
    #undef RULE
    #undef OPR
}

void PAsmPrintModule(PAsmModule *mod) {
    UNUSED(mod);
    printf("--- pasm module ---\n");
    LIST_EACH(mod->ops, PAsmOp *, op, {
        PAsmPrintOp(mod, op);
    });
    printf("-------------------\n");
}

static Map *_TmpToVregMap = 0;

void PAsmInit() {
    _TmpToVregMap = NewMap();
}

void PAsmDeinit() {
    DeleteMap(_TmpToVregMap);
}

HEAP_DEF(PAsmVReg)

PAsmVReg *PAsmVRegFromTmp(Node *tmp) {
    if (!MapContains(_TmpToVregMap, tmp)) {
        PAsmVReg *vreg = HEAP(PAsmVReg, {
            .id = tmp->index
        });
        MapPut(_TmpToVregMap, tmp, vreg);
    }
    PAsmVReg *vreg = MapGet(_TmpToVregMap, tmp);
    return vreg;
}

PAsmVReg *NewPAsmVReg() {
    Node *tmp = IR.Tmp();
    PAsmVReg *vreg = PAsmVRegFromTmp(tmp);
    MapRemove(_TmpToVregMap, tmp);
    MemFree(tmp);
    return vreg;
}

static const char *SPILL = "[spill]";

void PAsmAllocate(PAsmModule *mod) {
    
    List *ops = mod->ops;
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
    
    Set *colors = NewSet();
    SetAdd(colors, "A");
    SetAdd(colors, "B");
    SetAdd(colors, "C");
    
    Map *precoloring = NewMap();
    
    Coloring *coloring = ColorRIG(
        rig,
        colors,
        precoloring,
        (void *)SPILL);
        
    mod->coloring = coloring;
    
    DeleteMap(precoloring);
    DeleteSet(colors);
    DeleteRIG(rig);
    DeleteSet(live);
}
