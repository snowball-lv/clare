#include <backends/i386/i386.h>

#include <helpers/Unused.h>
#include <collections/List.h>

#include <stdio.h>
#include <assert.h>

int dummy;

#define MANGLE(name)    i386 ## name
#define RULE_FILE       <backends/i386/i386.rules>

    #include <ir/muncher.def>
    
#undef RULE_FILE
#undef MANGLE
