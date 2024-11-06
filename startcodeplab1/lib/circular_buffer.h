#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

typedef struct {
    double *buffer;      // Array of elements
    int read_index;      // Read index
    int write_index;     // Write index
    int maxlen;          // Maximum number of elements
} circ_bbuf_t;

int circ_bbuf_init(circ_bbuf_t *cbuf, int maxlen);
void enqueue_task(circ_bbuf_t *cbuf, double task);
double dequeue_task(circ_bbuf_t *cbuf);
int is_buffer_full(circ_bbuf_t *cbuf);
int is_buffer_empty(circ_bbuf_t *cbuf);


#endif