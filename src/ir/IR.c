#include <ir/IR.h>

#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <mem/Mem.h>

#define NODE_TYPE_TMP   1
#define NODE_TYPE_MOV   2
#define NODE_TYPE_ADD   3
#define NODE_TYPE_I32   4

#define NODE_TYPE_ANY   5
#define NODE_TYPE_NONE  6

#define AS_STR(v) # v

Node *NewNode() {
    // Node *node = ALLOC(Node);
    Node *node = malloc(sizeof(Node));
    node->dummy = 0;
    node->type = NODE_TYPE_NONE;
    node->type_name = AS_STR(NODE_TYPE_NONE);
    node->left = 0;
    node->right = 0;
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

        case NODE_TYPE_ANY:
            node->type_name = AS_STR(NODE_TYPE_ANY);
            break;
        case NODE_TYPE_NONE:
            node->type_name = AS_STR(NODE_TYPE_NONE);
            break;
    }
    return node;
}

#define UNUSED(x) (void)(x)

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
    UNUSED(i32);
    return NodeFromType(NODE_TYPE_I32);
}

Node *Any() {
    return NodeFromType(NODE_TYPE_ANY);
}

_IR IR = {

    .dummy = 1337,

    .Tmp = Tmp,
    .Mov = Mov,
    .Add = Add,
    .I32 = I32,

    .Any = Any

};

int NodeMatches(Node *root, Node *pattern) {

    if (pattern == 0) {
        return 1;
    } else if (root == 0) {
        return 0;
    }

    if (root->type == pattern->type) {
        return
            NodeMatches(root->left, pattern->left) &&
            NodeMatches(root->right, pattern->right);
    }

    return 0;
}

const char *NodeTypeName(Node *node) {
    return node->type_name;
}
