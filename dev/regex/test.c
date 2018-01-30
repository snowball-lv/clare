#include <clare/helpers/Unused.h>
#include <clare/regex/RegEx.h>
#include <stdio.h>
#include <clare/helpers/Error.h>
#include <clare/mem/Mem.h>
#include <assert.h>
#include <clare/collections/List.h>


#define RULE_FILE   "../../../dev/regex/lexer.rules"
#include <clare/regex/lexer.def>
#undef RULE_FILE

int main(int argc, char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    assert(MemEmpty());
    
    FILE *file = fopen("../dev/regex/test.c", "r");
    Lex(file);
    fclose(file);
    
    // printf("\n");
    // printf("allocations: %zu\n", MemAllocations());
    // printf("allocation size: %zu\n", MemAllocationSize());
    // printf("max size: %zu\n", MemMaxSize());
    
    assert(MemEmpty());
    return 0;
}
