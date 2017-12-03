#pragma once

#include <helpers/Types.h>

#include <stdint.h>

#define IR_MAX_KIDS    2

typedef struct Node Node;
struct Node {
    int type;
    Node *kids[IR_MAX_KIDS];
    union {
        int32_t i32;
        int index;
        int arg_index;
    };
};

HEAP_DECL(Node)

#define NT(name)    NT ## name

enum {
    
    #define NODE(name, params, init)    NT(name),
    
        NODE(Invalid, (), {})
        #include <ir/nodes.def>
        
    #undef NODE
};

int NodeCmp(Node *a, Node *b);
int NodeMatch(Node *root, Node *pattern);
const char *NodeName(Node *node);
void NodeDeleteTree(Node *root);

typedef struct {
    
    int dummy;
    
    #define NODE(name, params, init) Node *(*name) params;
        #include <ir/nodes.def>
    #undef NODE
} _Nodes;

extern _Nodes Nodes;
extern _Nodes IR;

TYPE_DECL(IRModule)
TYPE_DECL(IRFunction)

IRFunction *IRModuleNewFunction(IRModule *mod, const char *name);
void IRFunctionSetBody(IRFunction *func, Node *body);

void IRPrintTree(Node *root);
