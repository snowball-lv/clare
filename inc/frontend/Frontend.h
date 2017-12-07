#pragma once

#include <ir/IR.h>

#include <stdio.h>

typedef struct {
    int dummy;
    IRModule *(*SourceToIRModule)(FILE *source_file);
} Frontend;
