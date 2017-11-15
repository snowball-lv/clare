#include <ir/IR.h>

int dummy;

HEAP_DEF(Node)

int NodeCmp(Node *a, Node *b) {
    
    if (a == 0 || b == 0) {
        return 0;
    }
    
    if (a->type == NT(Any) || b->type == NT(Any)) {
        return 1;
    } else {
        return a->type == b->type;
    }
}

int NodeMatch(Node *root, Node *pattern) {
    
    if (pattern == 0) {
        return 1;
    }
    
    if (pattern != 0 && root == 0) {
        return 0;
    }
    
    if (NodeCmp(root, pattern)) {
        for (int i = 0; i < MAX_KIDS; i++) {
            Node *a = root->kids[i];
            Node *b = pattern->kids[i];
            if (!NodeMatch(a, b)) {
                return 0;
            }
        }
        return 1;
    }
    
    return 0;
}

#define NODE(name) static Node *_ ## name() {   \
    return HEAP(Node, { .type = NT(name) });    \
}
    #include <ir/nodes.def>
#undef NODE

_Nodes Nodes = {

    .dummy = 0,    
    
    #define NODE(name)      .name = _ ## name,
        #include <ir/nodes.def>
    #undef NODE
};
