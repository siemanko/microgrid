#include "set.h"

#include <stdlib.h>

#define INITIAL_CAPACITY 5

void make_set(Set * s) {
    s->contents = (uint8_t*)malloc(sizeof(uint8_t)*INITIAL_CAPACITY);
    s->size = 0;
    s->capacity = INITIAL_CAPACITY;
}

void set_increase_capacity(Set* s) {
    uint8_t* old_contents = s->contents;
    int new_capacity = 2*s->capacity;
    s->contents = (uint8_t*)malloc(sizeof(uint8_t)*new_capacity);
    s->capacity = new_capacity;
    int i;
    for (i=0; i<s->size; ++i) s->contents[i] = old_contents[i];
}

int set_in(Set* s, uint8_t el) {
    int i;
    for (i=0; i<s->size; ++i) {
        if (s->contents[i] == el) {
            return 1;
        }
    }
    return 0;
}

void set_add(Set* s, uint8_t el) {
    if (set_in(s, el)) return;   // DON'T READD
    // MAKE SURE ENOUGH CAPACITY
    if (s->size == s->capacity) set_increase_capacity(s);
    s->contents[s->size] = el;
    s->size++;
}

int set_remove(Set* s, uint8_t el) {
    if (!set_in(s, el)) return 0;
    int idx = 0;
    int i;
    for(i=0; i<s->size; ++i) {
        if (s->contents[i] == el) {
            idx = i;
            break;
        }
    }
    s->contents[idx] = s->contents[s->size - 1];
    s->size--;
    return 1;
}

int set_lower_bound(Set* s, uint8_t lb, uint8_t *res) {
    int found = 0;
    int i;
    for (i=0; i<s->size; ++i) {
        if (s->contents[i] >= lb) {
            if (!found) { found=1; *res=s->contents[i]; }
            if(found && s->contents[i] < *res) *res = s->contents[i];
        }
    }
    return found;
}
