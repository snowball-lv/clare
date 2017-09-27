#pragma once

#include <mem/Mem.h>

#define TYPE_DECL(name)                     \
    typedef struct name name;               \
    name *New ## name();                    \
    void Delete ## name(name *self);

#define TYPE_DEF(name, body, ctor, dtor)    \
    struct name body;                       \
    name *New ## name() {                   \
        name *self = ALLOC(name);           \
        ctor;                               \
        return self;                        \
    }                                       \
    void Delete ## name(name *self) {       \
        dtor;                               \
        MemFree(self);                      \
    }
