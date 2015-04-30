#ifndef SHARED_ALGORITHM_VECTOR_H
#define SHARED_ALGORITHM_VECTOR_H

#include <stdint.h>

typedef struct {
    void** buffer;
    uint16_t size;
    uint16_t capacity;
    // If this option is set it will increase memory
    // just enough to allow all the operations.
    // It's slower but consumes no extra memory.
    uint8_t conserve_memory;
} Vector;

void make_vector(Vector* v, 
                 uint16_t initial_capacity,
                 uint8_t conserve_memory);

void destroy_vector(Vector* v);

void vector_resize(Vector* v, int new_capacity);

void vector_push_back(Vector* v, void* el);

#endif