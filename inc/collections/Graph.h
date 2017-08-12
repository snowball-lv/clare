#pragma once

#include <collections/Set.h>

typedef struct Graph Graph;

Graph *NewGraph();
void DeleteGraph(Graph *graph);

void GraphAdd(Graph *graph, void *node);
int GraphContains(Graph *graph, void *node);
void GraphRemove(Graph *graph, void *node);

void GraphConnect(Graph *graph, void *a, void *b);
int GraphConnected(Graph *graph, void *a, void *b);
void GraphDisconnect(Graph *graph, void *a, void *b);

int GraphSize(Graph *graph);
int GraphEdgeCount(Graph *graph, void *node);

Set *GraphNodes(Graph *graph);
Set *GraphEdges(Graph *graph, void *node);

Graph *GraphCopy(Graph *graph);
