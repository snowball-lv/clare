#pragma once

#include <ir/IR.h>
#include <collections/List.h>
#include <collections/Set.h>

List *i386Munch(Node *root);
void DeleteOps(List *ops);
void PrintOps(List *ops);

Set *OpUse(void *op);
Set *OpDef(void *op);

int VRegIndex(void *vreg);
