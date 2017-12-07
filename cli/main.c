#include <helpers/Unused.h>
#include <mem/Mem.h>
#include <cli/Args.h>

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

static void run_compiler(Args *args);

int main(int argc, char **argv) {
    assert(MemEmpty());
    
    UNUSED(argc);
    UNUSED(argv);
    
    Args args = { 0 };
    
    for (int i = 1; i < argc; i++) {
        // printf("%s\n", argv[i]);
        
        if (strcmp("-f", argv[i]) == 0) {
            
            args.frontend = argv[i + 1];
            i++;
            continue;
            
        } else if (strcmp("-b", argv[i]) == 0) {
            
            args.backend = argv[i + 1];
            i++;
            continue;
            
        } else {
            args.source_file = argv[i];
        }
    }
    
    run_compiler(&args);
    
    assert(MemEmpty());
    return 0;
}

static void run_compiler(Args *args) {
    
    printf("--- clare ---\n");
    
    printf("frontend: %s\n", args->frontend);
    printf("backend: %s\n", args->backend);
    printf("source_file: %s\n", args->source_file);
}
