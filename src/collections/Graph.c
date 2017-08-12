#include <collections/Graph.h>

#include <mem/Mem.h>
#include <collections/Map.h>
#include <collections/Set.h>

#include <assert.h>

struct Graph {
    Map *map;
};

Graph *NewGraph() {
    Graph *graph = ALLOC(Graph);
    graph->map = NewMap();
    return graph;
}

void DeleteGraph(Graph *graph) {
    MAP_EACH(graph->map, void *, node, Set *, edges, {
        DeleteSet(edges);
    });
    DeleteMap(graph->map);
    MemFree(graph);
}

void GraphAdd(Graph *graph, void *node) {
    if (!GraphContains(graph, node)) {
        MapPut(graph->map, node, NewSet());
    }
}

int GraphContains(Graph *graph, void *node) {
    return MapGet(graph->map, node) != 0;
}

void GraphRemove(Graph *graph, void *node) {
    if (GraphContains(graph, node)) {
        Set *edges = MapGet(graph->map, node);
        SET_EACH(edges, void *, edge, {
            GraphDisconnect(graph, node, edge);
        });
        DeleteSet(edges);
        MapRemove(graph->map, node);
    }
}

void GraphConnect(Graph *graph, void *a, void *b) {
    assert(GraphContains(graph, a));
    assert(GraphContains(graph, b));
    SetAdd(MapGet(graph->map, a), b);
    SetAdd(MapGet(graph->map, b), a);
}

int GraphConnected(Graph *graph, void *a, void *b) {
    if (GraphContains(graph, a)) {
        return SetContains(MapGet(graph->map, a), b);
    }
    return 0;
}

void GraphDisconnect(Graph *graph, void *a, void *b) {
    assert(GraphContains(graph, a));
    assert(GraphContains(graph, b));
    SetRemove(MapGet(graph->map, a), b);
    SetRemove(MapGet(graph->map, b), a);
}

int GraphSize(Graph *graph) {
    return MapSize(graph->map);
}

Set *GraphNodes(Graph *graph) {
    return MapKeys(graph->map);
}

Set *GraphEdges(Graph *graph, void *node) {
    assert(GraphContains(graph, node));
    return SetCopy(MapGet(graph->map, node));
}

int GraphEdgeCount(Graph *graph, void *node) {
    assert(GraphContains(graph, node));
    Set *edges = MapGet(graph->map, node);
    return SetSize(edges);
}

Graph *GraphCopy(Graph *graph) {
    Graph *copy = NewGraph();
    MAP_EACH(graph->map, void *, node, Set *, edges, {
        MapPut(copy->map, node, SetCopy(edges));
    });
    return copy;
}
