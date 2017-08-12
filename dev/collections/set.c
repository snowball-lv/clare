#include <assert.h>

#include <mem/Mem.h>
#include <collections/Set.h>

int main() {

    Set *set = NewSet();

    SetAdd(set, "a");
    SetAdd(set, "b");
    SetAdd(set, "c");

    SET_EACH(set, char *, value, {
        assert(SetContains(set, value));
    });

    assert(SetSize(set) == 3);

    SetRemove(set, "c");
    assert(!SetContains(set, "c"));

    assert(SetSize(set) == 2);

    Set *copy = SetCopy(set);
    assert(SetSize(copy) == 2);

    SetRemove(copy, "a");
    assert(SetSize(copy) == 1);

    Set *diff = SetSubtract(set, copy);
    assert(SetSize(diff) == 1);
    assert(SetContains(diff, "a"));
    assert(!SetContains(diff, "b"));
    DeleteSet(diff);

    DeleteSet(copy);

    assert(SetSize(set) == 2);
    SetRemove(set, "b");
    SET_ANY(set, char *, value, {
        assert(value == (char *)"a");
        SetRemove(set, "a");
    });
    assert(SetSize(set) == 0);

    DeleteSet(set);

    assert(MemEmpty());
	return 0;
}
