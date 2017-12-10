#!/usr/bin/env sh

{                               \
    find inc/ -name "*.*" &     \
    find src/ -name "*.*";      \
} |                             \
xargs wc -l

