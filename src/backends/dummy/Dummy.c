#include <backends/dummy/Dummy.h>

#include <helpers/Unused.h>
#include <collections/List.h>

#include <stdio.h>
#include <assert.h>

int dummy;

void _DummyMunch(Node *root, List *patterns) {
    
    int _index = 0;
    
    #define MUNCH(node)     _DummyMunch(node, patterns);
    
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
    
    #undef MUNCH
        
    printf("Trying to match node: %s\n", NodeName(root));
    assert(0 && "No rule matches node");
}

void DummyMunch(Node *root) {
    
    List *patterns = NewList();
    
    #define RULE(pattern, action)   ListAdd(patterns, pattern);
        #include <backends/dummy/dummy.rules>
    #undef RULE
    
    _DummyMunch(root, patterns);
    
    LIST_EACH(patterns, Node *, pattern, {
        NodeDeleteTree(pattern);
    });
    DeleteList(patterns);
}
