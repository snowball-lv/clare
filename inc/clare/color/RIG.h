#pragma once

#include <clare/collections/Set.h>

typedef struct RIG RIG;

RIG *NewRIG();
void DeleteRIG(RIG *rig);

void RIGAdd(RIG *rig, void *node);
int RIGContains(RIG *rig, void *node);
void RIGRemove(RIG *rig, void *node);

RIG *RIGCopy(RIG *rig);

void RIGConnect(RIG *rig, void *a, void *b);
int RIGConnected(RIG *rig, void *a, void *b);

Set *RIGNodes(RIG *rig);
Set *RIGEdges(RIG *rig, void *node);

int RIGEdgeCount(RIG *rig, void *node);

#define RIG_EACH(rig, type, node, block) {  \
    Set *_nodes = RIGNodes(rig);            \
    SET_EACH(_nodes, type, node, block);    \
    DeleteSet(_nodes);                      \
}

void RIGAddAll(RIG *rig, Set *nodes);
