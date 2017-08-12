#include <collections/List.h>

#include <mem/Mem.h>

#include <assert.h>

typedef struct Link Link;
struct Link {
    void *value;
    Link *next;
};

static Link *NewLink(void *value) {
    Link *link = ALLOC(Link);
    link->value = value;
    link->next = 0;
    return link;
}

static void DeleteLink(Link *link) {
    MemFree(link);
}

struct List {
    int size;
    Link *head;
    Link *tail;
};

List *NewList() {
    List *list = ALLOC(List);
    list->size = 0;
    list->head = 0;
    list->tail = 0;
    return list;
}

void DeleteList(List *list) {
    Link *curr = list->head;
    while (curr != 0) {
        Link *link = curr;
        curr = link->next;
        DeleteLink(link);
    }
    MemFree(list);
}

void ListAdd(List *list, void *value) {
    Link *link = NewLink(value);
    if (list->tail != 0) {
        list->tail->next = link;
        list->tail = list->tail->next;
    } else {
        list->head = list->tail = link;
    }
    list->size += 1;
}

int ListSize(List *list) {
    return list->size;
}

void *ListGet(List *list, int index) {
    assert(0 <= index && index < ListSize(list));
    Link *link = list->head;
    for (int i = 0; i < index; i++) {
        link = link->next;
    }
    return link->value;
}

List *ListCopy(List *list) {
    List *copy = NewList();
    LIST_EACH(list, void *, value, {
        ListAdd(copy, value);
    });
    return copy;
}

static Link *ListGetLink(List *list, int index) {
    assert(0 <= index && index < ListSize(list));
    Link *link = list->head;
    for (int i = 0; i < index; i++) {
        link = link->next;
    }
    return link;
}

void ListSet(List *list, int index, void *value) {
    assert(0 <= index && index < ListSize(list));
    Link *link = ListGetLink(list, index);
    link->value = value;
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
    assert(0 <= index && index < ListSize(list));
    Link *link = ListGetLink(list, index);
    Link *prev = index > 0 ? ListGetLink(list, index - 1) : 0;
    if (prev != 0) {
        prev->next = link->next;
    }
    if (link == list->head) {
        list->head = link->next;
    }
    if (link == list->tail) {
        list->tail = prev;
    }
    list->size -= 1;
    DeleteLink(link);
}

void ListPrepend(List *list, void *value) {
    Link *link = NewLink(value);
    if (list->head != 0) {
        link->next = list->head;
        list->head = link;
    } else {
        list->head = list->tail = link;
    }
    list->size += 1;
}
