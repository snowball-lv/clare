#pragma once

#include <helpers/Types.h>

TYPE_DECL(StringMap)

void StringMapPut(StringMap *map, const char *key, void *value);
void *StringMapGet(StringMap *map, const char *key);
