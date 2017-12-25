#pragma once

#include <helpers/Types.h>
#include <collections/List.h>

#include <stdint.h>

#define IR_MAX_KIDS    3

typedef struct Node Node;
struct Node {
    
    int type;
    int data_type;
    Node *kids[IR_MAX_KIDS];
    
    union {
        int32_t i32;
        float f;
        int id;
        int arg_index;
        struct {
            const char *fname;
            List *args;
        };
        const char *str;
    };
};

#define IR_DT(name)     IR_DT_ ## name

enum {
    #define TYPE(name)      IR_DT(name),
    TYPE(Invalid)
    TYPE(None)
    TYPE(Void)
    TYPE(I32)
    TYPE(Str)
    TYPE(Float)
    TYPE(Double)
    #undef TYPE
};

const char *IRDataTypeName(int data_type);

HEAP_DECL(Node)

#define NT(name)    IR_NT_ ## name

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

extern _Nodes IR;

TYPE_DECL(IRModule)
TYPE_DECL(IRFunction)

IRFunction *IRModuleNewFunction(IRModule *mod, const char *name);
void IRFunctionSetBody(IRFunction *func, Node *body);

void IRPrintTree(Node *root);
List *IRModuleFunctions(IRModule *mod);
const char *IRFunctionName(IRFunction *func);
Node *IRFunctionBody(IRFunction *func);

int IRIsScalar(int data_type);
