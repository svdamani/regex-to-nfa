#include "util.h"

/* Create a generic list for  some kind of data elements, elem_size is the size in
   bytes of each data element, initial_capacity is the amount of space reserved */
void list_create(int elem_size, list *glist) {
    glist->elem_size = elem_size;
    glist->capacity = INITIAL_CAPACITY;
    glist->length = 0;
    glist->p_dat = (char*)malloc(elem_size * INITIAL_CAPACITY);
}

/* Append an element to the tail of specified generic list */
void list_push(list *gl, const void *elem) {
    /* if we're running out of space */
    if (gl->capacity == gl->length) {
        gl->capacity *= 2;   /* expand two-fold */
        gl->p_dat = (char*)realloc(gl->p_dat, gl->elem_size * gl->capacity);
    }
    /* append *elem to the tail of glist */
    memcpy(gl->p_dat + gl->elem_size * gl->length++, elem, gl->elem_size);
}

/* Remove the last element in the list */
void list_pop(list *glist) {
    if (glist->length == 0) abort();  /* check for  stack underflow */
    glist->length -= 1;
}

/* Add an element to the list only if  this element is not in the list */
int list_add(list *glist, const void *elem) {
    char *cur = glist->p_dat;
    for (int i = 0; i < glist->length; i++, cur += glist->elem_size)
        if (*((int**)cur) == *((int**)elem)) return 0;   /// elem found, return
    /* elem not in the list, hence append it */
    list_push(glist, elem);
    return 1;
}

/* Free the memory for  the generic list */
void list_free(list *glist) { free(glist->p_dat); }

/* Empty the created generic list */
void list_clear(list *glist) { glist->length = 0; }