#pragma once

#include <clare/collections/List.h>

typedef struct {
    int dummy;
} AST;

AST *ASTNull();
AST *ASTMod(List *flist);
AST *ASTFDef(const char *name, AST *type, AST *);
