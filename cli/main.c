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
            
        } else {
            args.source_file = argv[i];
        }
    }
    
    run_compiler(&args);
    
    assert(MemEmpty());
    return 0;
}

static void compile_file(FILE *file, Frontend *frontend, Backend *backend);

static void run_compiler(Args *args) {
    
    printf("--- clare ---\n");
    puts("");
    
    printf("Arguments:\n");
    printf("    frontend: %s\n", args->frontend);
    printf("    backend: %s\n", args->backend);
    printf("    source_file: %s\n", args->source_file);
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
    
    FILE *file = fopen(args->source_file, "r");
    if (file == 0) {
        printf("no such file: %s\n", args->source_file);
        exit(1);
        return;
    }
    
    compile_file(file, frontend, backend);
    
    fclose(file);
}

static void compile_file(FILE *file, Frontend *frontend, Backend *backend) {
    
    UNUSED(file);
    UNUSED(frontend);
    UNUSED(backend);

    IRModule *irMod = frontend->SourceToIRModule(file);
    UNUSED(irMod);
    
    PAsmInit();
    backend->Init();
    PAsmModule *pasmMod = backend->IRToPAsmModule(irMod);
    
    PAsmAllocate(pasmMod);
    
    FILE *output = fopen("output.asm", "w");
    PrintPAsmModule(pasmMod, output);
    fclose(output);
    
    DeletePAsmModule(pasmMod);
    backend->Deinit();
    PAsmDeinit();
    
    DeleteIRModule(irMod);
}
