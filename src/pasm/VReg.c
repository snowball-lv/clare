#include <pasm/VReg.h>

#include <mem/Mem.h>

struct VReg {
    int dummy;
};

VReg *NewVReg() {
    VReg *vReg = ALLOC(VReg);
    vReg->dummy = 0;
    return vReg;
}

void DeleteVReg(VReg *vReg) {
    MemFree(vReg);
}
