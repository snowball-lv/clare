
#include <helpers/Unused.h>
#include <regex/RegEx.h>
#include <stdio.h>
#include <helpers/Error.h>
#include <mem/Mem.h>
#include <assert.h>


#define RULE_FILE   "../../dev/regex/lexer.rules"
    #include <regex/lexer.def>
#undef RULE_FILE

int main(int argc, char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    assert(MemEmpty());
    
    FILE *file = fopen("../dev/regex/test.txt", "r");
    Lex(file);
    fclose(file);
    
    assert(MemEmpty());
    return 0;
}
