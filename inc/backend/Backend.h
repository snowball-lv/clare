#pragma once

#include <pasm/PAsm.h>
#include <ir/IR.h>
#include <collections/Set.h>
#include <collections/Map.h>

#include <stdio.h>

// TODO: fix kludge for PAsmModule and Backend
// referencing each other

typedef struct PAsmModule PAsmModule;
typedef struct PAsmVReg PAsmVReg;

struct Backend {
    int dummy;
    // void (*Select)(PAsmModule *mod, IRFunction *func);
    Set *(*Colors)();
    Map *(*Precoloring)();
    void (*Init)();
    void (*Deinit)();
    PAsmModule *(*IRToPAsmModule)(IRModule *irMod);
    
    void (*LoadVReg)(PAsmFunction *func, PAsmVReg *spill, PAsmVReg *tmp);
    void (*StoreVReg)(PAsmFunction *func, PAsmVReg *spill, PAsmVReg *tmp);
    
    List *(*GenPrologue)(PAsmFunction *func);
    List *(*GenEpilogue)(PAsmFunction *func);
};

typedef struct Backend Backend;
