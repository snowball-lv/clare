#include <assert.h>
#include <stdio.h>

#include <mem/Mem.h>
#include <collections/Graph.h>

int main() {

    Graph *graph = NewGraph();

    GraphAdd(graph, "a");
    GraphAdd(graph, "b");
    GraphAdd(graph, "c");

    assert(GraphContains(graph, "a"));
    assert(GraphContains(graph, "b"));
    assert(GraphContains(graph, "c"));

    GraphRemove(graph, "c");
    assert(!GraphContains(graph, "c"));

    GraphConnect(graph, "a", "b");
    assert(GraphConnected(graph, "a", "b"));
    assert(!GraphConnected(graph, "a", "c"));

    GraphDisconnect(graph, "a", "b");
    assert(!GraphConnected(graph, "a", "b"));

    GraphConnect(graph, "a", "a");
    assert(GraphConnected(graph, "a", "a"));

    GraphDisconnect(graph, "a", "a");
    assert(!GraphConnected(graph, "a", "a"));

    assert(GraphSize(graph) == 2);

    GraphConnect(graph, "a", "b");
    GraphConnect(graph, "a", "a");
    assert(GraphEdgeCount(graph, "a") == 2);
    assert(GraphEdgeCount(graph, "b") == 1);

    DeleteGraph(graph);

    assert(MemEmpty());
	return 0;
}
