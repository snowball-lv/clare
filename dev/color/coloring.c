#include <assert.h>
#include <stdio.h>

#include <clare/mem/Mem.h>
#include <clare/color/RIG.h>
#include <clare/color/Coloring.h>
#include <clare/collections/Set.h>
#include <clare/collections/Map.h>

int main() {

    MemEnableRecycling();

    RIG *rig = NewRIG();
    Set *colors = NewSet();
    Map *precoloring = NewMap();

    SetAdd(colors, "red");
    SetAdd(colors, "green");
    SetAdd(colors, "blue");

    MapPut(precoloring, "a", "red");
    MapPut(precoloring, "b", "green");

    RIGAdd(rig, "a");
    RIGAdd(rig, "b");
    RIGAdd(rig, "c");
    RIGAdd(rig, "d");

    SET_EACH(colors, char *, color, {
        RIGAdd(rig, color);
        MapPut(precoloring, color, color);
    });

    RIGConnect(rig, "a", "d");
    RIGConnect(rig, "b", "d");
    RIGConnect(rig, "c", "d");

    RIGConnect(rig, "a", "green");
    RIGConnect(rig, "a", "blue");

    RIGConnect(rig, "b", "red");
    RIGConnect(rig, "b", "blue");

    RIGConnect(rig, "c", "red");
    RIGConnect(rig, "c", "green");
    
    RIGConnect(rig, "d", "blue");

    Coloring *coloring = ColorRIG(
        rig,
        colors,
        precoloring,
        "[spill]");

    SET_EACH(colors, char *, color, {
        assert(ColoringGetColor(coloring, color) == color);
    });

    // printf("%s -> %s\n", "a", ColoringGetColor(coloring, "a"));
    // printf("%s -> %s\n", "b", ColoringGetColor(coloring, "b"));
    // printf("%s -> %s\n", "c", ColoringGetColor(coloring, "c"));
    // printf("%s -> %s\n", "d", ColoringGetColor(coloring, "d"));

    assert(ColoringGetColor(coloring, "a") == (void *)"red");
    assert(ColoringGetColor(coloring, "b") == (void *)"green");
    assert(ColoringGetColor(coloring, "c") == (void *)"blue");
    assert(ColoringGetColor(coloring, "d") == (void *)"[spill]");

    DeleteColoring(coloring);
    DeleteMap(precoloring);
    DeleteSet(colors);
    DeleteRIG(rig);

    MemDisableRecycling();

    assert(MemEmpty());

    // printf("allocations: %zu\n", MemAllocations());
    // printf("allocation size: %zu\n", MemAllocationSize());

	return 0;
}
