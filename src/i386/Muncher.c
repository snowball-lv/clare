#include <i386/Muncher.h>

#include <helpers/Unused.h>
#include <stdio.h>

#define RULES_FILE <i386/munch.rules>
    #include <ir/Muncher.tmpl>
#undef RULES_FILE

void I386_Munch(Node *root) {
    Munch(root);
}
