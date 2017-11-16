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
    printf("push %d\n", i32);
    INC;
    TOP.i32 = i32;
}

static void vm_add() {
    printf("add\n");
    int32_t i = TOP.i32;
    DEC;
    TOP.i32 += i;
}

static void vm_store(int index) {
    printf("store %d\n", index);
    VM.locals[index] = TOP;
    DEC;
}

static void vm_load(int index) {
    printf("load %d\n", index);
    INC;
    TOP = VM.locals[index];
}

int32_t DummyVMTop() {
    return TOP.i32;
}

void _DummyMunch(Node *root, List *patterns) {
    
    int _index = 0;
    
    #define MUNCH(node)     _DummyMunch(node, patterns);
    
    #define RULE(pattern, action) {             \
        Node *p = ListGet(patterns, _index);    \
        if (NodeMatch(root, p)) {               \
            action;                             \
            return;                             \
        }                                       \
        _index++;                               \
    }
        #include <backends/dummy/dummy.rules>
    #undef RULE
    
    #undef MUNCH
        
    printf("Trying to match node: %s\n", NodeName(root));
    assert(0 && "No rule matches node");
}

void DummyMunch(Node *root) {
    
    List *patterns = NewList();
    
    #define RULE(pattern, action)   ListAdd(patterns, pattern);
        #include <backends/dummy/dummy.rules>
    #undef RULE
    
    _DummyMunch(root, patterns);
    
    LIST_EACH(patterns, Node *, pattern, {
        NodeDeleteTree(pattern);
    });
    DeleteList(patterns);
}
