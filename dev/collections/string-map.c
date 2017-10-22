#include <assert.h>

#include <mem/Mem.h>
#include <collections/StringMap.h>

int main() {
    assert(MemEmpty());

    StringMap *map = NewStringMap();
    DeleteStringMap(map);

    assert(MemEmpty());
	return 0;
}
