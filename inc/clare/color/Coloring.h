#pragma once

#include <clare/color/RIG.h>
#include <clare/collections/Set.h>
#include <clare/collections/Map.h>

typedef struct Coloring Coloring;

Coloring *ColorRIG(RIG *rig, Set *colors, Map *precoloring, void *spill);
void DeleteColoring(Coloring *coloring);

void *ColoringGetColor(Coloring *coloring, void *node);
int ColoringIsColored(Coloring *coloring, void *node);
