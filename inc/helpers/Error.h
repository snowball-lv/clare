#pragma once

#define ERROR(...) {                \
    fprintf(stderr, "\n");          \
    fprintf(stderr, "\x1b[31m");    \
    fprintf(stderr, __VA_ARGS__);   \
    fprintf(stderr, "%d\t%s\n", __LINE__, __FILE__);   \
    fprintf(stderr, "\x1b[39;49m"); \
    fprintf(stderr, "\n");          \
    exit(1);                        \
}

#define ASSERT(cond) { if (!(cond)) {   \
    ERROR("Failed: " #cond "\n");       \
}}

#define ASSERTM(cond, ...) { if (!(cond)) {     \
    ERROR("Failed: " #cond "\n" __VA_ARGS__);   \
}}
