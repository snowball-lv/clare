#include <dummy.h>
#include <stdio.h>

#include <clare/helpers/Unused.h>
#include <clare/helpers/Error.h>

#include <AST.h>

#include <f1.parser.h>
extern FILE *yyin;

static AST *ParseFile(const char *path);

int main(int argc, char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    printf("--- Clare F1 CLI ---\n");
    
    for (int i = 1; i < argc; i++) {
        const char *path = argv[i];
        printf("input file: %s\n", path);
        ParseFile(path);
    }
    
    return 0;
}

static AST *ParseFile(const char *path) {
    
    FILE *file = fopen(path, "r");
    ASSERT(file != 0);

    AST *ast = 0;

    yyin = file;
    do {
        yyparse(&ast);
    } while (!feof(yyin));

    fclose(file);
    return ast;
}