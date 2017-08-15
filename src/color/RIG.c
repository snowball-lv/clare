#include <color/RIG.h>

#include <mem/Mem.h>
#include <collections/Graph.h>

#include <assert.h>

struct RIG {
    Graph *edges;
};

RIG *NewRIG() {
    RIG *rig = ALLOC(RIG);
    rig->edges = NewGraph();
    return rig;
}

void DeleteRIG(RIG *rig) {
    DeleteGraph(rig->edges);
    MemFree(rig);
}

void RIGAdd(RIG *rig, void *node) {
    GraphAdd(rig->edges, node);
}

int RIGContains(RIG *rig, void *node) {
    return GraphContains(rig->edges, node);
}

void RIGRemove(RIG *rig, void *node) {
    GraphRemove(rig->edges, node);
}

RIG *RIGCopy(RIG *rig) {
    RIG *copy = NewRIG();
    DeleteGraph(copy->edges);
    copy->edges = GraphCopy(rig->edges);
    return copy;
}

void RIGConnect(RIG *rig, void *a, void *b) {
    assert(a != b);
    GraphConnect(rig->edges, a, b);
}

int RIGConnected(RIG *rig, void *a, void *b) {
    return GraphConnected(rig->edges, a, b);
}

Set *RIGNodes(RIG *rig) {
    return GraphNodes(rig->edges);
}

Set *RIGEdges(RIG *rig, void *node) {
    return GraphEdges(rig->edges, node);
}

void RIGAddAll(RIG *rig, Set *nodes) {
    SET_EACH(nodes, void *, node, {
        if (!RIGContains(rig, node)) {
            RIGAdd(rig, node);
        }
    });
}
