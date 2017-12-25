#include <frontends/clare-ir/clare-ir.h>

#include <helpers/Unused.h>
#include <helpers/Error.h>
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
        const char *str;
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
    TOK(ASG),
    TOK(STR),
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
            TOK(ASG)
            TOK(STR)
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
        case '=': return (Token) { .type = TOK_ASG};
        case EOF: return (Token) { .type = TOK_EOF };
        case '$': tok = TOK_TMP; break;
        default: {
            if (isalpha(c) || c == '_') {
                PUSH(c);
                tok = TOK_ID;
            } else if (isdigit(c) || c == '-') {
                PUSH(c);
                tok = TOK_NUM;
            } else if (c == '"') {
                tok = TOK_STR;
            } else {
                fprintf(stderr, "illegal character: %c\n", (char) c);
                exit(1);
            }
        }
    }
    
    while (1) {
        c = fgetc(src->file);
        switch (tok) {
            
            case TOK_STR: {
                if (c == '"') {
                    return (Token) {
                        .type = TOK_STR,
                        .str = strcpy(malloc(strlen(buffer) + 1), buffer)
                    };
                } else if (c == EOF) {
                    GERROR;
                } else {
                    PUSH(c);
                }
                break;
            }
            
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

static Node *ParseStm(Token tok, Source *src, Map *tmpCache);

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
            
            Token tok_index = NextToken(src);
            assert(tok_index.type == TOK_NUM);
            
            assert(NextToken(src).type == TOK_COMMA);
            
            Token tok_type = NextToken(src);
            assert(tok_type.type == TOK_ID);
            int data_type = IR_DT_Invalid;
            
            if (strcmp(tok_type.id, "i32") == 0) {
                data_type = IR_DT_I32;
            } else if (strcmp(tok_type.id, "float") == 0) {
                data_type = IR_DT_Float;
            } else {
                ERROR("Unknown data type: %s\n", tok_type.id);
            }
            
            assert(NextToken(src).type == TOK_R_PAREN);
            
            return IR.Arg(tok_index.num, data_type);
            
        } else if (strcmp(tok.id, "i32") == 0) {
            
            assert(NextToken(src).type == TOK_L_PAREN);
            Token tok = NextToken(src);
            assert(tok.type == TOK_NUM);
            assert(NextToken(src).type == TOK_R_PAREN);
            
            return IR.I32(tok.num);
            
        } else if (strcmp(tok.id, "str") == 0) {
            
            assert(NextToken(src).type == TOK_L_PAREN);
            Token tok = NextToken(src);
            assert(tok.type == TOK_STR);
            assert(NextToken(src).type == TOK_R_PAREN);
            
            return IR.Str(tok.str);
            
        } else if (strcmp(tok.id, "if") == 0) {
            
            Node *cond = ParseNode(NextToken(src), src);
            
            Token tok_then = NextToken(src);
            assert(tok_then.type == TOK_ID);
            assert(strcmp(tok_then.id, "then") == 0);
            
            Node *t = IR.Nop();
            
            while (1) {
                Token tok = NextToken(src);
                assert(tok.type == TOK_ID || tok.type == TOK_TMP);
                if (strcmp(tok.id, "else") != 0) {
                    t = IR.Seq(
                        t,
                        ParseStm(tok, src, tmpCache));
                } else {
                    break;
                }
            }
            
            Node *f = IR.Nop();
            
            while (1) {
                Token tok = NextToken(src);
                assert(tok.type == TOK_ID || tok.type == TOK_TMP);
                if (strcmp(tok.id, "end") != 0) {
                    f = IR.Seq(
                        f,
                        ParseStm(tok, src, tmpCache));
                } else {
                    break;
                }
            }
            
            return IR.Branch(cond, t, f);
            
        } else if (strcmp(tok.id, "while") == 0) {
            
            Node *cond = ParseNode(NextToken(src), src);
            
            Token tok_then = NextToken(src);
            assert(tok_then.type == TOK_ID);
            assert(strcmp(tok_then.id, "do") == 0);
            
            Node *t = IR.Nop();
            
            while (1) {
                Token tok = NextToken(src);
                assert(tok.type == TOK_ID || tok.type == TOK_TMP);
                if (strcmp(tok.id, "end") != 0) {
                    t = IR.Seq(
                        t,
                        ParseStm(tok, src, tmpCache));
                } else {
                    break;
                }
            }
            
            return IR.Loop(cond, t);
            
        } else if (strcmp(tok.id, "float") == 0) {
            
            assert(NextToken(src).type == TOK_L_PAREN);
            Token tok = NextToken(src);
            assert(tok.type == TOK_NUM);
            assert(NextToken(src).type == TOK_R_PAREN);
            
            return IR.Float(tok.num);
            
        } else if (strcmp(tok.id, "add") == 0) {
            
            assert(NextToken(src).type == TOK_L_PAREN);
            Node *left = ParseNode(NextToken(src), src);
            assert(NextToken(src).type == TOK_COMMA);
            Node *right = ParseNode(NextToken(src), src);
            assert(NextToken(src).type == TOK_R_PAREN);
            
            return IR.Add(left, right);
            
        } else if (strcmp(tok.id, "eq") == 0) {
            
            assert(NextToken(src).type == TOK_L_PAREN);
            Node *left = ParseNode(NextToken(src), src);
            assert(NextToken(src).type == TOK_COMMA);
            Node *right = ParseNode(NextToken(src), src);
            assert(NextToken(src).type == TOK_R_PAREN);
            
            return IR.Eq(left, right);
            
        } else if (strcmp(tok.id, "lt") == 0) {
            
            assert(NextToken(src).type == TOK_L_PAREN);
            Node *left = ParseNode(NextToken(src), src);
            assert(NextToken(src).type == TOK_COMMA);
            Node *right = ParseNode(NextToken(src), src);
            assert(NextToken(src).type == TOK_R_PAREN);
            
            return IR.Lt(left, right);
            
        } else if (strcmp(tok.id, "ret") == 0) {
            
            assert(NextToken(src).type == TOK_L_PAREN);
            Node *node = ParseNode(NextToken(src), src);
            assert(NextToken(src).type == TOK_R_PAREN);
            
            return IR.Ret(node);
            
        } else if (strcmp(tok.id, "ftd") == 0) {
            
            assert(NextToken(src).type == TOK_L_PAREN);
            Node *node = ParseNode(NextToken(src), src);
            assert(NextToken(src).type == TOK_R_PAREN);
            
            return IR.Ftd(node);
            
        } else if (strcmp(tok.id, "itf") == 0) {
            
            assert(NextToken(src).type == TOK_L_PAREN);
            Node *node = ParseNode(NextToken(src), src);
            assert(NextToken(src).type == TOK_R_PAREN);
            
            return IR.Itf(node);
            
        } else if (strcmp(tok.id, "branch") == 0) {
            
            assert(NextToken(src).type == TOK_L_PAREN);
            Node *cond = ParseNode(NextToken(src), src);
            assert(NextToken(src).type == TOK_COMMA);
            Node *t = ParseNode(NextToken(src), src);
            assert(NextToken(src).type == TOK_COMMA);
            Node *f = ParseNode(NextToken(src), src);
            assert(NextToken(src).type == TOK_R_PAREN);
            
            return IR.Branch(cond, t, f);
            
        } else if (strcmp(tok.id, "call") == 0) {
            
            assert(NextToken(src).type == TOK_L_PAREN);
            
            Token tok_type = NextToken(src);
            assert(tok_type.type == TOK_ID);
            int data_type = IR_DT_Invalid;
            
            if (strcmp(tok_type.id, "i32") == 0) {
                data_type = IR_DT_I32;
            } else if (strcmp(tok_type.id, "float") == 0) {
                data_type = IR_DT_Float;
            } else {
                ERROR("Unknown data type: %s\n", tok_type.id);
            }
            
            assert(NextToken(src).type == TOK_COMMA);
            
            Token fname = NextToken(src);
            assert(fname.type == TOK_ID);
            
            List *args = NewList();
            
            while (1) {
                Token tok = NextToken(src);
                if (tok.type == TOK_COMMA) {
                    
                    Token id_tok = NextToken(src);
                    assert(id_tok.type == TOK_ID || id_tok.type == TOK_TMP);
                    Node *arg = ParseNode(id_tok, src);
                    ListAdd(args, arg);
                        
                } else if (tok.type == TOK_R_PAREN) {
                    break;
                } else {
                    printf("expected R_PAREN or COMMA: %s\n", TokName(tok));
                    exit(1);
                }
            }
            
            return IR.Call(data_type, fname.id, args);
            
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
            ERROR("temp. \"%s\" has not been initialized\n", tok.tmp)
            // MapPut(tmpCache, (void *) tok.tmp, IR.Tmp());
            // return MapGet(tmpCache, (void *) tok.tmp);
        }
        
    } else {
        fprintf(stderr, "this token can't start a node: %s\n", TokName(tok));
        exit(1);
    }
    
    #undef ParseNode
}

static Node *ParseStm(Token tok, Source *src, Map *tmpCache) {
    switch (tok.type) {
        
        case TOK_TMP: {
            
            assert(NextToken(src).type == TOK_ASG);
            Node *value = ParseNode(NextToken(src), src, tmpCache);
            
            Node *tmp = 0;
            MAP_EACH(tmpCache, char *, key, Node *, value, {
                if (strcmp(key, tok.tmp) == 0) {
                    tmp = value;
                    break;
                }
            });
            
            if (tmp == 0) {
                MapPut(tmpCache, (void *) tok.tmp, IR.Tmp(value->data_type));
                tmp = MapGet(tmpCache, (void *) tok.tmp);
            }
            
            return IR.Mov(tmp, value);
        }

        default:
            return ParseNode(tok, src, tmpCache);
    }
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
        assert(tok.type == TOK_ID || tok.type == TOK_TMP);
        if (tok.type == TOK_TMP) {
            body = IR.Seq(
                body,
                ParseStm(tok, src, tmpCache));
        } else if (strcmp(tok.id, "end") != 0) {
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
