#pragma once

#include <color/RIG.h>
#include <collections/Set.h>
#include <collections/Map.h>

typedef struct Coloring Coloring;

Coloring *ColorRIG(RIG *rig, Set *colors, Map *precoloring, void *spill);
void DeleteColoring(Coloring *coloring);

void *ColoringGetColor(Coloring *coloring, void *node);
int ColoringIsColored(Coloring *coloring, void *node);
