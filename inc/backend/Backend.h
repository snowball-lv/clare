#pragma once

#include <pasm/PAsm.h>
#include <ir/IR.h>
#include <collections/Set.h>
#include <collections/Map.h>

// TODO: fix kludge for PAsmModule and Backend
// referencing each other

typedef struct PAsmModule PAsmModule;

struct Backend {
    int dummy;
    void (*Select)(PAsmModule *mod, IRFunction *func);
    Set *(*Colors)();
    Map *(*Precoloring)();
    void (*Init)();
    void (*Deinit)();
    PAsmModule *(*IRToPAsmModule)(IRModule *irMod);
};

typedef struct Backend Backend;
