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
    return ToString("_label_%d", _Label_Counter++);
}

static int _VReg_Counter = 1;

VReg *NewVReg() {
    VReg *ptr = ALLOC(VReg);
    ptr->index = _VReg_Counter++;
    return ptr;
}

/*
    $l - label
*/
void OpPrintf(Op *op) {
    
    const char *fmt = op->fmt;
    
    do {
        
        const char *ptr = strchr(fmt, '$');
        if (ptr != 0) {
            
            size_t spn = strcspn(fmt, "$");
            printf("%.*s", (int)spn, fmt);
            
            #define FMT(specifier, block) {                     \
                size_t slen = strlen(specifier);                \
                if (strncmp(ptr + 1, specifier, slen) == 0) {   \
                    block;                                      \
                    fmt = ptr + 1 + slen;                       \
                    continue;                                   \
                }                                               \
            }
            
                #include <i386/op.fmt>
            
            #undef FMT
            
            printf("$");
            fmt = ptr + 1;
            
        } else {
            printf("%s", fmt);
            return;
        }
        
    } while (1);
}
