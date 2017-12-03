#include <pasm/PAsm.h>

#include <helpers/Unused.h>
#include <collections/List.h>

#include <string.h>
#include <stdio.h>

int dummy;

HEAP_DEF(PAsmOp)

TYPE_DEF(PAsmModule, {
    int dummy;
    List *header;
}, {
    self->header = NewList();
}, {
    DeleteList(self->header);
})

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
        
        printf("%s", ptr);
        break;
    }
    
    #undef RULE
    #undef OPR
}
