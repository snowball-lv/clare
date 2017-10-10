#pragma once

typedef struct List List;

List *NewList();
void DeleteList(List *list);

void ListAdd(List *list, void *value);
void ListPrepend(List *list, void *value);
int ListSize(List *list);
void *ListGet(List *list, int index);

#define INDEX(value) value ## _index

#define LIST_EACH(list, type, value, block) {               \
    int INDEX(value) = 0;                                   \
    for(; INDEX(value) < ListSize(list); INDEX(value)++) {  \
        type value = ListGet(list, INDEX(value));           \
        block;                                              \
    }                                                       \
}

List *ListCopy(List *list);
void ListSet(List *list, int index, void *value);
int ListIndexOf(List *list, void *value);
void ListRemove(List *list, int index);

#define LIST_FIRST(list, type, value, block) {  \
    if (ListSize(list) > 0) {                   \
        type value = ListGet(list, 0);          \
        block;                                  \
    }                                           \
}

#define LIST_LAST(list, type, value, block) {           \
    if (ListSize(list) > 0) {                           \
        type value = ListGet(list, ListSize(list) - 1); \
        block;                                          \
    }                                                   \
}

#define LIST_REV(list, type, value, block) {        \
    int INDEX(value) = ListSize(list) - 1;          \
    for(; INDEX(value) >= 0; INDEX(value)--) {      \
        type value = ListGet(list, INDEX(value));   \
        block;                                      \
    }                                               \
}

List *_ListInternalFromVA(int marker, ...);
extern int _ListInternalEndMarker; 

#define TO_LIST(...) _ListInternalFromVA(0, __VA_ARGS__, &_ListInternalEndMarker);
