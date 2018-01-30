#include <assert.h>

#include <clare/mem/Mem.h>
#include <clare/collections/Map.h>

int main() {

    Map *map = NewMap();

    MapPut(map, "a", "1");
    MapPut(map, "b", "2");
    MapPut(map, "c", "3");
    MapPut(map, "d", "4");

    assert(MapContains(map, "a"));
    assert(MapContains(map, "b"));
    assert(MapContains(map, "c"));
    assert(MapContains(map, "d"));

    Map *copy_a = MapCopy(map);

    assert(MapGet(map, "a") == (void *) "1");
    assert(MapGet(map, "b") == (void *) "2");
    assert(MapGet(map, "c") == (void *) "3");
    assert(MapGet(map, "d") == (void *) "4");

    assert(MapSize(map) == 4);

    Set *keys_a = MapKeys(map);
    assert(SetSize(keys_a) == 4);
    DeleteSet(keys_a);

    MAP_EACH(map, void *, key, char *, value, {
        if (key == (void *) "a") {
            assert(value == (void *) "1" && "a");
        } else if (key == (void *) "b") {
            assert(value == (void *) "2" && "b");
        } else if (key == (void *) "c") {
            assert(value == (void *) "3" && "c");
        } else if (key == (void *) "d") {
            assert(value == (void *) "4" && "d");
        }
    });

    MapRemove(map, "b");
    MapRemove(map, "d");

    assert(MapGet(map, "a") == (void *) "1");
    assert(MapGet(map, "b") == (void *) 0);
    assert(MapGet(map, "c") == (void *) "3");
    assert(MapGet(map, "d") == (void *) 0);

    assert(MapContains(map, "a"));
    assert(MapContains(map, "b") == 0);
    assert(MapContains(map, "c"));
    assert(MapContains(map, "d") == 0);

    assert(MapSize(map) == 2);

    Set *keys_b = MapKeys(map);
    assert(SetSize(keys_b) == 2);
    DeleteSet(keys_b);

    MAP_EACH(map, char *, key, char *, value, {
        if (key == (void *) "a") {
            assert(value == (void *) "1" && "a");
        } else if (key == (void *) "b") {
            assert(value == 0 && "b");
        } else if (key == (void *) "c") {
            assert(value == (void *) "3" && "c");
        } else if (key == (void *) "d") {
            assert(value == 0 && "d");
        }
    });

    Map *copy_b = MapCopy(map);

    DeleteMap(map);

    assert(MapContains(copy_a, "a"));
    assert(MapContains(copy_a, "b"));
    assert(MapContains(copy_a, "c"));
    assert(MapContains(copy_a, "d"));

    DeleteMap(copy_a);

    assert(MapContains(copy_b, "a"));
    assert(MapContains(copy_b, "b") == 0);
    assert(MapContains(copy_b, "c"));
    assert(MapContains(copy_b, "d") == 0);

    DeleteMap(copy_b);

    assert(MemEmpty());
	return 0;
}
