#include <clare/frontends/Frontends.h>
#include <clare/helpers/Unused.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <clare/frontends/clare-ir/clare-ir.h>

Frontend *GetFrontend(const char *name) {
    
    UNUSED(name);
    
    #define DEF(b_name, b_addr) {                           \
        if (strcmp(name, b_name) == 0) { return b_addr; }   \
    }
    
    // DEF("i386", &i386_Backend);
    
    DEF("clare-ir", &clare_IR_Frontend);
    
    return 0;
}
