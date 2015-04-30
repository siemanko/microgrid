#include "vector.h"

#include <stdlib.h>

#include "shared/utils.h"

void make_vector(Vector* v, 
                 uint16_t initial_capacity,
                 uint8_t conserve_memory) {
    assert(initial_capacity >= 0);
    v->buffer = safe_malloc(sizeof(void*) * initial_capacity);
    v->capacity = initial_capacity;
    v->conserve_memory = conserve_memory;
    v->size = 0;
}

void destroy_vector(Vector* v) {
    if (v->buffer != 0) {
        free(v->buffer);
    }
}

void vector_resize(Vector* v, int new_capacity) {
    assert(new_capacity >= v->size);
    void** new_buffer = safe_malloc(sizeof(void*) * new_capacity);
    int vidx;
    for (vidx = 0; vidx < v->size; ++vidx) {
        new_buffer[vidx] = v->buffer[vidx];
    }
    free(v->buffer);
    v->buffer = new_buffer;
    v->capacity = new_capacity;
}

void vector_push_back(Vector* v, void* el) {
    if (v->size == v->capacity) {
        if (v->conserve_memory) {
            vector_resize(v, v->capacity+1);
        } else {
            vector_resize(v, (uint32_t)3*v->capacity/2 + 1);
        }
    }
    v->buffer[v->size++] = el;
}
