#include <collections/Map.h>

#include <mem/Mem.h>
#include <collections/List.h>
#include <collections/Set.h>

#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    void *key;
    void *value;
    int used;
} MapEntry;

struct Map {
    int capacity;
    int size;
    MapEntry *entries;
};

Map *NewMap() {

    Map *map = ALLOC(Map);
    map->capacity = 16;
    map->size = 0;

    size_t size = map->capacity * sizeof(MapEntry);
    map->entries = MemAlloc(size);
    memset(map->entries, 0, size);

    return map;
}

void DeleteMap(Map *map) {
    MemFree(map->entries);
    MemFree(map);
}

static int HashKey(void *key) {
    return (intptr_t) key;
}

static int KeyIndex(Map *map, void *key) {
    return HashKey(key) % map->capacity;
}

static MapEntry *GetEntry(Map *map, void *key) {
    int index = KeyIndex(map, key);
    return &map->entries[index];
}

void MapPut(Map *map, void *key, void *value) {
    MapEntry *entry = GetEntry(map, key);
    if (entry->used) {
        if (entry->key != key) {
            fprintf(stderr, "collision\n");
            exit(1);
        } else {
            entry->value = value;
        }
    } else {

        entry->key = key;
        entry->value = value;
        entry->used = 1;

        map->size++;
    }
}

void *MapGet(Map *map, void *key) {
    MapEntry *entry = GetEntry(map, key);
    if (entry->used && entry->key == key) {
        return entry->value;
    }
    return 0;
}

void MapRemove(Map *map, void *key) {
    MapEntry *entry = GetEntry(map, key);
    if (entry->used) {

        entry->key = 0;
        entry->value = 0;
        entry->used = 0;

        map->size--;
    }
}

Set *MapKeys(Map *map) {
    Set *keys = NewSet();
    for (int i = 0; i < map->capacity; i++) {
        MapEntry *entry = &map->entries[i];
        if (entry->used) {
            SetAdd(keys, entry->key);
        }
    }
    return keys;
}

int MapSize(Map *map) {
    return map->size;
}

Map *MapCopy(Map *map) {
    Map *copy = NewMap();
    MAP_EACH(map, void *, key, void *, value, {
        MapPut(copy, key, value);
    });
    return copy;
}

int MapContains(Map *map, void *key) {
    MapEntry *entry = GetEntry(map, key);
    return entry->used && entry->key == key;
}
