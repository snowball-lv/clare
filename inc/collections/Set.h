#pragma once

#include <collections/List.h>

typedef struct Set Set;

Set *NewSet();
void DeleteSet(Set *set);

void SetAdd(Set *set, void *value);
int SetContains(Set *set, void *value);
List *SetToList(Set *set);
void SetRemove(Set *set, void *value);

List *_SetInternalList(Set *set);

#define SET_EACH(set, type, value, block) { \
    List *_copy = _SetInternalList(set);    \
    LIST_EACH(_copy, type, value, block);   \
}

Set *ListToSet(List *list);
Set *SetCopy(Set *set);

int SetSize(Set *set);

Set *SetSubtract(Set *a, Set *b);

#define SET_ANY(set, type, value, block) {  \
    List *_list = _SetInternalList(set);    \
    LIST_FIRST(_list, type, value, block);  \
}

void SetAddAll(Set *set, Set *values);
void SetRemoveAll(Set *set, Set *values);
