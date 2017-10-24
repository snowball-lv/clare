#include <i386/i386.h>

#include <helpers/Unused.h>
#include <mem/Mem.h>
#include <helpers/Strings.h>

#include <stdio.h>
#include <stdint.h>
#include <string.h>

Op *MkOp(Op op) {
    Op *ptr = ALLOC(Op);
    memcpy(ptr, &op, sizeof(Op));
    return ptr;
}

static int _Label_Counter = 1;

Label *NewLabel() {
    return ToString("_label_%d", _Label_Counter++);
}

static int _VReg_Counter = 1;

VReg *NewVReg() {
    VReg *ptr = ALLOC(VReg);
    ptr->index = _VReg_Counter++;
    return ptr;
}
