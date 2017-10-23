#include <ir/IR.h>

#include <helpers/Unused.h>

int dummy;

struct Node {
    int dummy;
};

void DeleteNodeTree(Node *root) {
    UNUSED(root);
}

#define FUNC(type, name, params, body)  \
    static type _ ## name params body
    
    #include <ir/IR.mod>
    
#undef FUNC

_IR IR = {

    #define FUNC(type, name, params, body)  \
        .name = _ ## name,
        
        #include <ir/IR.mod>
        
    #undef FUNC
    
};
