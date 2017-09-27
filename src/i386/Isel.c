#include <i386/Isel.h>

#include <mem/Mem.h>
#include <helpers/Unused.h>
#include <collections/List.h>

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

static void IselMunch(Isel *isel, Node *root) {
    
    int index = 0;
    
    #define Munch(root) IselMunch(isel, root)
    #define RULE(pattern, action) {                 \
            Node *p = ListGet(isel->list, index);   \
            if (NodeMatches(root, p)) {             \
                action;                             \
                return;                             \
            }                                       \
            index++;                                \
        }
    
        #include <i386/munch.rules>
        
    #undef RULE
    #undef Munch
} 

void IselSelect(Isel *isel, Node *root) {
    IselMunch(isel, root);
}
