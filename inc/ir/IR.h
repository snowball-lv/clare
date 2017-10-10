#pragma once

#include <stdint.h>

#include <helpers/Types.h>

typedef struct Node Node;

struct Node {
    int dummy;
    int type;
    char *type_name;
    Node *left;
    Node *right;
    Node *aux;
    int32_t i32;
};

TYPE_DECL(IRModule)
TYPE_DECL(IRFunc)

IRFunc *FuncFromName(const char *name);

typedef struct {

    int dummy;

    Node *(*Tmp)();

    Node *(*Mov)(Node *dst, Node *src);
    Node *(*Add)(Node *left, Node *right);
    Node *(*I32)(int32_t i32);

    Node *(*Exp)();
    
    Node *(*Branch)(Node *cond, Node *t, Node *f);
    Node *(*Eq)(Node *a, Node *b);
    Node *(*Ret)(Node *n);
    Node *(*Call)(IRFunc *func);

} _IR;

extern _IR IR;

int NodeMatches(Node *root, Node *pattern);

const char *NodeTypeName(Node *node);
void DeleteNodeTree(Node *root);
