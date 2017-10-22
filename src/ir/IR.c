#include <ir/IR.h>

#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <mem/Mem.h>
#include <helpers/Unused.h>
#include <collections/Set.h>
#include <collections/StringMap.h>

#define NODE_TYPE_TMP   1
#define NODE_TYPE_MOV   2
#define NODE_TYPE_ADD   3
#define NODE_TYPE_I32   4

#define NODE_TYPE_EXP   5
#define NODE_TYPE_NONE  6

Node *NewNode() {
    // Node *node = ALLOC(Node);
    Node *node = ALLOC(Node);
    node->dummy = 0;
    node->type = NODE_TYPE_NONE;
    node->left = 0;
    node->right = 0;
    node->aux = 0;
    node->i32 = 0;
    node->func = 0;
    node->args = 0;
    return node;
}

Node *NodeFromType(int type) {
    Node *node = NewNode();
    node->type = type;
    return node;
}

Node *Tmp() {
    return NodeFromType(NODE_TYPE_TMP);
}

Node *Mov(Node *dst, Node *src) {
    Node *node = NodeFromType(NODE_TYPE_MOV);
    node->left = dst;
    node->right = src;
    return node;
}

Node *Add(Node *left, Node *right) {
    Node *node = NodeFromType(NODE_TYPE_ADD);
    node->left = left;
    node->right = right;
    return node;
}

Node *I32(int32_t i32) {
    Node *node = NodeFromType(NODE_TYPE_I32);
    node->i32 = i32;
    return node;
}

Node *Exp() {
    return NodeFromType(NODE_TYPE_EXP);
}

Node *Branch(Node *cond, Node *t, Node *f) {
    RET(Node, {
        self->left = t;
        self->right = f;
        self->aux = cond;
    });
}

Node *Eq(Node *a, Node *b) {
    RET(Node, {
        self->left = a;
        self->right = b;
    });
}

Node *Ret(Node *n) {
    RET(Node, {
        self->left = n;
    });
}

Node *Call(IRFunc *func, List *args) {
    RET(Node, {
        self->func = func;
        self->args = args;
    });
}

_IR IR = {

    .dummy = 1337,

    .Tmp = Tmp,
    .Mov = Mov,
    .Add = Add,
    .I32 = I32,

    .Exp = Exp,

    .Branch = Branch,
    .Eq = Eq,
    .Ret = Ret,
    .Call = Call,
};

static int NodeIs(Node *node, int type) {
    switch (type) {
        
        case NODE_TYPE_EXP:
        switch (node->type) {
            case NODE_TYPE_TMP:
            case NODE_TYPE_MOV:
            case NODE_TYPE_ADD:
            case NODE_TYPE_I32:
                return 1;
        }
        break;
        
    }
    return node->type == type;
}

int NodeMatches(Node *root, Node *pattern) {

    if (pattern == 0) {
        return 1;
    } else if (root == 0) {
        return 0;
    }

    if (NodeIs(root, pattern->type)) {
        return
            NodeMatches(root->left, pattern->left) &&
            NodeMatches(root->right, pattern->right);
    }

    return 0;
}

static void FillNodeSet(Set *nodes, Node *root) {
    if (root != 0) {
        
        FillNodeSet(nodes, root->left);
        FillNodeSet(nodes, root->right);
        FillNodeSet(nodes, root->aux);
        
        if (root->args != 0) {
            LIST_EACH(root->args, Node *, arg, {
                FillNodeSet(nodes, arg);
            });
        }
        
        SetAdd(nodes, root);
    }
}

void DeleteNodeTree(Node *root) {
    Set *nodes = NewSet();
    FillNodeSet(nodes, root);
    SET_EACH(nodes, Node *, n, {
        
        if (n->args != 0) {
            DeleteList(n->args);
        }
        
        MemFree(n);
    });
    DeleteSet(nodes);
}

TYPE_DEF(IRModule, {
    int dummy;
    StringMap *funcs;
}, {
    self->funcs = NewStringMap();
}, {
    DeleteStringMap(self->funcs);
})

TYPE_DEF(IRFunc, {
    int dummy;
    const char *name;
}, {}, {})

IRFunc *FuncFromName(const char *name) {
    RET(IRFunc, {
        self->name = name;
    });
}

const char *IRFuncGetName(IRFunc *func) {
    return func->name;
}

void IRModuleAddFunc(IRModule *mod, IRFunc *func) {
    StringMapPut(
        mod->funcs,
        IRFuncGetName(func),
        func);
}

IRFunc *IRModuleGetFunc(IRModule *mod, const char *name) {
    return StringMapGet(mod->funcs, name);
}
