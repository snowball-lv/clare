#pragma once

typedef struct Node Node;

void DeleteNodeTree(Node *root);

typedef struct {
    
    #define FUNC(type, name, params, body)  \
        type (*name) params;
        
        #include <ir/IR.mod>
        
    #undef FUNC
    
} _IR;

extern _IR IR;
