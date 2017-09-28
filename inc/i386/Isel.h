#pragma once

#include <ir/IR.h>
#include <collections/List.h>

typedef struct Isel Isel;

Isel *NewIsel();
void DeleteIsel(Isel *isel);

List *IselSelect(Isel *isel, Node *root);
