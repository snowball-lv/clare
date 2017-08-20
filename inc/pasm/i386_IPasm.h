#pragma once

#include <pasm/IPasm.h>

extern IPasm i386_IPasm;

typedef struct i386_Instr i386_Instr;

typedef struct {
    i386_Instr *(*nop)();
} _i386;

extern _i386 i386;
