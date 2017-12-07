#include <frontends/Frontends.h>
#include <helpers/Unused.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

Frontend *GetFrontend(const char *name) {
    
    UNUSED(name);
    
    #define DEF(b_name, b_addr) {                           \
        if (strcmp(name, b_name) == 0) { return b_addr; }   \
    }
    
    // DEF("i386", &i386_Backend);
    
    return 0;
}
