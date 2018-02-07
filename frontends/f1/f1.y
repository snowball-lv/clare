
%{
    
#include <stdio.h>
#include <stdlib.h>
#include <AST.h>
#include <clare/helpers/Unused.h>
#include <clare/collections/List.h>

extern int yylex();
extern FILE *yyin;
void yyerror (AST **ast, char const *msg);

%}

%code requires {
    #include <AST.h>
}

%locations
%union {
    const char *str;
    int ival;
    List *list;
    AST *ast;
}

%parse-param { AST **ast }

%token FUNCTION PARAMS BODY END
%token WHILE DO
%token RETURN
%token TYPE_INT TYPE_STR
%token ID
%token COLON
%token ASG
%token L_PAREN R_PAREN
%token L_BRACKET R_BRACKET
%token INT STR
%token COMMA
%token UMINUS
%token ADD SUB MUL DIV
%token LT

%precedence SIMPLE

%left   LT
%left   ADD SUB
%left   MUL DIV
%right  UMINUS
%left   L_BRACKET L_PAREN

%type <list> flist
%type <ast> function

%start module

%%
module
    : flist         { *ast = ASTMod($1); }
    | %empty        { *ast = ASTMod(NewList()); }
    ;
    
flist
    : flist function    { $$ = $1; ListAdd($$, $2);  }
    | function          { $$ = NewList(); ListAdd($$, $1); }
    ;
    
function
    : FUNCTION ID COLON type function_content END { 
        $$ = 0;
    }
    ;
    
function_content
    : function_params function_body
    ;
    
function_params
    : PARAMS param_decls
    | PARAMS
    | %empty
    ;
    
param_decls
    : param_decls param_decl
    | param_decl
    ;
    
param_decl
    : ID COLON type
    ;
    
function_body
    : BODY stm_list
    | BODY
    ;
    
stm_list
    : stm_list stm
    | stm
    ;
    
stm
    : RETURN exp                    %prec SIMPLE
    | exp                           %prec SIMPLE
    | var_decl
    | WHILE exp DO stm_list END
    | exp ASG exp                   %prec SIMPLE
    ;
    
var_decl
    : ID COLON type ASG exp         %prec SIMPLE
    ;
    
exp
    : L_PAREN exp R_PAREN
    | ID                            %prec SIMPLE
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
    | ID L_PAREN R_PAREN
    ;

fcall_args
    : fcall_args COMMA exp
    | exp
    ;
    
type
    : TYPE_INT
    | TYPE_STR
    | type L_BRACKET R_BRACKET
    ;
%%

void yyerror (AST **ast, char const *msg) {
    UNUSED(ast);
    UNUSED(msg);
    
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
