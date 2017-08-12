#pragma once

#include <stddef.h>

void *MemAlloc(size_t size);
void MemFree(void *ptr);

#define ALLOC(type) MemAlloc(sizeof(type))

int MemEmpty();
