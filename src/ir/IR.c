#include <ir/IR.h>

#include <helpers/Unused.h>
#include <helpers/Types.h>

#include <string.h>
#include <stdio.h>

int dummy;

#define MAX_KIDS 3

#define NT(name) NODE_TYPE_ ## name

enum {
    
    NT(INVALID),
    NT(NONE),
    
    #define FUNC(type, name, params, body)  \
        NT(name),
        
        #include <ir/IR.nodes>
        
    #undef FUNC
};

struct Node {
    
    int dummy;
    
    int node_type;
    Node *kids[MAX_KIDS];
    
    int32_t i32;
    int arg_index;
    const char *name;
    List *args;
};

Node *NodeChild(Node *node, int index) {
    return node->kids[index];
}

static Node *NewNode() {
    Node *node = ALLOC(Node);
    memset(node, 0, sizeof(Node));
    node->node_type = NODE_TYPE_NONE;
    return node;
}

void DeleteNodeTree(Node *root) {
    MemFree(root);
}

static void CollectNodes(Set *set, Node *root) {
    UNUSED(set);
    UNUSED(root);
}

static void DeleteNodeSet(Set *nodes) {
    SET_EACH(nodes, Node *, node, {
        UNUSED(node);
    });
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

static int KidsMatch(Node *root, Node *pattern) {
    for (int i = 0; i < MAX_KIDS; i++) {
        if (!NodeMatches(root->kids[i], pattern->kids[i])) {
            return 0;
        }
    }
    return 1;
}

int NodeMatches(Node *root, Node *pattern) {
    if (pattern == 0) {
        return 1;
    } else if (pattern != 0 && root == 0) {
        return 0;
    } else {
        if (root->node_type == pattern->node_type) {
            if (KidsMatch(root, pattern)) {
                return 1;
            }
        }
    }
    return 0;
}

static const char *NodeName(Node *node) {
    
    if (node == 0) {
        return "{null node}";
    }
    
    switch (node->node_type) {
        
        #define FUNC(type, name, params, body)  \
            case NT(name): return #name;
            
            #include <ir/IR.nodes>
            
        #undef FUNC
        
    }
    
    return "{invalid node type}";
}

void NodePrint(Node *node) {
    printf("%s", NodeName(node));
}
