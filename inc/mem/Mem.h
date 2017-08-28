#pragma once

#include <stddef.h>

void *MemAlloc(size_t size);
void MemFree(void *ptr);

#define ALLOC(type) MemAlloc(sizeof(type))

int MemEmpty();

size_t MemAllocations();
size_t MemAllocationSize();

void MemEnableRecycling();
void MemDisableRecycling();
