#include <helpers/Unused.h>
#include <mem/Mem.h>
#include <cli/Args.h>

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include <frontends/Frontends.h>
#include <backends/Backends.h>

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
            
        } else if (strcmp("-i", argv[i]) == 0) {
            
            args.source_file = argv[i + 1];
            i++;
            continue;
            
        } else if (strcmp("-o", argv[i]) == 0) {
            
            args.output_file = argv[i + 1];
            i++;
            continue;
            
        } else {
            printf("stray argument: %s\n", argv[i]);
            exit(1);
        }
    }
    
    run_compiler(&args);
    
    printf("allocations: %zu\n", MemAllocations());
    printf("allocation size: %zu\n", MemAllocationSize());
    
    assert(MemEmpty());
    return 0;
}

static void compile_file(   FILE *src, 
                            Frontend *frontend, 
                            Backend *backend,
                            FILE *out);

static void run_compiler(Args *args) {
    
    printf("--- clare ---\n");
    puts("");
    
    printf("Arguments:\n");
    printf("    frontend: %s\n", args->frontend);
    printf("    backend: %s\n", args->backend);
    printf("    source_file: %s\n", args->source_file);
    printf("    output_file: %s\n", args->output_file);
    puts("");
    
    Frontend *frontend = GetFrontend(args->frontend);
    if (frontend == 0) {
        printf("no frontend available for: %s\n", args->frontend);
        exit(1);
        return;
    }
    
    Backend *backend = GetBackend(args->backend);
    if (backend == 0) {
        printf("no backend available for: %s\n", args->backend);
        exit(1);
        return;
    }
    
    FILE *src = fopen(args->source_file, "r");
    if (src == 0) {
        printf("no such file: %s\n", args->source_file);
        exit(1);
        return;
    }
    
    FILE *out = fopen(args->output_file, "w");
    compile_file(src, frontend, backend, out);
    fclose(out);
    
    fclose(src);
}

static void compile_file(   FILE *src,
                            Frontend *frontend,
                            Backend *backend,
                            FILE *out) {
                                
    IRModule *irMod = frontend->SourceToIRModule(src);
    
    backend->Init();
    PAsmModule *pasmMod = backend->IRToPAsmModule(irMod);
    
    OutputPAsmModuleCFG(pasmMod);
    
    int i = 1;
    do {
        printf("reg. alloc iteration: %d\n", i++);
        PAsmAllocate(pasmMod);
    } while (PAsmSpill(pasmMod));
    
    // PAsmAllocate(pasmMod);
    // PAsmSpill(pasmMod);
    // PAsmAllocate(pasmMod);
    
    PrintPAsmModule(pasmMod, out);
    
    DeletePAsmModule(pasmMod);
    backend->Deinit();
    
    DeleteIRModule(irMod);
}
