#pragma once

typedef struct {
    const char *(*version)();
    void (*DeleteInstr)(void *instr);
} IPasm;
