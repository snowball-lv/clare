#include <collections/StringMap.h>

#include <collections/Map.h>
#include <helpers/Unused.h>

#include <string.h>

TYPE_DEF(StringMap, {
    Map *map;
}, {
    self->map = NewMap();
}, {
    DeleteMap(self->map);
})

static const char *ResolveKey(StringMap *map, const char *key) {
    const char *rk = key;
    MAP_EACH(map->map, char *, k, void *, v, {
        UNUSED(v);
        if (strcmp(key, k) == 0) {
            rk = k;
            break;
        }
    });
    return rk;
}

void StringMapPut(StringMap *map, const char *key, void *value) {
    const char *rk = ResolveKey(map, key);
    MapPut(map->map, (void *)rk, value);
}

void *StringMapGet(StringMap *map, const char *key) {
    const char *rk = ResolveKey(map, key);
    return MapGet(map->map, (void *)rk);
}
