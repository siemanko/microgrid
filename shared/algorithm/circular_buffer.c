#include "circular_buffer.h"

#include <stdlib.h>
#include <stdio.h>

#include "shared/utils.h"

void make_cb(CircularBuffer *buffer, uint16_t size) {
    assert(size >= 0);
    buffer->size = size;
    buffer->start = 0;
    buffer->count = 0;
    if (buffer->size >= 0) {
        buffer->element = safe_malloc(sizeof(void*)*size);
    }
    /* allocated array of void pointers. Same as below */
    //buffer->element = safe_malloc(sizeof(void *) * size);

}

int cb_full(CircularBuffer *buffer) {
    if (buffer->count == buffer->size) {
        return 1;
    } else {
        return 0;
    }
}

int cb_empty(CircularBuffer *buffer) {
    if (buffer->count == 0) {
        return 1;
    } else {
        return 0;
    }
}

int cb_push(CircularBuffer *buffer, void *data) {
    int index;
    if (cb_full(buffer)) {
        return 0;
    } else {
        index = buffer->start + buffer->count+1;
        if (index >= buffer->size) {
            index = 0;
        }
        buffer->element[index] = data;
        ++buffer->count;

        assert(buffer->count <= buffer->size);
        assert(buffer->start < buffer->size);

        return 1;
    }
}


void * cb_popqueue(CircularBuffer *buffer) {
    void * element = 0;
    if (!cb_empty(buffer)) {
       /* FIFO implementation */
       element = buffer->element[buffer->start];
       buffer->count--;
       buffer->start++;
       if (buffer->start == buffer->size) {
           buffer->start = 0;
       }
    }
    assert(buffer->count <= buffer->size);
    assert(buffer->start <= buffer->size);
    return element;
}

void * cb_popstack(CircularBuffer *buffer) {
    int index;
    if (cb_empty(buffer)) {
        return 0;
    } else {
        /* LIFO implementation */
        index = buffer->start + buffer->count - 1;
        if (index >= buffer->size) {
           index = buffer->count - buffer->size - 1;
        }
        buffer->count--;
        return buffer->element[index];
    }
}
