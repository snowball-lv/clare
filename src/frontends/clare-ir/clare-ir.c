#include <frontends/clare-ir/clare-ir.h>

#include <helpers/Unused.h>

IRModule *_SourceToIRModule(FILE *source_file) {
    UNUSED(source_file);
    
    IRModule *irMod = NewIRModule();
    IRFunction *func = IRModuleNewFunction(irMod, "sum3");
    
    Node *a = IR.Tmp();
    Node *b = IR.Tmp();
    Node *c = IR.Tmp();
    
    Node *body = IR.Seq(
        IR.Seq(
            IR.Mov(a, IR.Arg(0)),
            IR.Seq(
                IR.Mov(b, IR.Arg(1)),
                IR.Mov(c, IR.Arg(2)))),
        IR.Ret(IR.Add(a, IR.Add(b, c))));

    IRFunctionSetBody(func, body);
    return irMod;
}

Frontend clare_IR_Frontend = {
    .dummy = 0,
    .SourceToIRModule = _SourceToIRModule
};
