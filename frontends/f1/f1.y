
%{
#include <stdio.h>
#include <stdlib.h>
extern int yylex();
void yyerror (char const *msg);
extern int LexerLineNum();
extern FILE *yyin;
%}

%union {
    const char *str;
}

%token FUNCTION
%token ID
%token COLON
%token END
%token TYPE_INT
%token PARAMS
%token BODY

%%
module
    : function module
    | // nothing
    ;
    
function
    : FUNCTION ID COLON type function_content END
    ;
    
function_content
    : function_params function_body
    ;
    
function_params
    : PARAMS param_decls
    | // nothing
    ;
    
param_decls
    : ID COLON type param_decls
    | // nothing
    ;
    
function_body
    : BODY
    ;
    
type
    : TYPE_INT
    ;
%%

void yyerror (char const *msg) {
    printf("Parse error: %s\n", msg);
    int line = LexerLineNum();
    printf("Line: %d\n", line);
    
    fpos_t pos;
    fgetpos(yyin, &pos);
    rewind(yyin);
    int counter = 0;
    
    while (!feof(yyin)) {
        if (counter == line - 1) {
            // right line
            while (!feof(yyin)) {
                int c = fgetc(yyin);
                printf("%c", c);
                if (c == '\n') {
                    break;
                }
            }
            break;
        } else {
            while (!feof(yyin)) {
                int c = fgetc(yyin);
                if (c == '\n') {
                    counter++;
                    break;
                }
            }
        }
    }
    
    fsetpos(yyin, &pos);
    
    exit(-1);
}
