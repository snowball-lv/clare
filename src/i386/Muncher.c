#include <i386/Muncher.h>

#include <helpers/Unused.h>
#include <stdio.h>

static void Munch(Node *root) {

    #define RULE(pattern, action)           \
        if (NodeMatches(root, pattern)) {   \
            action;                         \
            return;                         \
        }

        #include <i386/munch.rules>

    #undef RULE
}

void I386_Munch(Node *root) {
    Munch(root);
}
