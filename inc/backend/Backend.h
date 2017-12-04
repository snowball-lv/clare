#pragma once

#include <pasm/PAsm.h>
#include <ir/IR.h>
#include <collections/Set.h>
#include <collections/Map.h>

typedef struct {
    int dummy;
    void (*Select)(PAsmModule *mod, IRFunction *func);
} Backend;
