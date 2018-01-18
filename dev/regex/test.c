#include <helpers/Unused.h>
#include <regex/RegEx.h>
#include <stdio.h>
#include <helpers/Error.h>
#include <mem/Mem.h>
#include <assert.h>
#include <collections/List.h>


#define RULE_FILE   "../../dev/regex/lexer.rules"
    #include <regex/lexer.def>
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
