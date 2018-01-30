#include <assert.h>

#include <clare/mem/Mem.h>
#include <clare/color/RIG.h>

int main() {

    RIG *rig = NewRIG();

    RIGAdd(rig, "a");
    RIGAdd(rig, "b");
    RIGAdd(rig, "c");

    assert(RIGContains(rig, "a"));
    assert(RIGContains(rig, "b"));
    assert(RIGContains(rig, "c"));

    RIGRemove(rig, "c");
    assert(!RIGContains(rig, "c"));

    RIG *copy = RIGCopy(rig);
    assert(RIGContains(copy, "a"));
    assert(RIGContains(copy, "b"));
    assert(!RIGContains(copy, "c"));

    RIGConnect(copy, "a", "b");
    assert(RIGConnected(copy, "a", "b"));
    assert(!RIGConnected(copy, "a", "c"));

    DeleteRIG(copy);

    DeleteRIG(rig);

    assert(MemEmpty());
	return 0;
}
