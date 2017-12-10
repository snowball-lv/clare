#include <backends/Backends.h>
#include <helpers/Unused.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <backends/i386/i386.h>

Backend *GetBackend(const char *name) {
    
    #define DEF(b_name, b_addr) {                           \
        if (strcmp(name, b_name) == 0) { return b_addr; }   \
    }
    
    DEF("i386", &i386_Backend);
    
    return 0;
}
