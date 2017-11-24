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

Coloring *Color(List *ops);

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
    Coloring *coloring = Color(ops);
    printf("------------------------\n");
    PrintColoredOps(ops, coloring);
    DeleteColoring(coloring);
    
    DeleteOps(ops);
    DeleteList(ops);
    
    NodeDeleteTree(root);
    
    // printf("vm top: %d\n", DummyVMTop());
    // assert(DummyVMTop() == 10);
    
    assert(MemEmpty());
	return 0;
}

void *GetSpill(RIG *rig, Coloring *coloring, void *spillColor) {
    void *spill = 0;
    Set *nodes = RIGNodes(rig);
    SET_EACH(nodes, void *, vreg, {
        void *color = ColoringGetColor(coloring, vreg);
        if (color == spillColor) {
            spill = vreg;
            break;
        }
    });
    DeleteSet(nodes);
    return spill;
}

Coloring *Color(List *ops) {
    UNUSED(ops);
    
    RIG *rig = NewRIG();
    Set *colors = NewSet();
    Map *precoloring = NewMap();
    
    Set *live = NewSet();
    LIST_REV(ops, void *, op, {
        
        Set *def = OpDef(op);
        SET_EACH(def, void *, vreg, {
            RIGAdd(rig, vreg);
            SetRemove(live, vreg);
        });
        DeleteSet(def);
        
        Set *use = OpUse(op);
        SET_EACH(use, void *, vreg, {
            RIGAdd(rig, vreg);
            SetAdd(live, vreg);
        });
        DeleteSet(use);
        
        printf("#");
        SET_EACH(live, void *, vreg, {
            int index = VRegIndex(vreg);
            if (index == 1) {
                // MapPut(precoloring, vreg, "eax");
            }
            printf(" %d", index);
            SET_EACH(live, void *, edge, {
                if (vreg != edge) {
                    RIGConnect(rig, vreg, edge);
                }
            });
        });
        printf("\n");
    });
    DeleteSet(live);
    
    SetAdd(colors, "eax");
    SetAdd(colors, "ecx");
    
    const char *SPILL = "[spill]";
    
    Coloring *coloring = ColorRIG(
        rig,
        colors,
        precoloring,
        (void *)SPILL);
        
    DeleteMap(precoloring);
    DeleteSet(colors);
    
    void *spill = GetSpill(rig, coloring, (void *)SPILL);
    if (spill != 0) {
        int index = VRegIndex(spill);
        printf("spill: %d\n", index);
    
        List *ops2 = Spill(ops, spill);
        ListClear(ops);
        LIST_EACH(ops2, void *, op, {
            ListAdd(ops, op);
        })
        DeleteList(ops2);
    
        DeleteRIG(rig);
        DeleteColoring(coloring);
    
        return Color(ops);
    
    } else {
        DeleteRIG(rig);
    }
    
    return coloring;
}
