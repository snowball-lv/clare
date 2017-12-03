#include <ir/IR.h>

#include <helpers/Unused.h>
#include <collections/Set.h>
#include <collections/Map.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

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
        for (int i = 0; i < IR_MAX_KIDS; i++) {
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

static int TmpIndexCounter = 0;

static int NextTmpIndex() {
    return ++TmpIndexCounter;
}

#define NODE(name, params, init)                    \
static Node *_ ## name params {                     \
    Node *self = HEAP(Node, { .type = NT(name) });  \
    init;                                           \
    return self;                                    \
}
    #include <ir/nodes.def>
#undef NODE

_Nodes Nodes = {

    .dummy = 0,    
    
    #define NODE(name, params, init)    .name = _ ## name,
        #include <ir/nodes.def>
    #undef NODE
};

_Nodes IR = {

    .dummy = 0,    
    
    #define NODE(name, params, init)    .name = _ ## name,
        #include <ir/nodes.def>
    #undef NODE
};

const char *NodeName(Node *node) {
    switch (node->type) {
        #define NODE(name, params, init) case NT(name): return #name;
            #include <ir/nodes.def>
        #undef NODE
    }
    assert(0 && "Unknown node type");
}

static void _NodeDeleteTree(Node *root, Set *nodes) {
    
    if (root == 0) {
        return;
    }
    
    for (int i = 0; i < IR_MAX_KIDS; i++) {
        Node *child = root->kids[i];
        _NodeDeleteTree(child, nodes);
    }
    
    SetAdd(nodes, root);
}

void NodeDeleteTree(Node *root) {
    Set *nodes = NewSet();
    _NodeDeleteTree(root, nodes);
    SET_EACH(nodes, Node *, node, {
        MemFree(node);
    });
    DeleteSet(nodes);
}

TYPE_DEF(IRModule, {
    int dummy;
    Map *functions;
}, {
    self->functions = NewMap();
}, {
    MAP_EACH(self->functions, char *, name, IRFunction *, func, {
        DeleteIRFunction(func);
    });
    DeleteMap(self->functions);
})

TYPE_DEF(IRFunction, {
    int dummy;
    Node *body;
}, {
    self->body = 0;
}, {
    if (self->body != 0) {
        NodeDeleteTree(self->body);
    }
})

IRFunction *IRModuleNewFunction(IRModule *mod, const char *name) {

    if (MapContains(mod->functions, (void *)name)) {
        printf("module already contains function: %s\n", name);
        exit(1);
        return 0;
    }
    
    IRFunction *irFunc = NewIRFunction();
    MapPut(mod->functions, (void *)name, irFunc);
    return irFunc;
}

void IRFunctionSetBody(IRFunction *func, Node *body) {
    func->body = body;
}
