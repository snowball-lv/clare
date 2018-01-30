

#include <clare/helpers/Unused.h>
#include <clare/helpers/Error.h>
#include <clare/mem/Mem.h>
#include <clare/cli/Args.h>

#include <dummy.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include <clare/frontend/Frontend.h>
#include <clare/backend/Backend.h>
#include <clare-ir.h>
#include <i386.h>


static void run_compiler(Args *args);

int main(int argc, char **argv) {
    assert(MemEmpty());
    
    Args args = { 0 };
    
    for (int i = 1; i < argc; i++) {
        
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
            ERROR("stray argument: %s\n", argv[i]);
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

static Frontend *GetFrontend(const char *name) {
    if (strcmp(name, "clare-ir") == 0) {
        return &clare_IR_Frontend;
    }
    return 0;
}

static Backend *GetBackend(const char *name) {
    if (strcmp(name, "i386") == 0) {
        return &i386_Backend;
    }
    return 0;
}

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
    
    DeleteIRModule(irMod);
    
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
}
