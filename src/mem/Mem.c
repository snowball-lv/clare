#include <mem/Mem.h>

#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

static intptr_t counter = 0;

static void Add(void *ptr) {
    counter += (intptr_t) ptr;
}

static void Remove(void *ptr) {
    counter -= (intptr_t) ptr;
}

void *MemAlloc(size_t size) {
    void *ptr = malloc(size);
    Add(ptr);
    return ptr;
}

void MemFree(void *ptr) {
    free(ptr);
    Remove(ptr);
}

int MemEmpty() {
    return counter == 0;
}
