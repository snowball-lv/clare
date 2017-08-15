#include <color/Coloring.h>

#include <mem/Mem.h>
#include <collections/Set.h>
#include <color/RIG.h>

#include <assert.h>

struct Coloring {
    RIG *rig;
    Set *colors;
    Map *precoloring;
    void *spill;
    Map *coloring;
};

static void Color(Coloring *coloring);

Coloring *ColorRIG(RIG *rig, Set *colors, Map *precoloring, void *spill) {
    assert(rig != 0);
    assert(colors != 0);
    assert(precoloring != 0);
    assert(SetSize(colors) > 0);
    Coloring *coloring = ALLOC(Coloring);
    coloring->rig = RIGCopy(rig);
    coloring->colors = SetCopy(colors);
    coloring->precoloring = MapCopy(precoloring);
    coloring->spill = spill;
    coloring->coloring = NewMap();

    Color(coloring);

    return coloring;
}

void DeleteColoring(Coloring *coloring) {
    DeleteMap(coloring->coloring);
    DeleteMap(coloring->precoloring);
    DeleteSet(coloring->colors);
    DeleteRIG(coloring->rig);
    MemFree(coloring);
}

static Set *Uncolored(Coloring *coloring) {
    Set *nodes = RIGNodes(coloring->rig);
    Set *precolored = MapKeys(coloring->precoloring);
    Set *uncolored = SetSubtract(nodes, precolored);
    DeleteSet(precolored);
    DeleteSet(nodes);
    return uncolored;
}

static void *Next(Coloring *coloring) {
    void *next = 0;
    Set *uncolored = Uncolored(coloring);
    SET_EACH(uncolored, void *, node, {
        next = node;
    });
    DeleteSet(uncolored);
    return next;
}

typedef struct {
    void *node;
    Set *edges;
} Diff;

static Diff *Remove(Coloring *coloring, void *node) {
    assert(RIGContains(coloring->rig, node));
    assert(!MapContains(coloring->precoloring, node));
    Diff *diff = ALLOC(Diff);
    diff->node = node;
    diff->edges = RIGEdges(coloring->rig, node);
    RIGRemove(coloring->rig, node);
    return diff;
}

static void DeleteDiff(Diff *diff) {
    DeleteSet(diff->edges);
    MemFree(diff);
}

static void Apply(Coloring *coloring, Diff *diff) {
    assert(!RIGContains(coloring->rig, diff->node));
    RIGAdd(coloring->rig, diff->node);
    SET_EACH(diff->edges, void *, edge, {
        RIGConnect(coloring->rig, diff->node, edge);
    });
}

static void Select(Coloring *coloring, void *node) {
    assert(RIGContains(coloring->rig, node));
    assert(!MapContains(coloring->precoloring, node));
    Set *edges = RIGEdges(coloring->rig, node);

    Set *used = NewSet();
    SET_EACH(edges, void *, edge, {
        SetAdd(used, ColoringGetColor(coloring, edge));
    });

    MapPut(coloring->coloring, node, coloring->spill);

    Set *unused = SetSubtract(coloring->colors, used);
    SET_ANY(unused, void *, color, {
        MapPut(coloring->coloring, node, color);
    });

    DeleteSet(unused);
    DeleteSet(used);
    DeleteSet(edges);
}

static void Color(Coloring *coloring) {
    void *node = Next(coloring);
    if (node != 0) {

        Diff *diff = Remove(coloring, node);

        Color(coloring);

        Apply(coloring, diff);
        DeleteDiff(diff);

        Select(coloring, node);

    } else {
        // out of uncolored nodes
    }
}

void *ColoringGetColor(Coloring *coloring, void *node) {
    if (MapContains(coloring->coloring, node)) {
        return MapGet(coloring->coloring, node);
    } else {
        assert(MapContains(coloring->precoloring, node));
        return MapGet(coloring->precoloring, node);
    }
}
