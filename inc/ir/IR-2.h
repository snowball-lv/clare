#pragma once

#include <helpers/Types.h>

#define IR_PREFIX(name) IR ## name

TYPE_DECL(IR_PREFIX(Module))
TYPE_DECL(IR_PREFIX(Func))
