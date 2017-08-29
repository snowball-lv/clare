#include <assert.h>
#include <stdio.h>

#include <mem/Mem.h>
#include <color/RIG.h>
#include <color/Coloring.h>
#include <collections/Set.h>
#include <collections/Map.h>

int main() {

    MemEnableRecycling();

    RIG *rig = NewRIG();
    Set *colors = NewSet();
    Map *precoloring = NewMap();

    SetAdd(colors, "red");
    SetAdd(colors, "green");
    SetAdd(colors, "blue");

    // MapPut(precoloring, "a", "red");
    // MapPut(precoloring, "b", "green");

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

    Coloring *coloring = ColorRIG(
        rig,
        colors,
        precoloring,
        "[spill]");

    SET_EACH(colors, char *, color, {
        assert(ColoringGetColor(coloring, color) == color);
    });

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

    printf("allocations: %zu\n", MemAllocations());
    printf("allocation size: %zu\n", MemAllocationSize());

	return 0;
}
