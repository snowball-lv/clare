#include <regex/RegEx.h>

#include <helpers/Unused.h>
#include <helpers/Error.h>
#include <stdlib.h>
#include <limits.h>
#include <collections/Set.h>
#include <collections/Map.h>
#include <collections/List.h>
#include <mem/Mem.h>


enum {
    #define TOK(name)   T_ ## name,
        TOK(INVALID)
        TOK(NONE)
        TOK(EOF)
        TOK(CHAR)
        TOK(L_BRACKET)
        TOK(HYPHEN)
        TOK(R_BRACKET)
        TOK(ANY)
    #undef TOK
};

typedef struct {
    int type;
    char c;
} Token;

const char *TokTypeName(Token tok) {
    switch (tok.type) {
        #define TOK(name)   case T_ ## name: return "T_" #name;
            TOK(INVALID)
            TOK(NONE)
            TOK(EOF)
            TOK(CHAR)
            TOK(L_BRACKET)
            TOK(HYPHEN)
            TOK(R_BRACKET)
            TOK(ANY)
        #undef TOK
        default:
            ERROR("Unknown token: %i\n", tok.type);
    }
}

typedef struct {
    const char *regex;
    Token cur;
} Input;

static Token NextToken(Input *in) {
    Token tok = { 0 };
    switch (*in->regex) {
        
        case 0:
            tok.type = T_EOF;
            break;
            
        case '[':
            tok.type = T_L_BRACKET;
            in->regex += 1;
            break;
            
        case '-':
            tok.type = T_HYPHEN;
            in->regex += 1;
            break;
            
        case ']':
            tok.type = T_R_BRACKET;
            in->regex += 1;
            break;
            
        case '.':
            tok.type = T_ANY;
            in->regex += 1;
            break;
        
        default:
            tok.type = T_CHAR;
            tok.c = *in->regex;
            in->regex += 1;
            break;
    }
    return tok;
}

static void Advance(Input *in) {
    in->cur = NextToken(in);
}

typedef struct {
    int id;
    int final;
    Set *out;
} State;

static int _StateIDCounter = 1;

static State *EmptyState() {
    State *state = ALLOC(State);
    state->out = NewSet();
    state->id = _StateIDCounter++;
    state->final = 0;
    return state;
}

#define SYM_E       (INT_MAX - 1)
#define SYM_ANY     (INT_MAX - 2)
#define SYM_FAKE    (INT_MAX - 3)

typedef struct {
    int sym;
    State *target;
} Edge;

static Edge *EmptyEdge() {
    Edge *edge = ALLOC(Edge);
    edge->sym = 0;
    edge->target = 0;
    return edge;
}

typedef struct {
    Edge *start;
    State *end;
} NFA;

static NFA SimpleNFA() {
    NFA nfa = {
        .start = EmptyEdge(),
        .end = EmptyState()
    };
    nfa.start->sym = SYM_E;
    nfa.start->target = nfa.end;
    return nfa;
}

static NFA CharNFA(char c) {
    NFA nfa = SimpleNFA();
    nfa.start->sym = c;
    return nfa;
}

static NFA AnyNFA() {
    NFA nfa = SimpleNFA();
    nfa.start->sym = SYM_ANY;
    return nfa;
}

static NFA Alternate(NFA a, NFA b) {
    NFA in = SimpleNFA();
    SetAdd(in.end->out, a.start);
    SetAdd(in.end->out, b.start);
    NFA out = SimpleNFA();
    SetAdd(a.end->out, out.start);
    SetAdd(b.end->out, out.start);
    NFA nfa = {
        .start = in.start,
        .end = out.end
    };
    return nfa;
}

static NFA RangeNFA(char from, char to) {
    ASSERT(from <= to);
    NFA in = SimpleNFA();
    NFA out = SimpleNFA();
    for (int c = from; c <= to; c++) {
        NFA nfa = CharNFA(c);
        SetAdd(in.end->out, nfa.start);
        SetAdd(nfa.end->out, out.start);
    }
    NFA nfa = {
        .start = in.start,
        .end = out.end
    };
    return nfa;
}

static NFA Concat(NFA a, NFA b) {
    SetAdd(a.end->out, b.start);
    NFA nfa = {
        .start = a.start,
        .end = b.end
    };
    return nfa;
}

static void PrinteEdge(Edge *edge, Set *printed);

static void PrintState(State *state, Set *printed) {
    if (SetContains(printed, state)) {
        return;
    }
    SetAdd(printed, state);
    SET_EACH(state->out, Edge *, edge, {
        if (edge->sym != SYM_FAKE) {
            if (state->final) {
                printf("%i (f) -> ", state->id);
            } else {
                printf("%i -> ", state->id);
            }
            PrinteEdge(edge, printed);
        }
    });
}

static void PrinteEdge(Edge *edge, Set *printed) {
    if (edge->sym == SYM_FAKE) {
        return;
    }
    switch (edge->sym) {
        case SYM_E:     printf("[epsilon]");    break;
        case SYM_ANY:   printf("[any]");        break;
        case SYM_FAKE:  printf("[fake]");       break;
        default:        printf("%c", edge->sym);
    }
    if (edge->target->final) {
        printf(" -> %i (f)\n", edge->target->id);
    } else {
        printf(" -> %i\n", edge->target->id);
    }
    PrintState(edge->target, printed);
}

static void PrintNFA(NFA nfa) {
    Set *printed = NewSet();
    PrinteEdge(nfa.start, printed);
    DeleteSet(printed);
}

static void DumpEdge(Edge *edge, FILE *dot, Set *dumped);

static void DumpState(State *state, FILE *dot, Set *dumped) {
    
    if (SetContains(dumped, state)) {
        return;
    }
    SetAdd(dumped, state);
    
    fprintf(dot, "%i [", state->id);
    
    if (state->final) {
        fprintf(dot, "shape=doublecircle");
    } else {
        fprintf(dot, "shape=circle");
    }
    
    fprintf(dot, "]\n");
    
    SET_EACH(state->out, Edge *, edge, {
        if (edge->sym != SYM_FAKE) {
            fprintf(dot, "%i", state->id);
            DumpEdge(edge, dot, dumped);
        }
    });
}

static void DumpEdge(Edge *edge, FILE *dot, Set *dumped) {
    
    if (edge->sym == SYM_FAKE) {
        return;
    }
    
    fprintf(dot, " -> %i", edge->target->id);
    
    fprintf(dot, " [ ");    
    fprintf(dot, "label=\"");
    switch (edge->sym) {
        case SYM_E:     fprintf(dot, "[E]");            break;
        case SYM_ANY:   fprintf(dot, "[Any]");          break;
        case SYM_FAKE:  fprintf(dot, "[Fake]");         break;
        default:        fprintf(dot, "%c", edge->sym);
    }
    fprintf(dot, "\"");
    fprintf(dot, " ]\n");
    
    DumpState(edge->target, dot, dumped);
}

static void DumpNFA(const char *regex, const char *fname, NFA nfa) {
    
    FILE *dot = fopen(fname, "w");
    fprintf(dot, "strict digraph \"%s\" {\n", regex);
    fprintf(dot, "overlap=false\n");
    
    Set *dumped = NewSet();
    fprintf(dot, "S [ shape=plaintext ]\n");
    fprintf(dot, "S");
    DumpEdge(nfa.start, dot, dumped);
    DeleteSet(dumped);
    
    fprintf(dot, "}\n");
    
    fclose(dot);
}

static NFA CompileBracketExp(Input *in) {
    ASSERT(in->cur.type == T_L_BRACKET);
    Advance(in);
    NFA nfa = SimpleNFA();
    nfa.start->sym = SYM_FAKE;
    while (1) {
        switch (in->cur.type) {
            
            case T_CHAR:;
                Token a = in->cur;
                Advance(in);
                if (in->cur.type == T_HYPHEN) {
                    Advance(in);
                    ASSERT(in->cur.type == T_CHAR);
                    Token b = in->cur;
                    nfa = Alternate(nfa, RangeNFA(a.c, b.c));
                    Advance(in);
                } else {
                    nfa = Alternate(nfa, CharNFA(a.c));
                }
                continue;
                    
            case T_ANY:
                nfa = Alternate(nfa, AnyNFA());
                Advance(in);
                continue;
                    
            case T_R_BRACKET:
                Advance(in);
                return nfa;
                
            default:
                ERROR("Unhandled token: %s\n", TokTypeName(in->cur));
        }
    }
}

static NFA Compile(Input *in) {
    NFA nfa = SimpleNFA();
    Advance(in);
    while (in->cur.type != T_EOF) {
        switch (in->cur.type) {
            
            case T_CHAR:
                nfa = Concat(nfa, CharNFA(in->cur.c));
                Advance(in);
                continue;
                    
            case T_ANY:
                nfa = Concat(nfa, AnyNFA());
                Advance(in);
                continue;
                    
            case T_L_BRACKET:
                nfa = Concat(nfa, CompileBracketExp(in));
                continue;
                
            default:
                ERROR("Unhandled token: %s\n", TokTypeName(in->cur));
        }
    }
    return nfa;
}

static void CollectStates(Edge *edge, Set *states, Set *edges) {
    SetAdd(edges, edge);
    State *state = edge->target;
    if (!SetContains(states, state)) {
        SetAdd(states, state);
        SET_EACH(state->out, Edge *, edge, {
            CollectStates(edge, states, edges);
        });
    }
}

static void DeleteNFA(NFA nfa) {
    Set *states = NewSet();
    Set *edges = NewSet();
    CollectStates(nfa.start, states, edges);
    SET_EACH(states, State *, state, {
        DeleteSet(state->out);
        MemFree(state);
    });
    DeleteSet(states);
    SET_EACH(edges, Edge *, edge, {
        MemFree(edge);
    });
    DeleteSet(edges);
}

Set *Closure(State *state, int sym) {
    Set *set = NewSet();
    SET_EACH(state->out, Edge *, edge, {
        if (edge->sym == sym) {
            SetAdd(set, edge->target);
        }
    });
    return set;
}

Set *EClosure(State *state) {
    Set *set = NewSet();
    SetAdd(set, state);
    while (1) {
        int old_size = SetSize(set);
        
        Set *tmp = NewSet();
        SET_EACH(set, State *, state, {
            SetAdd(tmp, state);
            Set *c = Closure(state, SYM_E);
            SetAddAll(tmp, c);
            DeleteSet(c);
        });
        
        DeleteSet(set);
        set = tmp;
        
        if (SetSize(set) == old_size) {
            break;
        }
    }
    return set;
}

void Visit(State *state, Set *visited) {
    if (SetContains(visited, state)) {
        return;
    }
    SetAdd(visited, state);
    
    Set *c = EClosure(state);
    printf("%i ->", state->id);
    SET_EACH(c, State *, s, {
        printf(" %i", s->id);
    });
    printf("\n");
    DeleteSet(c);
    
    SET_EACH(state->out, Edge *, edge, {
        if (edge->sym != SYM_FAKE) {
            Visit(edge->target, visited);
        }
    });
}

static Set *AllEdges(Set *state_set) {
    Set *edges = NewSet();
    SET_EACH(state_set, State *, state, {
        SET_EACH(state->out, Edge *, edge, {
            if (edge->sym != SYM_E && edge->sym != SYM_FAKE) {
                SetAdd(edges, edge);
            }
        });
    });
    return edges;
}

static Set *DFAEdge(Set *state_set, int sym) {
    Set *set = NewSet();
    SET_EACH(state_set, State *, state, {
        Set *c = Closure(state, sym);
        SET_EACH(c, State *, s, {
            Set *e = EClosure(s);
            SetAddAll(set, e);
            DeleteSet(e);
        });
        DeleteSet(c);
    });
    return set;
}

static State *GetState(Map *map, Set *state_set) {
    State *state = 0;
    MAP_EACH(map, Set *, set, State *, s, {
        if (SetCmp(state_set, set)) {
            state = s;
            break;
        }
    });
    return state;
}

static NFA NFAToDFA(NFA nfa) {
    
    State *S = EmptyState();
    Set *S_set = EClosure(nfa.start->target);
    
    Map *map = NewMap();   
    MapPut(map, S_set, S);
    
    List *state_sets = NewList();
    ListAdd(state_sets, S_set); 
    
    int i = 0;
    while (i < ListSize(state_sets)) {
        
        Set *state_set = ListGet(state_sets, i);
        State *dfa_state = GetState(map, state_set);
        ASSERT(dfa_state != 0);
        
        Set *all_edges = AllEdges(state_set);
        SET_EACH(all_edges, Edge *, edge, {
            
            Set *set = DFAEdge(state_set, edge->sym);
            State *dfa_target = GetState(map, set);
            
            if (dfa_target == 0) {
                dfa_target = EmptyState();
                MapPut(map, set, dfa_target);
                ListAdd(state_sets, set);
            } else {
                DeleteSet(set);
            }
            
            Edge *e = EmptyEdge();
            e->sym = edge->sym;
            e->target = dfa_target;
            SetAdd(dfa_state->out, e);
        });
        
        DeleteSet(all_edges);
        i++;
    }
    
    DeleteMap(map);
    
    LIST_EACH(state_sets, Set *, set, {
        DeleteSet(set);
    });
    DeleteList(state_sets);
    
    Edge *entry = EmptyEdge();
    entry->sym = SYM_E;
    entry->target = S;
    
    NFA dfa = {
        .start = entry
    };
    return dfa;
}

int RegExMatchStream(const char *regex, FILE *input) {
    
    UNUSED(input);
    
    printf("\n");
    printf("--- %s\n", regex);
    Input in = { .regex = regex };
    NFA nfa = Compile(&in);
    nfa.end->final = 1;
    printf("--- NFA\n");
    PrintNFA(nfa);
    
    char nfa_name[128];
    sprintf(nfa_name, "r-%s.nfa.dot", regex);
    DumpNFA(regex, nfa_name, nfa);
    
    NFA dfa = NFAToDFA(nfa);
    
    char dfa_name[128];
    sprintf(dfa_name, "r-%s.dfa.dot", regex);
    DumpNFA(regex, dfa_name, dfa);
    
    DeleteNFA(dfa);
    DeleteNFA(nfa);
    
    while (fgetc(input) != EOF);
    return 1;

    // return 0;
}
