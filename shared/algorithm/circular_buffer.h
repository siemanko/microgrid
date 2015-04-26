/* 
 * File:   circular_buffer.h
 * Author: sidor
 *
 * Created on January 18, 2015, 6:30 PM
 */

#ifndef CIRCULAR_BUFFER_H
#define	CIRCULAR_BUFFER_H

typedef struct StructCircularBuffer {
    int size;
    int start;
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
    void **element;
} CircularBuffer;


void make_cb(CircularBuffer *buffer, int size);

// Returns true is buffer is full.
int cb_full(CircularBuffer *buffer);

// Returns true if buffer is empty
int cb_empty(CircularBuffer *buffer);

// Add element to buffer.
int cb_push(CircularBuffer *buffer, void *data);

// Pop oldest element (FIFO).
void * cb_popqueue(CircularBuffer *buffer);

// Pop most recent element (FILO).
void * cb_popstack(CircularBuffer *buffer);

#endif	/* CIRCULAR_BUFFER_H */

