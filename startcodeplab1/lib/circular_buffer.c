#include <stdlib.h>
#include "circular_buffer.h"


int circ_bbuf_init(circ_bbuf_t *cbuf, int maxlen) {
    cbuf->buffer = (double *)malloc(maxlen * sizeof(double));
    if (cbuf->buffer == NULL) {
        return -1; // Memory allocation failure
    }
    cbuf->maxlen = maxlen;
    cbuf->read_index = 0;
    cbuf->write_index = 0;
    return 0; // Success
}

void enqueue_task(circ_bbuf_t *cbuf, double task) {
    cbuf->buffer[cbuf->write_index] = task;
    cbuf->write_index = (cbuf->write_index + 1) % cbuf->maxlen;
    if (cbuf->write_index == cbuf->read_index) {
        cbuf->read_index = (cbuf->read_index + 1) % cbuf->maxlen;
    }
}
double dequeue_task(circ_bbuf_t *cbuf) {
    if (is_buffer_empty(cbuf)) {
        return 0.0;
    }
    double task = cbuf->buffer[cbuf->read_index];
    cbuf->read_index = (cbuf->read_index + 1) % cbuf->maxlen;

    return task;
}
int is_buffer_full(circ_bbuf_t *cbuf) {
    return ((cbuf->write_index +1) % cbuf->maxlen) == cbuf->read_index;
}

int is_buffer_empty(circ_bbuf_t *cbuf) {
    return cbuf->write_index == cbuf->read_index;
}

