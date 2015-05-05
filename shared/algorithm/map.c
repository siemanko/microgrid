#include "map.h"

#include <stdlib.h>

#include "shared/utils.h"


#define INITIAL_CAPACITY 5

void make_map(Map * m) {
    m->contents = (MapElement**)safe_malloc(sizeof(MapElement*)*INITIAL_CAPACITY);
    m->size = 0;
    m->capacity = INITIAL_CAPACITY;
}

void map_increase_capacity(Map * m) {
    MapElement** old_contents = m->contents;
    int new_capacity = 2*m->capacity;
    m->contents = (MapElement**)safe_malloc(sizeof(MapElement*)*new_capacity);
    m->capacity = new_capacity;
    int i;
    for (i=0; i<m->size; ++i)
        m->contents[i] = old_contents[i];
    free(old_contents);
}

int map_index(Map* m, uint8_t key) {
    int i;
    for (i=0; i<m->size; ++i) {
        if (m->contents[i]->key == key) {
            return i;
        }
    }
    return -1;
}

void* map_get(Map* m, uint8_t key) {
    int idx = map_index(m, key);
    if (idx == -1) {
        return 0;
    } else {
        return m->contents[idx]->value;
    }
}

void map_add(Map* m, uint8_t key, void* value) {
    int idx = map_index(m, key);
    if (idx != -1) {
        free(m->contents[idx]->value);
        m->contents[idx]->value = value;
    } else {
        // MAKE SURE ENOUGH CAPACITY
        if (m->size == m->capacity) map_increase_capacity(m);
        m->contents[m->size] = (MapElement*)malloc(sizeof(MapElement));
        m->contents[m->size]->key = key;
        m->contents[m->size]->value = value;
        m->size++;
    }
}

int map_remove(Map* m, uint8_t key) {
    int idx = map_index(m, key);
    if (idx == -1)
        return 0;
    free(m->contents[idx]->value);
    free(m->contents[idx]);
    m->contents[idx] = m->contents[m->size - 1];
    m->size--;
    return 1;
}

void map_destroy(Map* m) {
    free(m->contents);
}
