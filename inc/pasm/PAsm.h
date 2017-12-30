#pragma once

#include <pasm/PAsmTypes.h>

#include <helpers/Types.h>
#include <ir/IR.h>
#include <collections/Set.h>
#include <collections/Map.h>
#include <backend/Backend.h>

#include <stdint.h>
#include <stdio.h>

// TODO: fix kludge for PAsmModule and Backend
// referencing each other

typedef struct Backend Backend;

TYPE_DECL(PAsmModule)

HEAP_DECL(PAsmVReg)

HEAP_DECL(PAsmOp)

void PAsmModuleAddFunc(PAsmModule *mod, PAsmFunction *func);

PAsmVReg *NewPAsmVReg();
PAsmVReg *NewSpecialPAsmVReg();

void PAsmAllocate(PAsmModule *mod); 
int PAsmSpill(PAsmModule *mod); 

PAsmModule *PAsmModuleFromBackend(Backend *backend);

void PrintPAsmModule(PAsmModule *mod, FILE *output);

void OutputPAsmModuleCFG(PAsmModule *mod);
