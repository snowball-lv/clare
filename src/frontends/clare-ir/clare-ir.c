#include <frontends/clare-ir/clare-ir.h>

#include <helpers/Unused.h>
#include <ir/IR.h>

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
            if (isalpha(c)) {
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
                if (isalnum(c)) {
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
                if (isalnum(c)) {
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

static Node *ParseSource(Source *src);

static IRModule *_SourceToIRModule(FILE *source_file) {
    UNUSED(source_file);
    
    IRModule *irMod = NewIRModule();
    
    // IRFunction *func = IRModuleNewFunction(irMod, "sum3");
    // 
    // Node *a = IR.Tmp();
    // Node *b = IR.Tmp();
    // Node *c = IR.Tmp();
    // 
    // Node *body = IR.Seq(
    //     IR.Seq(
    //         IR.Mov(a, IR.Arg(0)),
    //         IR.Seq(
    //             IR.Mov(b, IR.Arg(1)),
    //             IR.Mov(c, IR.Arg(2)))),
    //     IR.Ret(IR.Add(a, IR.Add(b, c))));
    // 
    // IRFunctionSetBody(func, body);
    
    Source source = { .file = source_file };
    
    // Token tok = { .type = TOK_NONE };
    // while (tok.type != TOK_EOF) {
    //     tok = NextToken(&source);
    //     printf("tok: %s", TokName(tok));
    //     switch (tok.type) {
    //         case TOK_ID: printf(", id: %s", tok.id); break;
    //         case TOK_NUM: printf(", num: %d", tok.num); break;
    //         case TOK_TMP: printf(", tmp: $%s", tok.tmp); break;
    //     }
    //     printf("\n");
    // }
    
    IRFunction *func = IRModuleNewFunction(irMod, "foo");
    Node *body = ParseSource(&source);
    IRPrintTree(body);
    IRFunctionSetBody(func, body);
    
    return irMod;
}

Node *ParseNode(Token tok, Source *src) {
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
        
        return IR.Tmp();
        
    } else {
        fprintf(stderr, "this token can't start a node: %s\n", TokName(tok));
        exit(1);
    }
}
    
static Node *ParseSource(Source *src) {
    UNUSED(src);
    Node *tree = IR.Nop();
    while (1) {
        Token tok = NextToken(src);
        switch (tok.type) {
            case TOK_ID: 
                tree = IR.Seq(
                    tree, 
                    ParseNode(tok, src));
            break;
            case TOK_EOF: goto end;
            default:
                fprintf(stderr, "expecting either ID or EOF, got: %s\n", TokName(tok));
                exit(1);
        }
    }
    end:
    return tree;
}

Frontend clare_IR_Frontend = {
    .dummy = 0,
    .SourceToIRModule = _SourceToIRModule
};
