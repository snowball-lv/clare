#pragma once

#include <collections/List.h>

typedef struct Set Set;

Set *NewSet();
void DeleteSet(Set *set);

void SetAdd(Set *set, void *value);
int SetContains(Set *set, void *value);
List *SetToList(Set *set);
void SetRemove(Set *set, void *value);

#define SET_EACH(set, type, value, block) { \
    List *_copy = SetToList(set);           \
    LIST_EACH(_copy, type, value, block);   \
    DeleteList(_copy);                      \
}

Set *ListToSet(List *list);
Set *SetCopy(Set *set);

int SetSize(Set *set);

Set *SetSubtract(Set *a, Set *b);

#define SET_ANY(set, type, value, block) {  \
    List *_list = SetToList(set);           \
    LIST_FIRST(_list, type, value, block);  \
    DeleteList(_list);                      \
}

void SetAddAll(Set *set, Set *values);
void SetRemoveAll(Set *set, Set *values);
