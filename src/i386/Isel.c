#include <i386/Isel.h>

#include <mem/Mem.h>
#include <helpers/Unused.h>
#include <collections/List.h>
#include <i386/i386.h>
#include <helpers/Unused.h>

#include <stdio.h>

struct Isel {
    int dummy;
    List *list;
};

Isel *NewIsel() {
    
    Isel *isel = ALLOC(Isel);
    isel->dummy = 0;
    isel->list = NewList();
    
    #define RULE(pattern, action) ListAdd(isel->list, pattern);
        #include <i386/munch.rules>
    #undef RULE
    
    return isel;
}

void DeleteIsel(Isel *isel) {
    
    LIST_EACH(isel->list, Node *, pattern, {
        DeleteNodeTree(pattern);
    });
    DeleteList(isel->list);
    
    MemFree(isel);
}

static VReg *IselMunch(Isel *isel, List *ops, Node *root) {
    
    int index = 0;
    
    UNUSED(index);
    UNUSED(isel);
    UNUSED(ops);
    UNUSED(root);
    
    #define Emit(op)    ListAdd(ops, op);
    #define Munch(root) IselMunch(isel, ops, root)
    
    #define RULE(pattern, action) {                 \
            Node *p = ListGet(isel->list, index);   \
            if (NodeMatches(root, p)) {             \
                action;                             \
                return 0;                           \
            }                                       \
            index++;                                \
        }
    
        #include <i386/munch.rules>
        
    #undef RULE
    #undef Munch
    #undef Emit
    
    printf("no match for node: ");
    NodePrint(root);
    printf("\n");
    
    return 0;
} 

List *IselSelect(Isel *isel, Node *root) {
    List *ops = NewList();
    IselMunch(isel, ops, root);
    return ops;
}
