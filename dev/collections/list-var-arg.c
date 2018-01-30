#include <assert.h>

#include <clare/mem/Mem.h>
#include <clare/collections/List.h>
#include <clare/helpers/Unused.h>

int main() {
    assert(MemEmpty());
    
    List *list = TO_LIST("a", "b", "c");
    
    assert(ListSize(list) == 3);
    assert(ListGet(list, 0) == (void *)"a");
    assert(ListGet(list, 1) == (void *)"b");
    assert(ListGet(list, 2) == (void *)"c");
    
    DeleteList(list);

    assert(MemEmpty());
	return 0;
}
