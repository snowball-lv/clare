#pragma once

#define ERROR(...) {                \
    fprintf(stderr, "\x1b[31m");    \
    fprintf(stderr, __VA_ARGS__);   \
    fprintf(stderr, "\x1b[39;49m"); \
    exit(1);                        \
}
