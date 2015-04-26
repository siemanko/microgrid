/* 
 * File:   set.h
 * Author: sidor
 *
 * Created on January 22, 2015, 3:42 PM
 */

#ifndef SHARED_ALGORITHM_SET_H
#define SHARED_ALGORITHM_SET_H

#include <stdint.h>

typedef struct StructSet {
    uint8_t* contents;
    int size;
    int capacity;
} Set;

void make_set(Set * s);

// helper function that doubles set capacity.
// normally it's called automatically.
void set_increase_capacity(Set* s);

// Check whether el is in set s.
int set_in(Set* s, uint8_t el);

// Add el to set.
void set_add(Set* s, uint8_t el);

// Remove el for set. Returns true if element was found 
// and deleted.
int set_remove(Set* s, uint8_t el);

// Sets res to smallest element bigger/equal to lb.
// Returns 1 if such element is found, otherwise 0.
int set_lower_bound(Set* s, uint8_t lb, uint8_t *res);

// TODO(szymon): add this once debugger is working again.
/*
void set_print_to_computer(Set* s, int subsystem) {
    debug(subsystem, "Number of elements: %d", s->size);
    int i;
    for(i=0; i<s->size; ++i) {
        debug(subsystem, "Element %d: %d", i, s->contents[i]);
    }
}*/

#endif  /* SET_H */
