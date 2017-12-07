#include <frontends/clare-ir/clare-ir.h>

#include <helpers/Unused.h>

typedef struct {
    FILE *file;
} Source;

typedef struct {
    int type;
} Token;

#define TOK(name)   TOK_ ## name
enum {
    TOK(INVALID),
    TOK(NONE),
    TOK(EOF),
};
#undef TOK

static const char *TokName(Token tok) {
    switch (tok.type) {
        #define TOK(name)   case TOK_ ## name: return #name;
        TOK(INVALID)
        TOK(NONE)
        TOK(EOF)
        #undef TOK
    }
    return "{unknown tok type}";
}

static Token NextToken(Source *src) {
    UNUSED(src);
    return (Token) { TOK_EOF };
}

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
    
    Token tok = { TOK_NONE };
    while (tok.type != TOK_EOF) {
        tok = NextToken(&source);
        printf("tok: %s\n", TokName(tok));
    }
    
    return irMod;
}

Frontend clare_IR_Frontend = {
    .dummy = 0,
    .SourceToIRModule = _SourceToIRModule
};
