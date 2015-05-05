/*
 * File:   set.h
 * Author: sidor
 *
 * Created on January 22, 2015, 3:42 PM
 */

#ifndef SHARED_ALGORITHM_MAP_H
#define SHARED_ALGORITHM_MAP_H

#include <stdint.h>

typedef struct {
    uint8_t key;
    void* value;
} MapElement;

typedef struct StructSet {
    MapElement** contents;
    int size;
    int capacity;
} Map;


// constructs a new map.
void make_map(Map* m);

// Returns index of a given key in map.
// If there is no such element in map, returns -1.
int map_index(Map* m, uint8_t el);

// Add el to set. If key already exists it updates
// the value. If that's the case old value is freed.
void map_add(Map* m, uint8_t key, void* value);

// Returns value for a given key or
// null if key does not exists.
void* map_get(Map* m, uint8_t key);

// Remove el for set. Returns true if element was found
// and deleted. It also frees a value as a side effect.
int map_remove(Map* m, uint8_t key);

// Deallocates map contents.
void map_destroy(Map* m);

#endif  /* SET_H */
