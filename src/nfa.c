#include "util.h"

/* Create a new isolated NFA state, there's no transitions going out of it */
NFA_state* alloc_state() {
    NFA_state *state = malloc(sizeof(NFA_state));
    NFA_transition null_t = {NONE, 0};  /// create an isolated NFA state node
    state->to[0] = state->to[1] = NULL;
    state->transition[0] = state->transition[1] = null_t;
    return state;
}

/* Free allocated space for  specif ied NFA state */
void free_NFA_state(NFA_state *state) { free(state); }

/* get number of transitions going out from specif ied NFA state */
int NFA_trans_num(const NFA_state *state) {
    if (state->transition[1].trans_type != NONE) return 2;
    if (state->transition[0].trans_type != NONE) return 1;
    return 0;
}

/* Add another transition to specif ied NFA state and return accordingly */
int NFA_add_trans(NFA_state *from, Trans_type type, char trans, NFA_state *to) {
    int i = NFA_trans_num(from);
    if (i >= 2)  return -1;      /* no empty slot avaliable */
    from->transition[i].trans_type = type;
    from->transition[i].trans_char = trans;
    from->to[i] = to;
    return 0;
}

/* Add an epsilon transition from "from" to "to */
int NFA_epsilon(NFA_state *from, NFA_state *to) {
    return NFA_add_trans(from, EPSILON, 0, to);
}

/* Create an NFA for  recognizing single character */
NFA NFA_atomic(char c) {
    NFA nfa = {alloc_state(), alloc_state()};
    if (c == '\0') exit(-1);
    NFA_add_trans(nfa.start, CHARACTER, c, nfa.terminate);
    return nfa;
}

/* C = AB */
NFA NFA_concat(const NFA *A, const NFA *B) {
    NFA C = {A->start, B->terminate};
    NFA_epsilon(A->terminate, B->start);
    return C;
}

/* C = A|B */
NFA NFA_alternate(const NFA *A, const NFA *B) {
    NFA C = {alloc_state(), alloc_state()};
    NFA_epsilon(C.start, A->start);
    NFA_epsilon(C.start, B->start);
    NFA_epsilon(A->terminate, C.terminate);
    NFA_epsilon(B->terminate, C.terminate);
    return C;
}

/* C = A? = A|epsilon */
NFA NFA_optional(const NFA *A) {
    NFA C = {alloc_state(), A->terminate};
    NFA_epsilon(C.start, A->start);
    NFA_epsilon(C.start, A->terminate);
    return C;
}

/* C = A* */
NFA NFA_kleene(const NFA *A) {
    NFA C = {alloc_state(), alloc_state()};
    NFA_epsilon(A->terminate, C.start);
    NFA_epsilon(C.start, A->start);
    NFA_epsilon(C.start, C.terminate);
    return C;
}

/* C = A+ = AA* */
NFA NFA_positive(const NFA *A) {
    NFA C = {alloc_state(), alloc_state()};
    NFA_epsilon(C.start, A->start);
    NFA_epsilon(A->terminate, C.start);
    NFA_epsilon(A->terminate, C.terminate);
    return C;
}

/* Traverse the NFA while recording addresses of all states in generic list */
void NFA_traverse(NFA_state *state, list *visited) {
    for (int i = 0; i < NFA_trans_num(state); ++i)       /* DFS of graphs */
        if (list_add(visited, &state->to[i]) != 0)
            NFA_traverse(state->to[i], visited);
}

/* Free an NFA */
void NFA_dispose(NFA *nfa) {
    list visited;
    NFA_state **cur = (NFA_state**)visited.p_dat;
    /* traverse the NFA and record all states in a generic list */
    list_create(sizeof(NFA_state*), &visited);
    list_push(&visited, &nfa->start);
    NFA_traverse(nfa->start, &visited);
    /* free all states */
    for (int i = 0; i < visited.length; i++, cur++) free_NFA_state(*cur);
    list_free(&visited);
}

/* Dump the transition from state to state->to[i_to] */
void NFA_transition_graphviz(const NFA_state *state, int i_to, FILE *fp) {
    switch(state->transition[i_to].trans_type) {
    case EPSILON:       /// Epsilon in unicode
        fprintf(fp, "    addr_%p -> addr_%p [ label = \"\u03B5\" ];\n",
            (void*)state, (void*)state->to[i_to]);
        break;
    case CHARACTER:
        fprintf(fp, "    addr_%p -> addr_%p [ label = \"%c\" ];\n",
            (void*)state, (void*)state->to[i_to],
            state->transition[i_to].trans_char);
        break;
    default: abort();   /// Will never reach here
    }
}

/* Dump the transitions to all reachable states from specif ied state */
void NFA_reachable_graphviz(const NFA_state *state, list *visited, FILE *fp) {
    for (int i = 0; i < NFA_trans_num(state); ++i) {
        NFA_transition_graphviz(state, i, fp);  /* dump this transition */
        /* dump the transition target if  it has not ever been dumped */
        if (list_add(visited, &state->to[i]) != 0)
            NFA_reachable_graphviz(state->to[i], visited, fp);
    }
}

/* Dump GraphViz DOT code to visualize specif ied NFA */
void NFA_graphviz(const char *regex, const NFA *nfa, FILE *fp) {
    list visited_state;
    list_create(sizeof(NFA_state*), &visited_state);
    fprintf(fp, "digraph finite_state_machine {\n"
        "    rankdir=LR;\n    size=\"8,5\"\n"
        "    labelloc=\"b\";\n    label=\"Regex : %s\";"
        "    node [shape = doublecircle label=\"\"]; addr_%p\n"
        "    node [shape = circle]\n", regex, (void*)nfa->terminate);
    /* dump the finite state machine recursively */
    list_push(&visited_state, &nfa->start);
    NFA_reachable_graphviz(nfa->start, &visited_state, fp);
    fprintf(fp, "    node [shape = none label=\"\"]; start\n");/* start mark */
    fprintf(fp, "    start -> addr_%p [ label = \"start\" ]\n}\n",
        (void*)nfa->start);
    list_free(&visited_state);  /* destroy the created list */
}

/* Standard goes_to function for  an NFA */
void NFA_delta(const NFA_state *state, const char c, list *glist) {
    for (int i = 0; i < NFA_trans_num(state); ++i) {
        if (state->transition[i].trans_type == EPSILON)
            list_add(glist, state->to[i]);
        if (state->transition[i].trans_char == c)
            list_add(glist, state->to[i]);
    }
}

/* Matching string in an NFA */
int string_match(const NFA *nfa, char *str) {
    list *new = malloc(sizeof(list));
    list_create(sizeof(NFA_state*), new);
    NFA_state *state = nfa->start;
    for (int i = 0; str[i]; ++i) {
        NFA_delta(state, str[i], new);
    }
    char *cur = new->p_dat;
    for (int i = 0; i < new->length; i++, cur += new->elem_size)
        if ((NFA_state*)cur == nfa->terminate) return 1;
    return 0;
}