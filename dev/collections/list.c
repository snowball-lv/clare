#include <assert.h>

#include <mem/Mem.h>
#include <collections/List.h>

int main() {

    List *list = NewList();
    assert(ListSize(list) == 0);

    ListAdd(list, "a");
    assert(ListSize(list) == 1);

    ListAdd(list, "b");
    assert(ListSize(list) == 2);

    assert(ListGet(list, 0) == (void *)"a");
    assert(ListGet(list, 1) == (void *)"b");

    assert(ListIndexOf(list, "b") == 1);

    ListRemove(list, 0);
    assert(ListSize(list) == 1);
    assert(ListGet(list, 0) == (void *)"b");

    ListRemove(list, 0);
    assert(ListSize(list) == 0);

    ListAdd(list, "a");
    ListAdd(list, "b");
    LIST_FIRST(list, char *, value, {
        assert(value == (char *)"a");
        ListRemove(list, 0);
        LIST_FIRST(list, char *, value, {
            assert(value == (char *)"b");
            ListRemove(list, 0);
        });
    });
    assert(ListSize(list) == 0);

    DeleteList(list);

    assert(MemEmpty());
	return 0;
}
