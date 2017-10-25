#include <i386/i386.h>

#include <helpers/Unused.h>
#include <mem/Mem.h>
#include <helpers/Strings.h>

#include <stdio.h>
#include <stdint.h>
#include <string.h>

HEAP_DEF(Op)
HEAP_DEF(Opr)

static int _Label_Counter = 1;

Label *NewLabel() {
    return ToString("L%d", _Label_Counter++);
}

Label *NewLabelNamed(const char *name) {
    return ToString("L%d_%s", _Label_Counter++, name);
}

static int _VReg_Counter = 1;

VReg *NewVReg() {
    VReg *ptr = ALLOC(VReg);
    ptr->index = _VReg_Counter++;
    return ptr;
}

void OpPrintf(Op *op) {
    
    const char *fmt = op->fmt;
    int opt_counter = 0;
    
    do {
        
        const char *ptr = strchr(fmt, '$');
        if (ptr != 0) {
            
            size_t spn = strcspn(fmt, "$");
            printf("%.*s", (int)spn, fmt);
            
            #define ARG op->oprs[opt_counter]
            #define FMT(specifier, block) {                     \
                size_t slen = strlen(specifier);                \
                if (strncmp(ptr + 1, specifier, slen) == 0) {   \
                    block;                                      \
                    fmt = ptr + 1 + slen;                       \
                    opt_counter++;                              \
                    continue;                                   \
                }                                               \
            }
            
                #include <i386/op.fmt>
            
            #undef FMT
            #undef ARG
            
            printf("$");
            fmt = ptr + 1;
            
        } else {
            printf("%s", fmt);
            return;
        }
        
    } while (1);
}
