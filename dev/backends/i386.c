#include <assert.h>

#include <mem/Mem.h>
#include <helpers/Types.h>
#include <ir/IR.h>
#include <color/Coloring.h>
#include <color/RIG.h>
#include <collections/Set.h>
#include <collections/Map.h>
#include <collections/List.h>
#include <helpers/Unused.h>

#include <backends/dummy/Dummy.h>
#include <backends/i386/i386.h>

#include <stdio.h>

void Color(List *ops);

int main() {
    assert(MemEmpty());
    
    Node *a = Nodes.Tmp();
    Node *b = Nodes.Tmp();
    
    Node *root = Nodes.Seq(
        Nodes.Mov(a, Nodes.Add(
            Nodes.I32(1),
            Nodes.I32(2))),
        Nodes.Seq(
            Nodes.Mov(b, Nodes.Add(
                Nodes.I32(3),
                Nodes.I32(4))),
            Nodes.Mov(Nodes.Tmp(), Nodes.Add(
                a,
                b))));
    
    // DummyMunch(root);
    List *ops = i386Munch(root);
    PrintOps(ops);
    printf("------------------------\n");
    Color(ops);
    
    DeleteOps(ops);
    DeleteList(ops);
    
    NodeDeleteTree(root);
    
    // printf("vm top: %d\n", DummyVMTop());
    // assert(DummyVMTop() == 10);
    
    assert(MemEmpty());
	return 0;
}

void Color(List *ops) {
    UNUSED(ops);
    
    RIG *rig = NewRIG();
    Set *colors = NewSet();
    Map *precoloring = NewMap();
    
    Set *live = NewSet();
    LIST_REV(ops, void *, op, {
        
        Set *def = OpDef(op);
        SET_EACH(def, void *, vreg, {
            SetRemove(live, vreg);
        });
        DeleteSet(def);
        
        Set *use = OpUse(op);
        SET_EACH(use, void *, vreg, {
            SetAdd(live, vreg);
        });
        DeleteSet(use);
        
        printf("#");
        SET_EACH(live, void *, vreg, {
            int index = VRegIndex(vreg);
            printf(" %d", index);
            RIGAdd(rig, vreg);
            SET_EACH(live, void *, edge, {
                if (vreg != edge) {
                    RIGAdd(rig, edge);
                    RIGConnect(rig, vreg, edge);
                }
            });
        });
        printf("\n");
    });
    DeleteSet(live);
    
    SetAdd(colors, "eax");
    
    Coloring *coloring = ColorRIG(
        rig,
        colors,
        precoloring,
        "[spill]");
        
    DeleteColoring(coloring);
    DeleteMap(precoloring);
    DeleteSet(colors);
    DeleteRIG(rig);
}
