/* 
 * File:   circular_buffer.h
 * Author: sidor
 *
 * Created on January 18, 2015, 6:30 PM
 */

#ifndef CIRCULAR_BUFFER_H
#define	CIRCULAR_BUFFER_H

#include <stdlib.h>
#include "macros.h"

typedef struct StructCircularBuffer {
    int size;
    int start;
    //int end;  // position of last element
    /* Tracking start and end of buffer would waste
     * one position. A full buffer would always have
     * to leave last position empty or otherwise
     * it would look empty. Instead this buffer uses
     * count to track if buffer is empty or full
     */
    int count; // number of elements in buffer
    /* Two ways to make buffer element type opaque
     * First is by using typedef for the element
     * pointer. Second is by using void pointer.
     */
    /* different types of buffer:
    int *element;   // array of integers
    char *element;  // array of characters
    void *element;  // array of void type (could cast to int, char, etc)
    char **element; //array of char pointers (array of strings)
    void **element; // array of void pointers
    Choosing array of void pointers since it's the most flexible */
    void **element;
} CircularBuffer;


void make_cb(CircularBuffer *buffer, int size);

int cb_full(CircularBuffer *buffer);

int cb_empty(CircularBuffer *buffer);

int cb_push(CircularBuffer *buffer, void *data);

void * cb_popqueue(CircularBuffer *buffer);

void * cb_popstack(CircularBuffer *buffer);

#endif	/* CIRCULAR_BUFFER_H */

