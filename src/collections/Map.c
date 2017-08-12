#include <collections/Map.h>

#include <mem/Mem.h>
#include <collections/List.h>

#include <assert.h>

struct Map {
    List *keys;
    List *values;
};

Map *NewMap() {
    Map *map = ALLOC(Map);
    map->keys = NewList();
    map->values = NewList();
    return map;
}

void DeleteMap(Map *map) {
    DeleteList(map->keys);
    DeleteList(map->values);
    MemFree(map);
}

void MapPut(Map *map, void *key, void *value) {
    int index = ListIndexOf(map->keys, key);
    if (index >= 0) {
        ListSet(map->values, index, value);
    } else {
        ListAdd(map->keys, key);
        ListAdd(map->values, value);
    }
}

void *MapGet(Map *map, void *key) {
    int index = ListIndexOf(map->keys, key);
    if (index >= 0) {
        return ListGet(map->values, index);
    }
    return 0;
}

void MapRemove(Map *map, void *key) {
    int index = ListIndexOf(map->keys, key);
    if (index >= 0) {
        ListRemove(map->keys, index);
        ListRemove(map->values, index);
    }
}

Set *MapKeys(Map *map) {
    return ListToSet(map->keys);
}

int MapSize(Map *map) {
    assert(ListSize(map->keys) == ListSize(map->values));
    return ListSize(map->keys);
}

Map *MapCopy(Map *map) {
    Map *copy = NewMap();
    MAP_EACH(map, void *, key, void *, value, {
        MapPut(copy, key, value);
    });
    return copy;
}

int MapContains(Map *map, void *key) {
    return ListIndexOf(map->keys, key) >= 0;
}
