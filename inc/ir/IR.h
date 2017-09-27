#pragma once

#include <stdint.h>

typedef struct Node Node;

struct Node {
    int dummy;
    int type;
    char *type_name;
    Node *left;
    Node *right;
};

typedef struct {

    int dummy;

    Node *(*Tmp)();

    Node *(*Mov)(Node *dst, Node *src);
    Node *(*Add)(Node *left, Node *right);
    Node *(*I32)(int32_t i32);

    Node *(*Exp)();

} _IR;

extern _IR IR;

int NodeMatches(Node *root, Node *pattern);

const char *NodeTypeName(Node *node);
void DeleteNodeTree(Node *root);
