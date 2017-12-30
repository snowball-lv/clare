#include <pasm/PAsm.h>

#include <helpers/Unused.h>
#include <helpers/Error.h>
#include <collections/List.h>
#include <collections/Graph.h>
#include <collections/Map.h>
#include <color/RIG.h>
#include <color/Coloring.h>
#include <backend/Backend.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

HEAP_DEF(PAsmOp)

void PAsmFunctionSetBody(PAsmFunction *func, List *body) {
    func->body = body;
}

TYPE_DEF(PAsmModule, {
    Backend *backend;
    List *funcs;
}, {
    self->backend = 0;
    self->funcs = NewList();
}, {
    LIST_EACH(self->funcs, PAsmFunction *, func, {
        DeletePAsmFunction(func);
    });
    DeleteList(self->funcs);
})

void PAsmModuleAddFunc(PAsmModule *mod, PAsmFunction *func) {
    ListAdd(mod->funcs, func);
}

static void PAsmPrintOp(PAsmOp *op, Coloring *coloring, FILE *output) {
    
    #define printf(...)     fprintf(output, __VA_ARGS__)
    
    const char *fmt = op->fmt;
    const char *ptr = fmt;
    int opr_counter = 0;
    
    #define OPR     (&op->oprs[opr_counter])
    
    #define RULE(spec, ...) {                               \
        if (strncmp(spec, ptr, strlen(spec)) == 0) {        \
            __VA_ARGS__;                                    \
            opr_counter++;                                  \
            ptr += strlen(spec);                            \
            continue;                                       \
        }                                                   \
    }
    
    while (1) {
        
        size_t spn = strcspn(ptr, "$");
        printf("%.*s", (int) spn, ptr);
        ptr += spn;
        
        RULE("$vr", {
            if (coloring != 0 && ColoringIsColored(coloring, OPR->vreg)) {
                const char *color = ColoringGetColor(coloring, OPR->vreg);
                printf("%s", color);
            } else {
                printf("$vr%d", OPR->vreg->id);
            }
        });
        
        RULE("$id", {
            printf("$vr%d", OPR->vreg->id);
        });
        
        RULE("$i32", {
            printf("%d", OPR->i32);
        });
        
        RULE("$f", {
            printf("%f", OPR->f);
        });
        
        RULE("$str", {
            printf("%s", OPR->str);
        });
        
        RULE("$loc", {
            printf("loc($vr%d)", OPR->vreg->id);
        });
        
        RULE("$ptr", {
            printf("%p", OPR->ptr);
        });
        
        printf("%s", ptr);
        break;
    }
    
    #undef RULE
    #undef OPR
    
    #undef printf
}

HEAP_DEF(PAsmVReg)

static int _VRegIDCounter = 0;

static int NewVRegID() {
    return ++_VRegIDCounter;
}

PAsmVReg *NewPAsmVReg() {
    PAsmVReg *vreg = HEAP(PAsmVReg, {
        .id = NewVRegID()
    });
    return vreg;
}

PAsmVReg *NewSpecialPAsmVReg() {
    PAsmVReg *vreg = NewPAsmVReg();
    vreg->special = 1;
    return vreg;
}

static const char *SPILL = "[spill]";

static int _BasicBlockIDCounter = 1;

TYPE_DECL(BasicBlock)
TYPE_DEF(BasicBlock, {
    // fields
    int id;
    List *ops;
    const char *name;
    Set *live_in;
    Set *live_out;
}, {
    // ctor
    self->id = _BasicBlockIDCounter++;
    self->ops = NewList();
    self->name = 0;
    self->live_in = NewSet();
    self->live_out = NewSet();
}, {
    // dtor
    DeleteList(self->ops);
    DeleteSet(self->live_in);
    DeleteSet(self->live_out);
})

static void BasicBlockPrintName(BasicBlock *block) {
    if (block->name != 0) {
        printf("%s", block->name);
    } else {
        printf("%d", block->id);
    }
}

static List *Blockify(List *ops) {

    List *blocks = NewList();
    BasicBlock *current = 0;
    
    LIST_EACH(ops, PAsmOp *, op, {
        
        PAsmPrintOp(op, 0, stdout);
        
        if (current == 0) {
            current = NewBasicBlock();
            ListAdd(current->ops, op);
        } else {
            if (op->is_label) {
                ListAdd(blocks, current);
                current = NewBasicBlock();
                ListAdd(current->ops, op);
            } else if (op->is_jump || op->is_ret) {
                ListAdd(current->ops, op);
                ListAdd(blocks, current);
                current = 0;
            } else {
                ListAdd(current->ops, op);
            }
        }
    });
    
    if (current != 0) {
        ListAdd(blocks, current);
        current = 0;
    }

    ASSERT(current == 0);
    
    return blocks;
}

TYPE_DECL(CFG)
TYPE_DEF(CFG, {
    // fields
    int dummy;
    BasicBlock *first;
    BasicBlock *last;
    Map *inEdges;
    Map *outEdges;
    Set *blocks;
}, {
    // ctor
    self->first = NewBasicBlock();
    self->last = NewBasicBlock();
    self->inEdges = NewMap();
    self->outEdges = NewMap();
    self->blocks = NewSet();
    
    self->first->name = "first";
    self->last->name = "last";
    
    SetAdd(self->blocks, self->first);
    MapPut(self->inEdges, self->first, NewSet());
    MapPut(self->outEdges, self->first, NewSet());
    
    SetAdd(self->blocks, self->last);
    MapPut(self->inEdges, self->last, NewSet());
    MapPut(self->outEdges, self->last, NewSet());
}, {
    // dtor
    DeleteBasicBlock(self->first);
    DeleteBasicBlock(self->last);
    MAP_EACH(self->inEdges, void *, unused, Set *, set, {
        DeleteSet(set);
    });
    DeleteMap(self->inEdges);
    MAP_EACH(self->outEdges, void *, unused, Set *, set, {
        DeleteSet(set);
    });
    DeleteMap(self->outEdges);
    DeleteSet(self->blocks);
})

static void CFGAddBlock(CFG *cfg, BasicBlock *block) {
    SetAdd(cfg->blocks, block);
    if (!MapContains(cfg->inEdges, block)) {
        MapPut(cfg->inEdges, block, NewSet());
    }
    if (!MapContains(cfg->outEdges, block)) {
        MapPut(cfg->outEdges, block, NewSet());
    }
}

static void CFGConnect(CFG *cfg, BasicBlock *from, BasicBlock *to) {
    
    BasicBlockPrintName(from);
    printf(" -> ");
    BasicBlockPrintName(to);
    printf("\n");
    
    Set *out = MapGet(cfg->outEdges, from);
    SetAdd(out, to);
    
    Set *in = MapGet(cfg->inEdges, to);
    SetAdd(in, from);
}

static void PrintEdgeSet(Set *set) {
    printf("[");
    int first = 1;
    SET_EACH(set, BasicBlock *, block, {
        if (first) {
            printf(" ");
            first = 0;
        } else {
            printf(", ");
        }
        BasicBlockPrintName(block);
    });
    printf(" ]");
}

static void PrintLiveVariableSet(Set *set) {
    printf("{");
    int first = 1;
    SET_EACH(set, PAsmVReg *, vreg, {
        if (first) {
            printf(" ");
            first = 0;
        } else {
            printf(", ");
        }
        printf("$vr%d", vreg->id);
    });
    printf(" }");
}

static void CFGPrint(CFG *cfg) {
    printf("-------- cfg\n");
    SET_EACH(cfg->blocks, BasicBlock *, block, {
        
        PrintEdgeSet(MapGet(cfg->inEdges, block));
        
        printf(" -> ");
        PrintLiveVariableSet(block->live_in);
        
        printf(" ");
        BasicBlockPrintName(block);
        printf(" ");
        
        PrintLiveVariableSet(block->live_out);
        printf(" -> ");
        
        PrintEdgeSet(MapGet(cfg->outEdges, block));

        printf("\n");
    });
    printf("-------- /cfg\n");
}

static CFG *MakeCFG(List *blocks) {
    
    CFG *cfg = NewCFG();
    Map *map = NewMap();
    
    // fill lookup map & init block in/out edges
    LIST_EACH(blocks, BasicBlock *, block, {
        
        // init in/out edge set
        CFGAddBlock(cfg, block);
        
        // save in lookup map
        LIST_FIRST(block->ops, PAsmOp *, leader, {
            if (leader->is_label) {
                void *key = (void *)(intptr_t)leader->label_id;
                MapPut(map, key, block);
            }
        });
    });
    
    BasicBlock *prev = cfg->first;
    
    printf("first: %d\n", cfg->first->id);
    printf("last: %d\n", cfg->last->id);
    
    // connect the blocks
    LIST_EACH(blocks, BasicBlock *, block, {
    
        if (prev != 0) {
            // connect, prev -> block
            CFGConnect(cfg, prev, block);
        }
    
        prev = block;
    
        LIST_LAST(block->ops, PAsmOp, *op, {
            if (op->is_jump) {
                void *key = (void *)(intptr_t)op->label_id;
                BasicBlock *target = MapGet(map, key);
                
                // connect, block -> target
                CFGConnect(cfg, block, target);
                
                if (!op->is_cjump) {
                    prev = 0;
                }
            } else if (op->is_ret) {
                // connect, block -> last
                CFGConnect(cfg, block, cfg->last);
                prev = 0;
            }
        });
    });
    
    if (prev != 0) {
        // connect, prev -> last
        CFGConnect(cfg, prev, cfg->last);
        prev = 0;
    }
    
    ASSERT(prev == 0);
    
    DeleteMap(map);
    return cfg;
}

static void SolveLiveness(CFG *cfg) {
    
    int changed = 1;
    
    while (changed) {
        
        changed = 0;
        
        SET_EACH(cfg->blocks, BasicBlock *, block, {
            
            Set *old_in = SetCopy(block->live_in);
            Set *old_out = SetCopy(block->live_out);
            
            SetAddAll(block->live_in, block->live_out);
            
            LIST_REV(block->ops, PAsmOp *, op, {
            
                for (int i = 0; i < PASM_OP_MAX_OPRS; i++) {
                    PAsmVReg *vreg = op->def[i];
                    if (vreg != 0) {
                        // def'd
                        SetRemove(block->live_in, vreg);
                    }
                }
            
                for (int i = 0; i < PASM_OP_MAX_OPRS; i++) {
                    PAsmVReg *vreg = op->use[i];
                    if (vreg != 0) {
                        // used
                        SetAdd(block->live_in, vreg);
                    }
                }
            });
            
            Set *edges = MapGet(cfg->outEdges, block);
            SET_EACH(edges, BasicBlock *, edge, {
                SetAddAll(block->live_out, edge->live_in);
            });
            
            if (SetSize(old_in) != SetSize(block->live_in)) {
                changed = 1;
            }
            
            if (SetSize(old_out) != SetSize(block->live_out)) {
                changed = 1;
            }
            
            DeleteSet(old_in);
            DeleteSet(old_out);
        });
        
        CFGPrint(cfg);
    }
}

static void OutputRIGGraph(RIG *rig, FILE *dot, Map *precoloring) {
    fprintf(dot, "overlap=false\n");
    Set *nodes = RIGNodes(rig);
    SET_EACH(nodes, PAsmVReg *, a, {
        
        fprintf(dot, "%i [label=\"", a->id);
        char *color = MapGet(precoloring, a);
        if (color != 0) {
            fprintf(dot, "%s", color);
        } else {
            fprintf(dot, "$vr%i", a->id);
        }
        fprintf(dot, "\"]\n");
        
        Set *edges = RIGEdges(rig, a);
        SET_EACH(edges, PAsmVReg *, b, {
            fprintf(dot, "%i -- %i\n", a->id, b->id);
        });
        DeleteSet(edges);
    });
    DeleteSet(nodes);
}

void PAsmAllocateFunction_CFG(PAsmModule *mod, PAsmFunction *func) {
    
    printf("-------- blockify\n");
    printf("\n");
    
    List *blocks = Blockify(func->body);
    LIST_EACH(blocks, BasicBlock *, block, {
        printf("---- block %d\n", block->id);
        
        LIST_EACH(block->ops, PAsmOp *, op, {
            PAsmPrintOp(op, func->coloring, stdout);
        });
        
        printf("---- /block %d\n", block->id);
        printf("\n");
    });
    
    printf("-------- /blockify\n");
    
    CFG *cfg = MakeCFG(blocks);
    CFGPrint(cfg);
    SolveLiveness(cfg);
    DeleteCFG(cfg);
    
    // build the RIG from the liveness information
    
    RIG *rig = NewRIG();
    
    LIST_EACH(blocks, BasicBlock *, block, {
        
        Set *live = SetCopy(block->live_out);
        
        LIST_REV(block->ops, PAsmOp *, op, {
            
            for (int i = 0; i < PASM_OP_MAX_OPRS; i++) {
                PAsmVReg *vreg = op->def[i];
                if (vreg != 0) {
                    // def'd
                    RIGAdd(rig, vreg);
                    
                    // interferes with live set
                    SET_EACH(live, PAsmVReg *, l, {
                        if (vreg != l) {
                            RIGAdd(rig, l);
                            RIGConnect(rig, vreg, l);
                        }
                    });
                    
                    // interferes with other defs
                    for (int i = 0; i < PASM_OP_MAX_OPRS; i++) {
                        PAsmVReg *d = op->def[i];
                        if (d != 0 && vreg != d) {
                            RIGAdd(rig, d);
                            RIGConnect(rig, vreg, d);
                        }
                    }
                    
                    SetRemove(live, vreg);
                }
            }
            
            for (int i = 0; i < PASM_OP_MAX_OPRS; i++) {
                PAsmVReg *vreg = op->use[i];
                if (vreg != 0) {
                    // used
                    SetAdd(live, vreg);
                }
            }
            
        });
        
        // ---------------------------------------------
        
        // SET_EACH(live, PAsmVReg *, a, {
        //     SET_EACH(live, PAsmVReg *, b, {
        //         RIGAdd(rig, a);
        //         RIGAdd(rig, b);
        //         if (a != b) {
        //             RIGConnect(rig, a, b);
        //         }
        //     });
        // });
        // 
        // LIST_REV(block->ops, PAsmOp *, op, {
        // 
        //     for (int i = 0; i < PASM_OP_MAX_OPRS; i++) {
        //         PAsmVReg *vreg = op->def[i];
        //         if (vreg != 0) {
        //             // def'd
        //             SetRemove(live, vreg);
        //             RIGAdd(rig, vreg);
        //         }
        //     }
        // 
        //     for (int i = 0; i < PASM_OP_MAX_OPRS; i++) {
        //         PAsmVReg *vreg = op->use[i];
        //         if (vreg != 0) {
        //             // used
        //             SetAdd(live, vreg);
        //             RIGAdd(rig, vreg);
        //         }
        //     }
        // 
        //     SET_EACH(live, PAsmVReg *, a, {
        //         SET_EACH(live, PAsmVReg *, b, {
        //             if (a != b) {
        //                 RIGConnect(rig, a, b);
        //             }
        //         });
        //     });
        // });
        
        DeleteSet(live);
        DeleteBasicBlock(block);
    });
    
    DeleteList(blocks);
    
    Backend *backend = mod->backend;
    
    // output rig to .dot file
    char fname[128];
    sprintf(fname, "%s.rig.dot", func->name);
    FILE *dot = fopen(fname, "w");
    fprintf(dot, "strict graph %s {\n", func->name);
    
    OutputRIGGraph(rig, dot, backend->Precoloring());
    
    fprintf(dot, "}\n");
    fclose(dot);
    
    Coloring *coloring = ColorRIG(
        rig,
        backend->Colors(),
        backend->Precoloring(),
        (void *)SPILL);
    
    DeleteRIG(rig);
    
    func->coloring = coloring;
}

void PAsmAllocateFunction_Old(PAsmModule *mod, PAsmFunction *func) {
    
    List *ops = func->body;
    Set *live = NewSet();
    RIG *rig = NewRIG();
    
    LIST_REV(ops, PAsmOp *, op, {
    
        for (int i = 0; i < PASM_OP_MAX_OPRS; i++) {
            PAsmVReg *vreg = op->def[i];
            if (vreg != 0) {
                // def'd
                SetRemove(live, vreg);
                RIGAdd(rig, vreg);
            }
        }
    
        for (int i = 0; i < PASM_OP_MAX_OPRS; i++) {
            PAsmVReg *vreg = op->use[i];
            if (vreg != 0) {
                // used
                SetAdd(live, vreg);
                RIGAdd(rig, vreg);
            }
        }
    
        SET_EACH(live, PAsmVReg *, a, {
            SET_EACH(live, PAsmVReg *, b, {
                if (a != b) {
                    RIGConnect(rig, a, b);
                }
            });
        });
    });
    
    DeleteSet(live);
    
    Backend *backend = mod->backend;
    
    Coloring *coloring = ColorRIG(
        rig,
        backend->Colors(),
        backend->Precoloring(),
        (void *)SPILL);
    
    DeleteRIG(rig);
    
    func->coloring = coloring;
}

void PAsmAllocate(PAsmModule *mod) {
    LIST_EACH(mod->funcs, PAsmFunction *, func, {
        // PAsmAllocateFunction_Old(mod, func);
        PAsmAllocateFunction_CFG(mod, func);
    });
}

PAsmModule *PAsmModuleFromBackend(Backend *backend) {
    PAsmModule *mod = NewPAsmModule();
    mod->backend = backend;
    return mod;
}

static void PrintPAsmFunction(PAsmModule *mod, PAsmFunction *func, FILE *output) {
    
    Backend *backend = mod->backend;
    
    #define PAsmPrintOp(op)     PAsmPrintOp(op, func->coloring, output)
    
    LIST_EACH(func->header, PAsmOp *, op, {
        PAsmPrintOp(op);
    });
    
    List *prologue = backend->GenPrologue(func);
    LIST_EACH(prologue, PAsmOp *, op, {
        PAsmPrintOp(op);
        MemFree(op);
    });
    DeleteList(prologue);
    
    LIST_EACH(func->body, PAsmOp *, op, {
        PAsmPrintOp(op);
    });
    
    List *epilogue = backend->GenEpilogue(func);
    LIST_EACH(epilogue, PAsmOp *, op, {
        PAsmPrintOp(op);
        MemFree(op);
    });
    DeleteList(epilogue);
    
    LIST_EACH(func->footer, PAsmOp *, op, {
        PAsmPrintOp(op);
    });
    
    #undef PAsmPrintOp
}

void PrintPAsmModule(PAsmModule *mod, FILE *output) {
    // LIST_EACH(mod->ops, PAsmOp *, op, {
    //     PAsmPrintOp(mod, op, output);
    // });
    LIST_EACH(mod->funcs, PAsmFunction *, func, {
        PrintPAsmFunction(mod, func, output);
    });
}

static void SubVReg(PAsmOp *op, PAsmVReg *old, PAsmVReg *tmp) {
    for (int i = 0; i < PASM_OP_MAX_OPRS; i++) {
        PAsmOpr *opr = &op->oprs[i];
        if (opr->vreg != 0 && opr->vreg == old) {
            opr->vreg = tmp;
        }
    }
}

static int PAsmSpillFunction(PAsmModule *mod, PAsmFunction *func) {
    
    int has_spilled = 0;
    
    List *old_ops = func->body;
    func->body = NewList();
    
    Coloring *coloring = func->coloring;
    Backend *backend = mod->backend;
    
    LIST_EACH(old_ops, PAsmOp *, op, {
    
        Map *map = NewMap();
    
        for (int i = 0; i < PASM_OP_MAX_OPRS; i++) {
            
            PAsmVReg *use = op->use[i];
            if (use != 0) {
                void *color = ColoringGetColor(coloring, use);
                if (color == SPILL && !MapContains(map, use)) {
                    MapPut(map, use, NewPAsmVReg());
                    has_spilled = 1;
                }
            }
            
            PAsmVReg *def = op->def[i];
            if (def != 0) {
                void *color = ColoringGetColor(coloring, def);
                if (color == SPILL && !MapContains(map, def)) {
                    MapPut(map, def, NewPAsmVReg());
                    has_spilled = 1;
                }
            }
        }
        
        for (int i = 0; i < PASM_OP_MAX_OPRS; i++) {
            PAsmVReg *use = op->use[i];
            if (use != 0) {
                void *color = ColoringGetColor(coloring, use);
                if (color == SPILL) {
                    PAsmVReg *tmp = MapGet(map, use);
                    backend->LoadVReg(func, use, tmp);
                    op->use[i] = tmp;
                    SubVReg(op, use, tmp);
                }
            }
        }
    
        ListAdd(func->body, op);
        
        for (int i = 0; i < PASM_OP_MAX_OPRS; i++) {
            PAsmVReg *def = op->def[i];
            if (def != 0) {
                void *color = ColoringGetColor(coloring, def);
                if (color == SPILL) {
                    PAsmVReg *tmp = MapGet(map, def);
                    backend->StoreVReg(func, def, tmp);
                    op->def[i] = tmp;
                    SubVReg(op, def, tmp);
                }
            }
        }
        
        DeleteMap(map);
    });
    
    DeleteList(old_ops);
    
    return has_spilled;
}

int PAsmSpill(PAsmModule *mod) {
    
    int has_spilled = 0;
    
    LIST_EACH(mod->funcs, PAsmFunction *, func, {
        has_spilled |= PAsmSpillFunction(mod, func);
    });
    
    if (has_spilled) {
        LIST_EACH(mod->funcs, PAsmFunction *, func, {
            DeleteColoring(func->coloring);
            func->coloring = 0;
        });
    }
    
    return has_spilled;
}

static void OutputPAsmFunctionCFG(PAsmFunction *func) {
    
    List *blocks = Blockify(func->body);
    CFG *cfg = MakeCFG(blocks);
    SolveLiveness(cfg);
    
    char fname[128];
    sprintf(fname, "%s.cfg.dot", func->name);
    FILE *dot = fopen(fname, "w");
    fprintf(dot, "digraph %s {\n", func->name);
    
    Set *worklist = SetCopy(cfg->blocks);
    while (SetSize(worklist) > 0) {
        SET_ANY(worklist, BasicBlock *, from, {
            Set *edges = MapGet(cfg->outEdges, from);
            SET_EACH(edges, BasicBlock *, to, {
                
                if (from->name != 0) {
                    fprintf(dot, "%s", from->name);
                } else {
                    fprintf(dot, "%d", from->id);
                }
                
                fprintf(dot, " -> ");
                
                if (to->name != 0) {
                    fprintf(dot, "%s", to->name);
                } else {
                    fprintf(dot, "%d", to->id);
                }
                
                fprintf(dot, " [");
                fprintf(dot, " label=\"[");
                
                SET_EACH(from->live_out, PAsmVReg *, vreg, {
                    fprintf(dot, " $vr%i", vreg->id);
                });
                
                fprintf(dot, " ]\" ");
                fprintf(dot, " ]");
                
                fprintf(dot, "\n");
                
            });
            SetRemove(worklist, from);
        });
    }
    DeleteSet(worklist);
    
    SET_EACH(cfg->blocks, BasicBlock *, block, {
        
        if (block->name != 0) {
            fprintf(dot, "%s", block->name);
        } else {
            fprintf(dot, "%d", block->id);
        }
        
        fprintf(dot, " [");
        
        if (block == cfg->first) {
            fprintf(dot, " shape=Mdiamond ");
        } else if (block == cfg->last) {
            fprintf(dot, " shape=Msquare ");
        } else {
            fprintf(dot, " shape=box ");
            fprintf(dot, " label=\"");
            LIST_EACH(block->ops, PAsmOp *, op, {
                PAsmPrintOp(op, 0, dot);
                // replace \n with \l
                fseek(dot, -1, SEEK_CUR);
                fprintf(dot, "\\l");
            });
            fprintf(dot, "\"");
        }
        fprintf(dot, " ]\n");
    });
    
    fprintf(dot, "}\n");
    fclose(dot);
    
    DeleteCFG(cfg);
    LIST_EACH(blocks, BasicBlock *, block, {
        DeleteBasicBlock(block);
    });
    DeleteList(blocks);
}

void OutputPAsmModuleCFG(PAsmModule *mod) {
    LIST_EACH(mod->funcs, PAsmFunction *, func, {
        OutputPAsmFunctionCFG(func);
    });
}
