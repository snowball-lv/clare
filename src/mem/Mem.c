#include <mem/Mem.h>

#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

static intptr_t counter = 0;
static size_t allocations = 0;
static size_t allocation_size = 0;
static size_t current_size = 0;
static size_t max_size = 0;

static int recycling_enabled = 0;

size_t MemMaxSize() {
    return max_size;
}

static void Add(void *ptr) {
    counter += (intptr_t) ptr;
}

static void Remove(void *ptr) {
    counter -= (intptr_t) ptr;
}

void *MemAlloc(size_t size) {

    size_t *ptr = malloc(size + sizeof(size_t));
    Add(ptr);
    *ptr = size;

    allocations++;
    allocation_size += size + sizeof(size_t);
    
    current_size += size + sizeof(size_t);
    max_size = current_size > max_size ? current_size : max_size;

    return ptr + 1;
}

void MemFree(void *ptr) {
    size_t *rptr = (size_t *) ptr - 1;
    
    size_t size = *rptr;
    current_size = (current_size - size) - sizeof(size_t);
    
    free(rptr);
    Remove(rptr);
}

int MemEmpty() {
    return counter == 0;
}

size_t MemAllocations() {
    return allocations;
}

size_t MemAllocationSize() {
    return allocation_size;
}

void MemEnableRecycling() {
    recycling_enabled = 1;
}

void MemDisableRecycling() {
    recycling_enabled = 0;
}
