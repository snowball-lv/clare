#include <ir/IR.h>

#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <mem/Mem.h>
#include <helpers/Unused.h>

#define NODE_TYPE_TMP   1
#define NODE_TYPE_MOV   2
#define NODE_TYPE_ADD   3
#define NODE_TYPE_I32   4

#define NODE_TYPE_EXP   5
#define NODE_TYPE_NONE  6

#define AS_STR(v) # v

Node *NewNode() {
    // Node *node = ALLOC(Node);
    Node *node = ALLOC(Node);
    node->dummy = 0;
    node->type = NODE_TYPE_NONE;
    node->type_name = AS_STR(NODE_TYPE_NONE);
    node->left = 0;
    node->right = 0;
    node->i32 = 0;
    return node;
}

Node *NodeFromType(int type) {
    Node *node = NewNode();
    node->type = type;
    switch (type) {

        case NODE_TYPE_TMP:
            node->type_name = AS_STR(NODE_TYPE_TMP);
            break;

        case NODE_TYPE_MOV:
            node->type_name = AS_STR(NODE_TYPE_MOV);
            break;
        case NODE_TYPE_ADD:
            node->type_name = AS_STR(NODE_TYPE_ADD);
            break;
        case NODE_TYPE_I32:
            node->type_name = AS_STR(NODE_TYPE_I32);
            break;

        case NODE_TYPE_NONE:
            node->type_name = AS_STR(NODE_TYPE_NONE);
            break;
    }
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

_IR IR = {

    .dummy = 1337,

    .Tmp = Tmp,
    .Mov = Mov,
    .Add = Add,
    .I32 = I32,

    .Exp = Exp

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

const char *NodeTypeName(Node *node) {
    return node->type_name;
}

void DeleteNodeTree(Node *root) {
    if (root != 0) {
        DeleteNodeTree(root->left);
        DeleteNodeTree(root->right);
        MemFree(root);
    }
}
