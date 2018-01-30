#include <clare/ir/IR.h>

#include <clare/helpers/Unused.h>
#include <clare/helpers/Error.h>
#include <clare/collections/Set.h>
#include <clare/collections/Map.h>

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

static int _TmpIDCounter = 0;

static int NewTmpID() {
    return ++_TmpIDCounter;
}

#define NODE(name, params, init)                    \
static Node *_ ## name params {                     \
    Node *self = HEAP(Node, { .type = NT(name) });  \
    init;                                           \
    return self;                                    \
}
#include <clare/ir/nodes.def>
#undef NODE

_Nodes IR = {

    .dummy = 0,    
    
    #define NODE(name, params, init)    .name = _ ## name,
#include <clare/ir/nodes.def>
    #undef NODE
};

const char *NodeName(Node *node) {
    switch (node->type) {
        #define NODE(name, params, init) case NT(name): return #name;
#include <clare/ir/nodes.def>
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
    
    if (root->type == NT(Call) && root->args != 0) {
        LIST_EACH(root->args, Node *, arg, {
            _NodeDeleteTree(arg, nodes);
        });
    }
    
    SetAdd(nodes, root);
}

void NodeDeleteTree(Node *root) {
    Set *nodes = NewSet();
    _NodeDeleteTree(root, nodes);
    SET_EACH(nodes, Node *, node, {
        if (node->type == NT(Call) && node->args != 0) {
            DeleteList(node->args);
        }
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
    const char *name;
}, {
    self->body = 0;
}, {
    if (self->body != 0) {
        NodeDeleteTree(self->body);
    }
})

const char *IRFunctionName(IRFunction *func) {
    return func->name;
}

IRFunction *IRModuleNewFunction(IRModule *mod, const char *name) {

    if (MapContains(mod->functions, (void *)name)) {
        printf("module already contains function: %s\n", name);
        exit(1);
        return 0;
    }
    
    IRFunction *irFunc = NewIRFunction();
    irFunc->name = name;
    MapPut(mod->functions, (void *)name, irFunc);
    return irFunc;
}

void IRFunctionSetBody(IRFunction *func, Node *body) {
    func->body = body;
}

#define MANGLE(name)    IRPrinter ## name
#define RULE_FILE       <clare/ir/printer.rules>
#include <clare/ir/muncher.def>
#undef RULE_FILE
#undef MANGLE

void IRPrintTree(Node *root) {
    IRPrinterMunch(root);
}

List *IRModuleFunctions(IRModule *mod) {
    List *funcs = NewList();
    MAP_EACH(mod->functions, char *, name, IRFunction *, func, {
        ListAdd(funcs, func);
    });
    return funcs;
}

Node *IRFunctionBody(IRFunction *func) {
    return func->body;
}

const char *IRDataTypeName(int data_type) {
    switch (data_type) {
        #define TYPE(name) case IR_DT(name): return #name;
        TYPE(Invalid)
        TYPE(None)
        TYPE(Void)
        TYPE(I32)
        TYPE(Double)
        TYPE(Str)
        TYPE(Float)
        #undef TYPE
    }
    ERROR("Unknown data type: %d\n", data_type);
}

int IRIsScalar(int data_type) {
    switch (data_type) {
        #define TYPE(name) case IR_DT(name):
        TYPE(Invalid)
        TYPE(None)
        TYPE(Void)
        TYPE(Str)
        TYPE(Double)
            return 0;
        TYPE(I32)
        TYPE(Float)
            return 1;
        #undef TYPE
    }
    ERROR("Unknown data type: %d\n", data_type);
}
