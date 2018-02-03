#include <dummy.h>
#include <stdio.h>

#include <clare/helpers/Unused.h>
#include <clare/helpers/Error.h>

#include <f1.parser.h>
extern FILE *yyin;
const char *_current_file_name;

int main(int argc, char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    printf("--- Clare F1 CLI ---\n");
    
    for (int i = 1; i < argc; i++) {
        const char *path = argv[i];
        printf("input file: %s\n", path);
        FILE *file = fopen(path, "r");
        ASSERT(file != 0);
        
        _current_file_name = path;
        yyin = file;
        do {
            yyparse();
        } while (!feof(yyin));
        
        fclose(file);
    }
    
    return 0;
}

