#pragma once


#include <clare/helpers/Types.h>
#include <clare/collections/List.h>
#include <clare/color/Coloring.h>

#include <stdint.h>


typedef struct PAsmVReg PAsmVReg;
struct PAsmVReg {
    int id;
    int special;
    PAsmVReg *low;
    PAsmVReg *high;
};

typedef struct {
    PAsmVReg *vreg;
    union {
        const char *str;
        int32_t i32;
        float f;
        void *ptr;
    };
} PAsmOpr;

#define PASM_OP_MAX_OPRS    4

typedef struct {
    const char *fmt;
    PAsmOpr oprs[PASM_OP_MAX_OPRS];
    PAsmVReg *use[PASM_OP_MAX_OPRS];
    PAsmVReg *def[PASM_OP_MAX_OPRS];
    
    unsigned int is_label   : 1;
    unsigned int is_jump    : 1;
    unsigned int is_ret     : 1;
    unsigned int is_cjump   : 1;
    
    int label_id;
} PAsmOp;

typedef struct {
    const char *name;
    int dummy;
    List *header;
    List *body;
    List *footer;
    int stack_space;
    Coloring *coloring;
    Map *locations;
} PAsmFunction;

HEAP_DECL(PAsmVReg)
HEAP_DECL(PAsmOp)

PAsmFunction *NewPAsmFunction();
void DeletePAsmFunction(PAsmFunction *self);
