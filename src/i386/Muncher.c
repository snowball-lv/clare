#include <i386/Muncher.h>

#include <helpers/Unused.h>
#include <stdio.h>
#include <i386/i386.h>
#include <mem/Mem.h>

static I386_Node *Munch(Node *root) {

    #define RULE(pattern, action)           \
        if (NodeMatches(root, pattern)) {   \
            action;                         \
            return 0;                       \
        }

        #include <i386/munch.rules>

    #undef RULE
    
    return 0;
}

void I386_Munch(Node *root) {
    Munch(root);
}

struct I386_Muncher {
    int dummy;
};

I386_Muncher *New_I386_Muncher() {
    I386_Muncher *muncher = ALLOC(I386_Muncher);
    muncher->dummy = 0;
    return muncher;
}

void Delete_I386_Muncher(I386_Muncher *muncher) {
    MemFree(muncher);
}
