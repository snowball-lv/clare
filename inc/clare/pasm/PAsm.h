#pragma once

#include <clare/pasm/PAsmTypes.h>

#include <clare/helpers/Types.h>
#include <clare/ir/IR.h>
#include <clare/collections/Set.h>
#include <clare/collections/Map.h>
#include <clare/backend/Backend.h>

#include <stdint.h>
#include <stdio.h>

// TODO: fix kludge for PAsmModule and Backend
// referencing each other

typedef struct Backend Backend;

TYPE_DECL(PAsmModule)

void PAsmModuleAddFunc(PAsmModule *mod, PAsmFunction *func);

PAsmVReg *NewPAsmVReg();
PAsmVReg *NewSpecialPAsmVReg();

void PAsmAllocate(PAsmModule *mod); 
int PAsmSpill(PAsmModule *mod); 

PAsmModule *PAsmModuleFromBackend(Backend *backend);

void PrintPAsmModule(PAsmModule *mod, FILE *output);

void OutputPAsmModuleCFG(PAsmModule *mod);
