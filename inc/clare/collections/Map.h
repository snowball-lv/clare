#pragma once

#include <clare/collections/Set.h>
#include <clare/collections/List.h>

typedef struct Map Map;

Map *NewMap();
void DeleteMap(Map *map);

void MapPut(Map *map, void *key, void *value);
void *MapGet(Map *map, void *key);
void MapRemove(Map *map, void *key);
int MapSize(Map *map);

Set *MapKeys(Map *map);

#define MAP_EACH(map, ktype, key, vtype, value, block) {    \
    Set *_keys = MapKeys(map);                              \
    SET_EACH(_keys, ktype, key, {                           \
        vtype value = MapGet(map, key);                     \
        block;                                              \
    });                                                     \
    DeleteSet(_keys);                                       \
}

Map *MapCopy(Map *map);

int MapContains(Map *map, void *key);
