#pragma once

#include <helpers/Types.h>

#include <stdint.h>

#include <collections/List.h>

typedef struct Node Node;

void DeleteNodeTree(Node *root);
int NodeMatches(Node *root, Node *pattern);
void NodePrint(Node *node);
Node *NodeChild(Node *node, int index);

typedef struct {
    
    #define FUNC(type, name, params, body)  \
        type (*name) params;
        
        #include <ir/IR.mod>
        
    #undef FUNC
    
} _IR;

extern _IR IR;
