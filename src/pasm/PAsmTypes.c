#include <clare/pasm/PAsmTypes.h>


#include <clare/mem/Mem.h>


PAsmFunction *NewPAsmFunction() {
    
    PAsmFunction *self = ALLOC(PAsmFunction);
    
    // ctor
    self->header = NewList();
    self->body = NewList();
    self->footer = NewList();

    self->stack_space = 0;

    self->coloring = 0;
    
    self->locations = NewMap();

    return self;
}

static void CollectVRegs(List *ops, Set *vregs) {
    LIST_EACH(ops, PAsmOp *, op, {
        for (int i = 0; i < PASM_OP_MAX_OPRS; i++) {
            PAsmOpr *opr = &op->oprs[i];
            if (opr->vreg != 0 && !opr->vreg->special) {
                SetAdd(vregs, opr->vreg);
            }
        }
    });
}

void DeletePAsmFunction(PAsmFunction *self) {
    
    // dtor
    Set *vregs = NewSet();
    CollectVRegs(self->header, vregs);
    CollectVRegs(self->body, vregs);
    CollectVRegs(self->footer, vregs);
    SET_EACH(vregs, PAsmVReg *, vreg, {
        MemFree(vreg);
    });
    DeleteSet(vregs);
    
    LIST_EACH(self->header, PAsmOp *, op, {
        MemFree(op);
    });
    DeleteList(self->header);
    
    LIST_EACH(self->body, PAsmOp *, op, {
        MemFree(op);
    });
    DeleteList(self->body);
    
    LIST_EACH(self->footer, PAsmOp *, op, {
        MemFree(op);
    });
    DeleteList(self->footer);
    
    if (self->coloring != 0) {
        DeleteColoring(self->coloring);
    }
    
    if (self->locations) {
        DeleteMap(self->locations);
    }
    
    MemFree(self);
}
