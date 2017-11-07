#pragma once

#include <backends/Backend.h>

#define MANGLE(name) i386_ ## name

extern Backend MANGLE(Backend);
