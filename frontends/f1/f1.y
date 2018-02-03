
%{
    
#include <stdio.h>
#include <stdlib.h>

extern int yylex();
void yyerror (char const *msg);
extern FILE *yyin;
extern const char *_current_file_name;

%}

%locations
%union {
    const char *str;
    int ival;
}

%token FUNCTION
%token ID
%token COLON
%token END
%token TYPE_INT
%token PARAMS
%token BODY
%token RETURN
%token ADD SUB MUL DIV
%token L_PAREN R_PAREN
%token INT

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
    : BODY stm_list
    ;
    
stm_list
    : stm stm_list
    | // nothing
    ;
    
stm
    : RETURN exp
    ;
    
exp
    : exp ADD term
	| exp SUB term
    | term
    ;
    
term
    : term MUL factor
    | term DIV factor
    | factor
    ;
    
factor
    : ID
    | L_PAREN exp R_PAREN
    | INT
    ;
    
type
    : TYPE_INT
    ;
%%

void yyerror (char const *msg) {
    
    printf(
        "%d:%d - %d:%d\n", 
        yylloc.first_line, yylloc.first_column,
        yylloc.last_line, yylloc.last_column);
    
    printf("%s\n", msg);
    
    fpos_t pos;
    fgetpos(yyin, &pos);
    
    int line = 1;
    rewind(yyin);
    
    while (line != yylloc.first_line) {
        int c = fgetc(yyin);
        if (c == '\n') {
            line++;
        }
    }
    
    int in_error = 0;
    int column = 1;
    while (line <= yylloc.last_line) {
        
        int c = fgetc(yyin);
        if (c == '\n') {
            line++;
            column = 0;
        } else {
            column++;
        }
        
        if (!in_error) {
            if (line == yylloc.first_line && column >= yylloc.first_column) {
                in_error = 1;
                printf("\x1b[31m");
            }
        }
        
        if (in_error) {
            if (line == yylloc.last_line && column >= yylloc.last_column) {
                in_error = 0;
                printf("\x1b[39;49m");
            }
        }
        
        putchar(c);
    }
    
    // printf("\x1b[31m");
    // printf("\x1b[39;49m");
    
    fsetpos(yyin, &pos);
    
    exit(-1);
}
