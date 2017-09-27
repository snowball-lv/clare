#pragma once

#include <ir/IR.h>

void I386_Munch(Node *root);

typedef struct I386_Muncher I386_Muncher;

I386_Muncher *New_I386_Muncher();
void Delete_I386_Muncher(I386_Muncher *muncher);
