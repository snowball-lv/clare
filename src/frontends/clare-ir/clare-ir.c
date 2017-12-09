#include <frontends/clare-ir/clare-ir.h>

#include <helpers/Unused.h>
#include <ir/IR.h>
#include <collections/Map.h>

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

typedef struct {
    FILE *file;
} Source;

typedef struct {
    int type;
    union {
        const char *id;
        const char *tmp;
        int num; 
    };
} Token;

#define TOK(name)   TOK_ ## name
enum {
    TOK(INVALID),
    TOK(NONE),
    TOK(EOF),
    TOK(ID),
    TOK(NUM),
    TOK(TMP),
    TOK(L_PAREN),
    TOK(R_PAREN),
    TOK(COMMA),
};
#undef TOK

static const char *TokName(Token tok) {
    switch (tok.type) {
        #define TOK(name)   case TOK_ ## name: return #name;
            TOK(INVALID)
            TOK(NONE)
            TOK(EOF)
            TOK(ID)
            TOK(NUM)
            TOK(TMP)
            TOK(L_PAREN)
            TOK(R_PAREN)
            TOK(COMMA)
        #undef TOK
    }
    return "{unknown tok type}";
}

static Token NextToken(Source *src) {

    int c = fgetc(src->file);
    while (isspace(c)) {
        c = fgetc(src->file);
    }
    
    char buffer[256] = { 0 };
    int index = 0;
    int tok = TOK_NONE;
    
    #define PUSH(c)     buffer[index++] = (char) c
    
    switch (c) {
        case '(': return (Token) { .type = TOK_L_PAREN };
        case ')': return (Token) { .type = TOK_R_PAREN };
        case ',': return (Token) { .type = TOK_COMMA};
        case EOF: return (Token) { .type = TOK_EOF };
        case '$': tok = TOK_TMP; break;
        default: {
            if (isalpha(c) || c == '_') {
                PUSH(c);
                tok = TOK_ID;
            } else if (isdigit(c)) {
                PUSH(c);
                tok = TOK_NUM;
            } else {
                fprintf(stderr, "illegal character: %c\n", (char) c);
                exit(1);
            }
        }
    }
    
    while (1) {
        c = fgetc(src->file);
        switch (tok) {
            
            case TOK_ID: {
                if (isalnum(c) || c == '_') {
                    PUSH(c);
                } else {
                    if (c != EOF) {
                        ungetc(c, src->file);
                    }
                    return (Token) {
                        .type = TOK_ID,
                        .id = strcpy(malloc(strlen(buffer) + 1), buffer)
                    };
                }
                break;
            }
            
            case TOK_NUM: {
                if (isdigit(c)) {
                    PUSH(c);
                } else {
                    if (c != EOF) {
                        ungetc(c, src->file);
                    }
                    return (Token) {
                        .type = TOK_NUM,
                        .num = atoi(buffer)
                    };
                }
                break;
            }
            
            case TOK_TMP: {
                if (isalnum(c) || c == '_') {
                    PUSH(c);
                } else {
                    if (index == 0) {
                        fprintf(stderr, "$ should be followed by name\n");
                        exit(1);
                    }
                    if (c != EOF) {
                        ungetc(c, src->file);
                    }
                    return (Token) {
                        .type = TOK_TMP,
                        .tmp = strcpy(malloc(strlen(buffer) + 1), buffer)
                    };
                }
                break;
            }
        }
    }
    
    #undef PUSH
}

static void ParseSource(IRModule *mod, Source *src);

static IRModule *_SourceToIRModule(FILE *source_file) {
    UNUSED(source_file);
    
    IRModule *irMod = NewIRModule();
    
    Source source = { 
        .file = source_file
    };
    
    ParseSource(irMod, &source);
    
    List *funcs = IRModuleFunctions(irMod);
    LIST_EACH(funcs, IRFunction *, func, {
        printf("\n");
        printf("function: [%s]\n", IRFunctionName(func));
        printf("\n");
        Node *body = IRFunctionBody(func);
        IRPrintTree(body);
        printf("\n");
    });
    DeleteList(funcs);
    
    return irMod;
}

static Node *ParseNode(Token tok, Source *src, Map *tmpCache) {
    
    #define ParseNode(tok, src) ParseNode(tok, src, tmpCache)
    
    if (tok.type == TOK_ID) {
        
        if (strcmp(tok.id, "mov") == 0) {
            
            assert(NextToken(src).type == TOK_L_PAREN);
            Node *a = ParseNode(NextToken(src), src);
            assert(NextToken(src).type == TOK_COMMA);
            Node *b = ParseNode(NextToken(src), src);
            assert(NextToken(src).type == TOK_R_PAREN);
            
            return IR.Mov(a, b);
            
        } else if (strcmp(tok.id, "arg") == 0) {
            
            assert(NextToken(src).type == TOK_L_PAREN);
            Token tok = NextToken(src);
            assert(tok.type == TOK_NUM);
            assert(NextToken(src).type == TOK_R_PAREN);
            
            return IR.Arg(tok.num);
            
        } else if (strcmp(tok.id, "i32") == 0) {
            
            assert(NextToken(src).type == TOK_L_PAREN);
            Token tok = NextToken(src);
            assert(tok.type == TOK_NUM);
            assert(NextToken(src).type == TOK_R_PAREN);
            
            return IR.I32(tok.num);
            
        } else if (strcmp(tok.id, "add") == 0) {
            
            assert(NextToken(src).type == TOK_L_PAREN);
            Node *left = ParseNode(NextToken(src), src);
            assert(NextToken(src).type == TOK_COMMA);
            Node *right = ParseNode(NextToken(src), src);
            assert(NextToken(src).type == TOK_R_PAREN);
            
            return IR.Add(left, right);
            
        } else if (strcmp(tok.id, "ret") == 0) {
            
            assert(NextToken(src).type == TOK_L_PAREN);
            Node *node = ParseNode(NextToken(src), src);
            assert(NextToken(src).type == TOK_R_PAREN);
            
            return IR.Ret(node);
            
        } else {
            fprintf(stderr, "no such IR node: %s\n", tok.id);
            exit(1);
        }
        
    } else if (tok.type == TOK_TMP) {
        
        Node *tmp = 0;
        MAP_EACH(tmpCache, char *, key, Node *, value, {
            if (strcmp(key, tok.tmp) == 0) {
                tmp = value;
                break;
            }
        });
        
        if (tmp != 0) {
            return tmp;
        } else {
            MapPut(tmpCache, (void *) tok.tmp, IR.Tmp());
            return MapGet(tmpCache, (void *) tok.tmp);
        }
        
    } else {
        fprintf(stderr, "this token can't start a node: %s\n", TokName(tok));
        exit(1);
    }
    
    #undef ParseNode
}
    
static void ParseFunction(Token tok, IRModule *mod, Source *src) {
    
    Map *tmpCache = NewMap();
    
    assert(tok.type == TOK_ID);
    assert(strcmp(tok.id, "func") == 0);
    
    Token name = NextToken(src);
    assert(name.type == TOK_ID);
    
    IRFunction *func = IRModuleNewFunction(mod, name.id);
    Node *body = IR.Nop();
    
    while (1) {
        Token tok = NextToken(src);
        assert(tok.type == TOK_ID);
        if (strcmp(tok.id, "end") != 0) {
            body = IR.Seq(
                body,
                ParseNode(tok, src, tmpCache));
        } else {
            break;
        }
    }
    
    IRFunctionSetBody(func, body);
    DeleteMap(tmpCache);
}
    
static void ParseSource(IRModule *mod, Source *src) {
    while (1) {
        Token tok = NextToken(src);
        switch (tok.type) {
            case TOK_ID:
                if (strcmp(tok.id, "func") == 0) {
                    ParseFunction(tok, mod, src);
                } else {
                    fprintf(stderr, "was excpeting ID \"func\": %s\n", tok.id);
                    exit(1);
                }
            break;
            case TOK_EOF: return;
            default:
                fprintf(stderr, "expecting either ID or EOF, got: %s\n", TokName(tok));
                exit(1);
        }
    }
}

Frontend clare_IR_Frontend = {
    .dummy = 0,
    .SourceToIRModule = _SourceToIRModule
};
