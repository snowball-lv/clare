#pragma once

#include <stdint.h>

#include <helpers/Types.h>
#include <collections/List.h>

typedef struct Node Node;

TYPE_DECL(IRModule)
TYPE_DECL(IRFunc)

struct Node {
    int dummy;
    int type;
    Node *left;
    Node *right;
    Node *aux;
    int32_t i32;
    IRFunc *func;
    List *args;
};

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
    Node *(*Call)(IRFunc *func, List *args);

} _IR;

extern _IR IR;

int NodeMatches(Node *root, Node *pattern);

void DeleteNodeTree(Node *root);
