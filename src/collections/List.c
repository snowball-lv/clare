#include <collections/List.h>

#include <mem/Mem.h>
#include <helpers/Unused.h>

#include <assert.h>
#include <stdarg.h>

struct List {
    int capacity;
    int size;
    void **values;
};

#define VALIDATE_INDEX(list, index) assert(index >= 0 && index < list->size)

static List *NewUninitializedList() {
    return ALLOC(List);
}

List *NewList() {
    List *list = NewUninitializedList();
    list->capacity = 4;
    list->size = 0;
    list->values = MemAlloc(list->capacity * sizeof(void *));
    return list;
}

void DeleteList(List *list) {
    MemFree(list->values);
    MemFree(list);
}

static void PrepareForSizeIncrement(List *list) {
    int nsize = list->size + 1;
    if (nsize > list->capacity) {

        list->capacity = list->capacity * 2;

        void **nvalues = MemAlloc(list->capacity * sizeof(void *));
        for (int i = 0; i < list->size; i++) {
            nvalues[i] = list->values[i];
        }

        MemFree(list->values);
        list->values = nvalues;
    }
}

void ListAdd(List *list, void *value) {
    PrepareForSizeIncrement(list);
    list->values[list->size] = value;
    list->size++;
}

static void ShiftRight(List *list) {
    PrepareForSizeIncrement(list);
    for (int i = list->size; i > 0; i--) {
        list->values[i] = list->values[i - 1];
    }
}

void ListPrepend(List *list, void *value) {
    ShiftRight(list);
    list->values[0] = value;
    list->size++;
}

int ListSize(List *list) {
    return list->size;
}

void *ListGet(List *list, int index) {
    VALIDATE_INDEX(list, index);
    return list->values[index];
}

List *ListCopy(List *list) {

    List *copy = NewUninitializedList();

    copy->capacity = list->capacity;
    copy->size = list->size;
    copy->values = MemAlloc(list->capacity * sizeof(void *));

    LIST_EACH(list, void *, value, {
        copy->values[INDEX(value)] = value;
    });

    return copy;
}

void ListSet(List *list, int index, void *value) {
    VALIDATE_INDEX(list, index);
    list->values[index] = value;
}

int ListIndexOf(List *list, void *value) {
    LIST_EACH(list, void *, v, {
        if (v == value) {
            return INDEX(v);
        }
    });
    return -1;
}

void ListRemove(List *list, int index) {
    VALIDATE_INDEX(list, index);
    for (int i = index; i < list->size; i++) {
        list->values[i] = list->values[i + 1];
    }
    list->size--;
}

int _ListInternalEndMarker;

List *_ListInternalFromVA(int marker, ...) {
    
    List *list = NewList();
    
    va_list args;
    va_start(args, marker);
    while (1) {
        void *item = va_arg(args, void *);
        if (item == &_ListInternalEndMarker) {
            break;
        } else {
            ListAdd(list, item);
        }
    }
    va_end(args);
    
    return list;
}

void ListClear(List *list) {
    list->size = 0;
}
