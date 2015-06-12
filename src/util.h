#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* definition of transition characters (TT means "transition type") */
/* placeholder, "traditional" transition, epsilon transition */
typedef enum { NONE, CHARACTER, EPSILON } Trans_type;

/* Transition from one NFA state to another */
typedef struct {
    Trans_type trans_type;  /* type of the transition */
    char trans_char;        /* transition label */
} NFA_transition;

/* state in NFA, each state has at most 2 transitions */
typedef struct NFA_state {
    struct NFA_state *to[2];        /* destination of transition */
    NFA_transition transition[2];   /* transitions from this state */
} NFA_state;

/* Non deterministic finite automata (NFA) */
typedef struct {
    NFA_state *start, *terminate; /* start & terminate state */
} NFA;              /* Ideally, there should be only one terminate state  */

/* Dump DOT code to vizualize specified NFA */
void NFA_graphviz(const char *regex, const NFA *nfa, FILE *fp);

/* The smallest building block of regexp-NFA */
NFA NFA_atomic(char c);                                 /* c   */

/* Operators in regexp to build our final NFA for the regexp. */
NFA NFA_alternate(const NFA *A, const NFA *B);  /* A|B */
NFA NFA_concat(const NFA *A, const NFA *B);     /* AB  */
NFA NFA_kleene(const NFA *A);                   /* A*  */
NFA NFA_optional(const NFA *A);                 /* A?  */
NFA NFA_positive(const NFA *A);                 /* A+  */
void NFA_dispose(NFA *nfa);                     /* Free an NFA */
int string_match(const NFA *nfa, char *str);

/* Generic linear data structure similar to std::vector in C++ */
typedef struct {
    int elem_size, length, capacity;
    /* size of each element, no. of elements in list & capacity of list */
    char *p_dat;     /* pointer to actual data */
} list;

#define INITIAL_CAPACITY  8     /* default capacity for new list */

/* Create a generic list for some kind of data elements, elem_size specified
 * the size in bytes of each data element, initial_capacity is the amount of
 * space reserved for furture use (it cannot be zero). */
void list_create(int elem_size, list *glist);

/* Free the memory allocated for the generic list */
void list_free(list *glist);

/* Append an element to the tail of specified generic list */
void list_push(list *glist, const void *elem);

/* Add an element to the list only if this element is not in the list */
int list_add(list *glist, const void *elem);