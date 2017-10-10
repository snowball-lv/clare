#include <assert.h>

#include <mem/Mem.h>
#include <collections/List.h>
#include <helpers/Unused.h>

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
