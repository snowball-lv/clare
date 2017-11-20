#include <backends/i386/i386.h>

#include <helpers/Unused.h>
#include <collections/List.h>
#include <collections/Set.h>
#include <helpers/Types.h>

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

int dummy;

typedef struct {
    union {
        int dummy;
        int index;
        int32_t i32;
    };
} Opr;

HEAP_DEF(Opr)
#define OPR(...)    HEAP(Opr, __VA_ARGS__)

#define MAX_OPRS    2

typedef struct {
    const char *fmt;
    Opr *oprs[MAX_OPRS];
} Op;

HEAP_DEF(Op)
#define OP(...)    HEAP(Op, __VA_ARGS__)

static Op *Mov(Opr *dst, Opr *src) {
    return OP({
        .fmt = "mov $vr, $vr\n",
        .oprs = { dst, src }
    });
}

static Op *Add(Opr *left, Opr *right) {
    return OP({
        .fmt = "add $vr, $vr\n",
        .oprs = { left, right }
    });
}

#define MANGLE(name)    i386 ## name
#define RULE_FILE       <backends/i386/i386.rules>

    #include <ir/muncher.def>
    
#undef RULE_FILE
#undef MANGLE

static void CollectOprs(List *ops, Set *oprs) {
    LIST_EACH(ops, Op *, op, {
        for (int i = 0; i < MAX_OPRS; i++) {
            Opr *opr = op->oprs[i];
            if (opr != 0)  {
                SetAdd(oprs, opr);
            }
        }
    });
}

void DeleteOps(List *ops) {
    
    Set *oprs = NewSet();
    CollectOprs(ops, oprs);
    
    SET_EACH(oprs, Opr *, opr, {
        MemFree(opr);
    });
    DeleteSet(oprs);
    
    LIST_EACH(ops, Op *, op, {
        MemFree(op);
    });
}

static void PrintOpr(Opr *opr) {
    printf("%s", "[opr]");
    UNUSED(opr);
}

static void PrintOp(Op *op) {

    const char *fmt = op->fmt;
    
    int opr_counter = 0;
    
    while (1) {
        
        const char *spec = "$vr";
        const char *ptr = strstr(fmt, spec);
        
        if (ptr != 0) {
            
            ptrdiff_t diff = ptr - fmt;
            if (diff > 0) {
                printf("%.*s", (int) diff, fmt);
            }
            
            Opr *opr = op->oprs[opr_counter];
            PrintOpr(opr);
            opr_counter++;
            
            fmt = ptr + strlen(spec);
            
        } else {
            printf("%s", fmt);
            break;
        }
    }
}

void PrintOps(List *ops) {
    LIST_EACH(ops, Op *, op, {
        PrintOp(op);
    });
}
