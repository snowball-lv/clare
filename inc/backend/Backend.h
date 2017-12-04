#pragma once

#include <pasm/PAsm.h>
#include <ir/IR.h>

typedef struct {
    int dummy;
    void (*Select)(PAsmModule *mod, IRFunction *func);
    void (*Init)();
    void (*Deinit)();
} Backend;
