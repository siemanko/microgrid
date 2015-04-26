#include "circular_buffer.h"

#include <stdlib.h>

void make_cb(CircularBuffer *buffer, int size) {
    buffer->size = size;
    buffer->start = 0;
    buffer->count = 0;
    buffer->element = malloc(sizeof(buffer->element)*size);
    /* allocated array of void pointers. Same as below */
    //buffer->element = malloc(sizeof(void *) * size);

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
        index = buffer->start + buffer->count++;
        if (index >= buffer->size) {
            index = 0;
        }
        buffer->element[index] = data;
        return 1;
    }
}


void * cb_popqueue(CircularBuffer *buffer) {
    void * element;
    if (cb_empty(buffer)) {
        return 0;
    } else {
       /* FIFO implementation */
       element = buffer->element[buffer->start];
       buffer->start++;
       buffer->count--;
       if (buffer->start == buffer->size) {
           buffer->start = 0;
       }

       return element;
    }
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
