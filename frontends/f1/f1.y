
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
%token TYPE_INT TYPE_STR
%token PARAMS
%token BODY
%token RETURN
%token ADD SUB MUL DIV
%token ASG
%token L_PAREN R_PAREN
%token L_BRACKET R_BRACKET
%token INT STR
%token COMMA
%token UMINUS
%token WHILE
%token DO
%token LT

%left ADD SUB
%left MUL DIV
%left LT
%right L_BRACKET
%left R_BRACKET
%left UMINUS

%start module

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
    | fcall
    | var_decl
    | WHILE exp DO stm_list END
    | exp ASG exp
    ;
    
var_decl
    : ID COLON type ASG exp
    ;
    
exp
    : L_PAREN exp R_PAREN
    | ID
    | INT
    | STR
    | fcall
    | exp ADD exp
    | exp SUB exp
    | exp MUL exp
    | exp DIV exp
    | exp LT exp
    | exp L_BRACKET exp R_BRACKET
    | SUB exp                       %prec UMINUS
    ;
    
fcall
    : ID L_PAREN fcall_args R_PAREN
    ;

fcall_args
    : exp
    | exp COMMA fcall_args
    | // nothing
    ;
    
type
    : TYPE_INT
    | TYPE_STR
    | type L_BRACKET R_BRACKET
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
