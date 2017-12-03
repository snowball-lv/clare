#include <pasm/PAsm.h>

#include <helpers/Unused.h>
#include <collections/List.h>

#include <string.h>
#include <stdio.h>

int dummy;

HEAP_DEF(PAsmOp)

TYPE_DEF(PAsmModule, {
    int dummy;
    List *ops;
}, {
    self->ops = NewList();
}, {
    LIST_EACH(self->ops, PAsmOp *, op, {
        MemFree(op);
    });
    DeleteList(self->ops);
})

void PAsmModuleAddOp(PAsmModule *mod, PAsmOp *op) {
    ListAdd(mod->ops, op);
}

void PAsmPrintOp(PAsmOp *op) {
    
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
            printf("$vr%d", OPR->vreg->id);
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
        PAsmPrintOp(op);
    });
    printf("-------------------\n");
}
