#pragma once

typedef struct i386_R32     i386_R32;
typedef struct i386_Arg     i386_Arg;
typedef struct i386_Instr   i386_Instr;

typedef struct {
    i386_Instr *(*push)(i386_Arg *src);
    i386_Instr *(*mov)(i386_Arg *dst, i386_Arg *src);
} _i386;

extern _i386 i386;
