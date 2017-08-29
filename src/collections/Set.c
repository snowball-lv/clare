#include <collections/Set.h>

#include <mem/Mem.h>
#include <collections/List.h>

struct Set {
    List *list;
};

Set *NewSet() {
    Set *set = ALLOC(Set);
    set->list = NewList();
    return set;
}

void DeleteSet(Set *set) {
    DeleteList(set->list);
    MemFree(set);
}

int SetContains(Set *set, void *value) {
    return ListIndexOf(set->list, value) >= 0;
}

void SetAdd(Set *set, void *value) {
    if (!SetContains(set, value)) {
        ListAdd(set->list, value);
    }
}

List *_SetInternalList(Set *set) {
    return set->list;
}

List *SetToList(Set *set) {
    return ListCopy(set->list);
}

Set *ListToSet(List *list) {
    Set *set = NewSet();
    LIST_EACH(list, void *, value, {
        SetAdd(set, value);
    });
    return set;
}

Set *SetCopy(Set *set) {
    Set *copy = NewSet();
    SET_EACH(set, void *, value, {
        SetAdd(copy, value);
    });
    return copy;
}

void SetRemove(Set *set, void *value) {
    int index = ListIndexOf(set->list, value);
    if (index >= 0) {
        ListRemove(set->list, index);
    }
}

int SetSize(Set *set) {
    return ListSize(set->list);
}

Set *SetSubtract(Set *a, Set *b) {
    Set *copy = SetCopy(a);
    SET_EACH(b, void *, value, {
        SetRemove(copy, value);
    });
    return copy;
}

void SetAddAll(Set *set, Set *values) {
    SET_EACH(values, void *, value, {
        if (!SetContains(set, value)) {
            SetAdd(set, value);
        }
    });
}

void SetRemoveAll(Set *set, Set *values) {
    SET_EACH(values, void *, value, {
        if (SetContains(set, value)) {
            SetRemove(set, value);
        }
    });
}
