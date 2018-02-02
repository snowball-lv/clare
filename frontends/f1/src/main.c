#include <dummy.h>
#include <stdio.h>

#include <clare/helpers/Unused.h>
#include <clare/helpers/Error.h>
#include <clare/collections/List.h>
#include <clare/regex/RegEx.h>
#include <clare/mem/Mem.h>

#define RULE_FILE   <lexer.rules>
    #include <clare/regex/lexer.def>
#undef RULE_FILE

int main(int argc, char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    printf("--- Clare F1 CLI ---\n");
    
    for (int i = 1; i < argc; i++) {
        const char *path = argv[i];
        printf("input file: %s\n", path);
        FILE *file = fopen(path, "r");
        ASSERT(file != 0);
        
        Lex(file);
        
        fclose(file);
    }
    
    return 0;
}

