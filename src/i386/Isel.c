#include <i386/Isel.h>

#include <mem/Mem.h>
#include <helpers/Unused.h>
#include <collections/List.h>
#include <i386/i386.h>

#include <stdio.h>

struct Isel {
    int dummy;
    List *list;
    List *ops;
};

Isel *NewIsel() {
    
    Isel *isel = ALLOC(Isel);
    isel->dummy = 0;
    isel->list = NewList();
    isel->ops = NewList();
    
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
        
    LIST_EACH(isel->ops, Op *, op, {
        DeleteOp(op);
    });
    DeleteList(isel->ops);
    
    MemFree(isel);
}

static void IselEmit(Isel *isel, Op *op) {
    ListAdd(isel->ops, op);
}

static VReg *IselMunch(Isel *isel, Node *root) {
    
    int index = 0;
    
    #define Emit(op)    IselEmit(isel, op)
    #define Munch(root) IselMunch(isel, root)
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
    
    return 0;
} 

void IselSelect(Isel *isel, Node *root) {
    IselMunch(isel, root);
}
