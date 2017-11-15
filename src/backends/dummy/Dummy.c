#include <backends/dummy/Dummy.h>

#include <helpers/Unused.h>
#include <collections/List.h>

#include <stdio.h>

int dummy;

void _DummyMunch(Node *root, List *patterns) {
    
    int _index = 0;
    
    #define RULE(pattern, action) {             \
        Node *p = ListGet(patterns, _index);    \
        if (NodeMatch(root, p)) {               \
            action;                             \
            return;                             \
        }                                       \
        _index++;                               \
    }
        #include <backends/dummy/dummy.rules>
    #undef RULE
}

void DummyMunch(Node *root) {
    
    List *patterns = NewList();
    
    #define RULE(pattern, action)   ListAdd(patterns, pattern);
        #include <backends/dummy/dummy.rules>
    #undef RULE
    
    _DummyMunch(root, patterns);
    
    LIST_EACH(patterns, Node *, pattern, {
        MemFree(pattern);
    });
    DeleteList(patterns);
}
