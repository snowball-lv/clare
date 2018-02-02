
%{
#include <f1.parser.h>
#include <clare/helpers/Unused.h>

static const  char *copy_string(const char *str);
static int _lexer_line_counter = 1;
%}

%option noyywrap 

ID      [a-zA-Z_][0-9a-zA-Z_]*

%%

[\r\t ]         ; // ignore
:               { return COLON; }
function        { return FUNCTION; }
params          { return PARAMS; }
body            { return BODY; }
end             { return END; }
int             { return TYPE_INT; }
{ID}            { yylval.str = copy_string(yytext); return ID; }
\n              { _lexer_line_counter++; }
.               { printf("Caught: [%s]\n", yytext); exit(-1); }

%%

void _unused_lexer_symbol_kludge() {
    UNUSED(input);
    UNUSED(yyunput);
}

static const char *copy_string(const char *str) {
    char *copy = malloc(strlen(str) + 1);
    strcpy(copy, str);
    return copy;
}

int LexerLineNum() {
    return _lexer_line_counter;
}
