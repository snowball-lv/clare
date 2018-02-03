
%{
#include <f1.parser.h>
#include <clare/helpers/Unused.h>

static const  char *copy_string(const char *str);
static int _last_line = 0;
static int _column = 0;

#define YY_USER_ACTION {            \
    if (yylineno != _last_line) {   \
        _column = 1;                \
        _last_line = yylineno;      \
    }                               \
    yylloc.first_line = yylineno;   \
    yylloc.last_line = yylineno;    \
    yylloc.first_column = _column;  \
    _column = _column + yyleng;     \
    yylloc.last_column = _column;   \
} 

%}

%option noyywrap
%option yylineno

ID          [a-zA-Z_][0-9a-zA-Z_]*
INTEGER     [0-9]+
STRING      \"(\\.|[^\\\"])*\"

%%

[\n\r\t ]       ; // ignore
:               { return COLON; }
,               { return COMMA; }
\+              { return ADD; }
\-              { return SUB; }
\*              { return MUL; }
\/              { return DIV; }
\<              { return LT; }
\(              { return L_PAREN; }
\)              { return R_PAREN; }
\[              { return L_BRACKET; }
\]              { return R_BRACKET; }
=               { return ASG; }
function        { return FUNCTION; }
while           { return WHILE; }
do              { return DO; }
params          { return PARAMS; }
body            { return BODY; }
end             { return END; }
return          { return RETURN; }
int             { return TYPE_INT; }
string          { return TYPE_STR; }
{INTEGER}       { yylval.ival = atoi(yytext); return INT; }
{STRING}        { yylval.str = copy_string(yytext); return STR; }
{ID}            { yylval.str = copy_string(yytext); return ID; }
.               { printf("Caught: [%s]\n", yytext); return -1; }

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
