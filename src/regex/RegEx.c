#include <clare/regex/RegEx.h>

#include <clare/helpers/Unused.h>
#include <clare/helpers/Error.h>
#include <stdlib.h>
#include <limits.h>
#include <clare/collections/Set.h>
#include <clare/collections/Map.h>
#include <clare/collections/List.h>
#include <clare/mem/Mem.h>

#define printf(...)

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
        TOK(KLEENE)
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
            TOK(KLEENE)
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
            
        case '*':
            tok.type = T_KLEENE;
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
    fprintf(dot, "digraph \"%s\" {\n", regex);
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
    State *last = 0;
    while (in->cur.type != T_EOF) {
        
        nfa = Concat(nfa, SimpleNFA());
        last = nfa.end;
        
        switch (in->cur.type) {
            
            case T_CHAR:
                nfa = Concat(nfa, CharNFA(in->cur.c));
                Advance(in);
                break;
                    
            case T_ANY:
                nfa = Concat(nfa, AnyNFA());
                Advance(in);
                break;
                    
            case T_L_BRACKET:
                nfa = Concat(nfa, CompileBracketExp(in));
                break;
                
            default:
                ERROR("Unhandled token: %s\n", TokTypeName(in->cur));
        }
        
        // handle kleene star
        if (in->cur.type == T_KLEENE) {
            ASSERT(last != 0);
            
            nfa = Concat(nfa, SimpleNFA());
            
            Edge *back = EmptyEdge();
            back->sym = SYM_E;
            back->target = last;
            SetAdd(nfa.end->out, back);
        
            Edge *skip = EmptyEdge();
            skip->sym = SYM_E;
            skip->target = nfa.end;
            SetAdd(last->out, skip);
        
            Advance(in);
            continue;
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

static Set *Closure(State *state, int sym);

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

static Set *Closure(State *state, int sym) {
    ASSERT(sym != SYM_FAKE);
    Set *set = NewSet();
    SET_EACH(state->out, Edge *, edge, {

        switch (sym) {
            
            case SYM_FAKE:
                break;
            
            case SYM_E:
                switch (edge->sym) {
                    case SYM_E:
                        SetAdd(set, edge->target);
                        break;
                    default:
                        break;
                }
                break;
            
            case SYM_ANY:
                switch (edge->sym) {
                    case SYM_ANY:
                        SetAdd(set, edge->target);
                        break;
                    default:
                        break;
                }
                break;
            
            default:
                switch (edge->sym) {
                    case SYM_FAKE:
                    case SYM_E:
                        break;
                    case SYM_ANY:
                        SetAdd(set, edge->target);
                        break;
                    default:
                        if (sym == edge->sym) {
                            SetAdd(set, edge->target);
                        }
                        break;
                }
                break;
        }
    });
    return set;
}

static Set *EClosure(State *state) {
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

static int IsSetFinal(Set *set) {
    int final = 0;
    SET_EACH(set, State *, state, {
        if (state->final) {
            final = 1;
            break;
        }
    });
    return final;
}

static NFA NFAToDFA(NFA nfa) {
    
    Set *S_set = EClosure(nfa.start->target);
    State *S = EmptyState();
    S->final = IsSetFinal(S_set);
    
    Map *map = NewMap();   
    MapPut(map, S_set, S);
    
    List *state_sets = NewList();
    ListAdd(state_sets, S_set); 
    
    // printf("\n");
    int i = 0;
    while (i < ListSize(state_sets)) {
        
        Set *state_set = ListGet(state_sets, i);
        State *dfa_state = GetState(map, state_set);
        ASSERT(dfa_state != 0);
        
        Set *all_edges = AllEdges(state_set);
        Set *syms = NewSet();
        
        SET_EACH(all_edges, Edge *, edge, {
            
            void *key = (void *) (intptr_t) edge->sym;
            if (SetContains(syms, key)) {
                continue;
            } else {
                SetAdd(syms, key);
            }
            
            Set *set = DFAEdge(state_set, edge->sym);
            State *dfa_target = GetState(map, set);
            
            // LogTransition(state_set, edge, set);
            
            if (dfa_target == 0) {
            
                dfa_target = EmptyState();
                dfa_target->final = IsSetFinal(set);
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
        
        DeleteSet(syms);
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

struct RegEx {
    int min, max;
    int *final;
    int *any;
    int **trans;
    int initial;
};

static RegEx *RegExFromDFA(NFA dfa) {

    Set *states = NewSet();
    Set *edges = NewSet();
    
    CollectStates(dfa.start, states, edges);
    
    printf("--- dfa\n");
    printf("states: %i\n", SetSize(states));
    printf("edges: %i\n", SetSize(edges));
    
    // remove initial kludge edge (epsilon)
    SetRemove(edges, dfa.start);
    
    // find edge range
    int min = INT_MAX;
    int max = INT_MIN;
    SET_EACH(edges, Edge *, edge, {
        int c = edge->sym;
        if (c == SYM_ANY) {
            // do nothing
        } else {
            min = c < min ? c : min;
            max = c > max ? c : max;
        }
    });
    const int range = max - min + 1;
    
    // map states to new indices
    Map *lookup = NewMap();
    int counter = 0;
    SET_EACH(states, State *, state, {
        void *value = (void *) (intptr_t) counter;
        MapPut(lookup, state, value);
        counter++;
    });
    
    const int state_count = SetSize(states);
    
    // allocate and fill final table
    int *final = malloc(state_count * sizeof(int));
    SET_EACH(states, State *, state, {
        int i = (int) (intptr_t) MapGet(lookup, state);
        final[i] = state->final;
    });
    
    // allocate and fill "any"" table
    int *any = malloc(state_count * sizeof(int));
    for (int i = 0; i < state_count; i++) {
        any[i] = -1;
    }
    
    // allocate and initialize transition table
    int **trans = malloc(state_count * sizeof(int *));
    for (int i = 0; i < state_count; i++) {
        trans[i] = malloc(range * sizeof(int));
        for (int k = 0; k < range; k++) {
            trans[i][k] = -1;
        }
    }
    
    // fill transition table with proper values
    SET_EACH(states, State *, state, {
        int from = (int) (intptr_t) MapGet(lookup, state);
        SET_EACH(state->out, Edge *, edge, {
            int to = (int) (intptr_t) MapGet(lookup, edge->target);
            int c = edge->sym;
            if (c == SYM_ANY) {
                for (int i = 0; i < range; i++) {
                    if (trans[from][i] == -1) {
                        trans[from][i] = to;
                    }
                }
                any[from] = to;
            } else {
                trans[from][c - min] = to;
            }
        });
    });
    
    int initial = (int) (intptr_t) MapGet(lookup, dfa.start->target);
    
    DeleteMap(lookup);
    DeleteSet(states);
    DeleteSet(edges);
    
    RegEx *cr = ALLOC(RegEx);
    
    cr->min     = min;
    cr->max     = max;
    cr->final   = final;
    cr->any     = any;
    cr->trans   = trans;
    cr->initial = initial;
    
    return cr;
}

RegEx *RegExCompile(const char *regex) {
    
    UNUSED(DumpNFA);
    
    printf("\n");
    printf("regex: %s\n", regex);
    
    Input in = { .regex = regex };
    NFA nfa = Compile(&in);
    nfa.end->final = 1;
    
    Set *states = NewSet();
    Set *edges = NewSet();
    
    CollectStates(nfa.start, states, edges);
    
    printf("--- nfa\n");
    printf("states: %i\n", SetSize(states));
    printf("edges: %i\n", SetSize(edges));
    
    DeleteSet(edges);
    DeleteSet(states);
    
    NFA dfa = NFAToDFA(nfa);
    DeleteNFA(nfa);
    
    RegEx *cr = RegExFromDFA(dfa);
    DeleteNFA(dfa);
    
    return cr;
}

int RegExMatchStream(RegEx *cr, FILE *input) {

    int cur = cr->initial;

    fpos_t pos;
    fgetpos(input, &pos);
    int last_final = -1;

    while (1) {
        
        if (cr->final[cur]) {
            fgetpos(input, &pos);
            last_final = cur;
        }
        
        int c = fgetc(input);
        if (c != EOF) {
            
            int next = -1;
            if (c < cr->min || cr->max < c) {
                next = cr->any[cur];
            } else {
                next = cr->trans[cur][c - cr->min];
            }
            
            if (next != -1) {
                cur = next;
            } else {
                break;
            }
            
        } else {
            break;
        }
    }

    fsetpos(input, &pos);
    return last_final != -1;
}
