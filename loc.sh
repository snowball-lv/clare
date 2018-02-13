#!/usr/bin/env sh

{                               \
    find inc/ -name "*.*" &     \
    find src/ -name "*.*" &      \
    find backends/*/inc -name "*.*" &      \
    find backends/*/src -name "*.*" &      \
    find frontends/*/inc -name "*.*" &      \
    find frontends/*/src -name "*.*" &      \
} |                             \
xargs wc -l

