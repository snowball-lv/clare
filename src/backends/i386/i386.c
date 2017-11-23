#include <backends/i386/i386.h>

#include <helpers/Unused.h>
#include <collections/List.h>
#include <collections/Set.h>
#include <collections/Map.h>
#include <helpers/Types.h>

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

typedef struct {
    int id;
} VReg;

HEAP_DEF(VReg)
#define VREG(...)    HEAP(VReg, __VA_ARGS__)

static int _VRegCounter = -1;
static VReg *NewVReg() {
    return VREG({
        .id = _VRegCounter--
    });
}

typedef struct {
    int32_t i32;
    VReg    *vreg;
} Opr;

#define MAX_OPRS    2

typedef struct {
    const char *fmt;
    Opr oprs[MAX_OPRS];
    VReg *use[MAX_OPRS];
    VReg *def[MAX_OPRS];
} Op;

HEAP_DEF(Op)
#define OP(...)    HEAP(Op, __VA_ARGS__)

typedef struct {
    Map *vregs;
} MunchState;

static VReg *VRegForTmp(MunchState *state, Node *tmp) {
    Map *map = state->vregs;
    if (!MapContains(map, tmp)) {
        VReg *vreg = VREG({
            .id = tmp->index
        });
        MapPut(map, tmp, vreg);
    }
    return MapGet(map, tmp);
}

#define MANGLE(name)    i386 ## name
#define RULE_FILE       <backends/i386/i386.rules>
#define RET_TYPE        VReg *
#define RET_DEFAULT     0

#define STATE
#define STATE_T         MunchState
#define STATE_INIT      { state->vregs = NewMap(); }
#define STATE_DEINIT    { DeleteMap(state->vregs); }

    #include <ir/muncher.def>
    
#undef STATE_DEINIT
#undef STATE_INIT
#undef STATE_T
#undef STATE

#undef RET_DEFAULT
#undef RET_TYPE
#undef RULE_FILE
#undef MANGLE

static void CollectVRegs(List *ops, Set *vregs) {
    LIST_EACH(ops, Op *, op, {
        for (int i = 0; i < MAX_OPRS; i++) {
            VReg *vreg = op->oprs[i].vreg;
            if (vreg != 0)  {
                SetAdd(vregs, vreg);
            }
        }
    });
}

void DeleteOps(List *ops) {
    
    Set *vregs = NewSet();
    CollectVRegs(ops, vregs);
    
    SET_EACH(vregs, VReg *, vreg, {
        MemFree(vreg);
    });
    DeleteSet(vregs);
    
    LIST_EACH(ops, Op *, op, {
        MemFree(op);
    });
}

static void PrintOp(Op *op, Coloring *coloring) {

    const char *fmt = op->fmt;
    
    int opr_counter = 0;
    
    #define RULE(spec, action) {                    \
        const char *ptr = strstr(fmt, spec);        \
        if (ptr != 0) {                             \
            ptrdiff_t diff = ptr - fmt;             \
            if (diff > 0) {                         \
                printf("%.*s", (int) diff, fmt);    \
            }                                       \
            Opr *opr = &op->oprs[opr_counter];      \
            action;                                 \
            opr_counter++;                          \
            fmt = ptr + strlen(spec);               \
            continue;                               \
        }                                           \
    }
    
    while (1) {
        
        RULE("$vr", {
            if (coloring != 0) {
                char *color = ColoringGetColor(coloring, opr->vreg);
                printf("%s", color);
            } else {
                printf("$vr%d", opr->vreg->id);
            }
        });
            
        RULE("$i32", {
            printf("%d", opr->i32);
        });
        
        printf("%s", fmt);
        break;
    }
    
    #undef RULE
}

void PrintOps(List *ops) {
    LIST_EACH(ops, Op *, op, {
        PrintOp(op, 0);
    });
}

Set *OpUse(void *op) {
    Set *use = NewSet();
    for (int i = 0; i < MAX_OPRS; i++) {
        VReg *vreg = ((Op *)op)->use[i];
        if (vreg != 0) {
            SetAdd(use, vreg);
        }
    }
    return use;
}

Set *OpDef(void *op) {
    Set *def = NewSet();
    for (int i = 0; i < MAX_OPRS; i++) {
        VReg *vreg = ((Op *)op)->def[i];
        if (vreg != 0) {
            SetAdd(def, vreg);
        }
    }
    return def;
}

int VRegIndex(void *vreg) {
    return ((VReg *)vreg)->id;
}

void PrintColoredOps(List *ops, Coloring *coloring) {
    LIST_EACH(ops, Op *, op, {
        PrintOp(op, coloring);
    });
}
