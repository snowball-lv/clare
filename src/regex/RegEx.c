#include <regex/RegEx.h>

#include <helpers/Unused.h>
#include <helpers/Error.h>
#include <stdlib.h>
#include <collections/Set.h>


enum {
    #define TOK(name)   T_ ## name,
        TOK(INVALID)
        TOK(NONE)
        TOK(EOF)
        TOK(CHAR)
        TOK(L_BRACKET)
        TOK(HYPHEN)
        TOK(R_BRACKET)
        TOK(ANY)
    #undef TOK
};

typedef struct {
    int type;
    char c;
} Token;

const char *TokTypeName(Token tok) {
    switch (tok.type) {
        #define TOK(name)   case T_ ## name: return "T_" #name;
            TOK(INVALID)
            TOK(NONE)
            TOK(EOF)
            TOK(CHAR)
            TOK(L_BRACKET)
            TOK(HYPHEN)
            TOK(R_BRACKET)
            TOK(ANY)
        #undef TOK
        default:
            ERROR("Unknown token: %i\n", tok.type);
    }
}

typedef struct {
    const char *regex;
    Token cur;
} Input;

static Token NextToken(Input *in) {
    Token tok = { 0 };
    switch (*in->regex) {
        
        case 0:
            tok.type = T_EOF;
            break;
            
        case '[':
            tok.type = T_L_BRACKET;
            in->regex += 1;
            break;
            
        case '-':
            tok.type = T_HYPHEN;
            in->regex += 1;
            break;
            
        case ']':
            tok.type = T_R_BRACKET;
            in->regex += 1;
            break;
            
        case '.':
            tok.type = T_ANY;
            in->regex += 1;
            break;
        
        default:
            tok.type = T_CHAR;
            tok.c = *in->regex;
            in->regex += 1;
            break;
    }
    return tok;
}

static void Advance(Input *in) {
    in->cur = NextToken(in);
}


static void CompileBracketExp(Input *in) {
    ASSERT(in->cur.type == T_L_BRACKET);
    Advance(in);
    printf("bracket start\n");
    while (in->cur.type != T_EOF) {
        switch (in->cur.type) {
            
            case T_CHAR:;
                Token a = in->cur;
                Advance(in);
                if (in->cur.type == T_HYPHEN) {
                    Advance(in);
                    ASSERT(in->cur.type == T_CHAR);
                    Token b = in->cur;
                    printf("range: %c to %c\n", a.c, b.c);
                    Advance(in);
                } else {
                    printf("char: %c\n", a.c);
                }
                continue;
                    
            case T_ANY:
                printf("Any\n");
                Advance(in);
                continue;
                    
            case T_R_BRACKET:
                Advance(in);
                printf("bracket end\n");
                return;
                
            default:
                ERROR("Unhandled token: %s\n", TokTypeName(in->cur));
        }
    }
}

static void Compile(Input *in) {
    Advance(in);
    while (in->cur.type != T_EOF) {
        switch (in->cur.type) {
            
            case T_CHAR:
                printf("char: %c\n", in->cur.c);
                Advance(in);
                continue;
                    
            case T_L_BRACKET:
                CompileBracketExp(in);
                continue;
                    
            case T_ANY:
                printf("Any\n");
                Advance(in);
                continue;
                
            default:
                ERROR("Unhandled token: %s\n", TokTypeName(in->cur));
        }
    }
}

int RegExMatchStream(const char *regex, FILE *input) {
    UNUSED(regex);
    UNUSED(input);
    
    while (fgetc(input) != EOF);
    
    printf("\n");
    printf("--- %s\n", regex);
    Input in = { .regex = regex };
    Compile(&in);
    
    return 0;
}
