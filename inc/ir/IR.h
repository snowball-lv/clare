#pragma once

#include <helpers/Types.h>

#define MAX_KIDS    1

typedef struct Node Node;
struct Node {
    int type;
    Node *kids[MAX_KIDS];
};

HEAP_DECL(Node)

#define NT(name)    NT ## name

enum {
    #define NODE(name)      NT(name),
    
        NODE(Invalid)
        #include <ir/nodes.def>
        
    #undef NODE
};

int NodeCmp(Node *a, Node *b);
int NodeMatch(Node *root, Node *pattern);
