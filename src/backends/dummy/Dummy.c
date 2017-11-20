#include <backends/dummy/Dummy.h>

#include <helpers/Unused.h>
#include <collections/List.h>

#include <stdio.h>
#include <assert.h>

int dummy;

#define MAX_STACK       16
#define MAX_LOCALS      16

typedef union {
    int32_t i32;
} VMValue;

struct {
    int sp;
    VMValue stack[MAX_STACK];
    VMValue locals[MAX_LOCALS];
} VM;

#define INC     VM.sp++
#define DEC     VM.sp--
#define TOP     VM.stack[VM.sp]

static void vm_push(int32_t i32) {
    // printf("push %d\n", i32);
    INC;
    assert(VM.sp < MAX_STACK);
    TOP.i32 = i32;
}

static void vm_add() {
    // printf("add\n");
    int32_t i = TOP.i32;
    DEC;
    assert(VM.sp >= 0);
    TOP.i32 += i;
}

static void vm_store(int index) {
    assert(index < MAX_LOCALS);
    // printf("store %d\n", index);
    VM.locals[index] = TOP;
    DEC;
    assert(VM.sp >= 0);
}

static void vm_load(int index) {
    assert(index < MAX_LOCALS);
    // printf("load %d\n", index);
    INC;
    assert(VM.sp < MAX_STACK);
    TOP = VM.locals[index];
}

int32_t DummyVMTop() {
    return TOP.i32;
}

#define MANGLE(name)    Dummy ## name
#define RULE_FILE       <backends/dummy/dummy.rules>
#define RET_TYPE        void
#define RET_DEFAULT

    #include <ir/muncher.def>
    
#undef RET_DEFAULT
#undef RET_TYPE
#undef RULE_FILE
#undef MANGLE
