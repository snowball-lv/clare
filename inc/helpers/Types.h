#pragma once

#include <mem/Mem.h>

#define RET(type, body) {       \
    type *self = New ## type(); \
    body;                       \
    return self;                \
}

#define CAT(a, ...) a ## __VA_ARGS__

#define TYPE_DECL(name)                     \
    typedef struct name name;               \
    name *CAT(New, name)();                 \
    void CAT(Delete, name)(name *self);

#define TYPE_DEF(name, body, ctor, dtor)    \
    struct name body;                       \
    name *CAT(New, name)() {                \
        name *self = ALLOC(name);           \
        ctor;                               \
        return self;                        \
    }                                       \
    void CAT(Delete, name)(name *self) {    \
        dtor;                               \
        MemFree(self);                      \
    }
    
#define HEAP_DECL(type)             \
    type *Heap ## type(type val);

#define HEAP_DEF(type)              \
    type *Heap ## type(type val) {  \
        type *ptr = ALLOC(type);    \
        *ptr = val;                 \
        return ptr;                 \
    }

#define HEAP(type, ...)                 \
    Heap ## type((type) __VA_ARGS__)
