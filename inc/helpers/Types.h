#pragma once

#include <mem/Mem.h>

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