#pragma once

#include <ir/IR.h>

typedef struct Isel Isel;

Isel *NewIsel();
void DeleteIsel(Isel *isel);

void IselSelect(Isel *isel, Node *root);
