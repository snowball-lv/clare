#include <assert.h>

#include <mem/Mem.h>
#include <pasm/VReg.h>

int main() {
    assert(MemEmpty());

    VReg *vReg = NewVReg();
    DeleteVReg(vReg);

    assert(MemEmpty());
	return 0;
}
