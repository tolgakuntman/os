/**
 * \author {AUTHOR}
 */

#include <stdlib.h>
#include <stdio.h>
#include "sbuffer.h"
#include <pthread.h>
#include <stdbool.h>

pthread_cond_t data_available;
pthread_mutex_t buffer_lock;

/**
 * basic node for the buffer, these nodes are linked together to create the buffer
 */
typedef struct sbuffer_node {
    struct sbuffer_node *next;  /**< a pointer to the next node*/
    sensor_data_t data;         /**< a structure containing the data */
    bool accessed[2];           /**< Flags for each manager (0: Data Manager, 1: Storage Manager) */
} sbuffer_node_t;

/**
 * a structure to keep track of the buffer
 */
struct sbuffer {
    sbuffer_node_t *head;       /**< a pointer to the first node in the buffer */
    sbuffer_node_t *tail;       /**< a pointer to the last node in the buffer */
};

int sbuffer_init(sbuffer_t **buffer) {
    *buffer = malloc(sizeof(sbuffer_t));
    if (*buffer == NULL) return SBUFFER_FAILURE;
    (*buffer)->head = NULL;
    (*buffer)->tail = NULL;
    if (pthread_mutex_init(&buffer_lock, NULL) != 0) return SBUFFER_FAILURE;
    if (pthread_cond_init(&data_available, NULL) != 0) return SBUFFER_FAILURE;
    return SBUFFER_SUCCESS;
}

int sbuffer_free(sbuffer_t **buffer) {
    sbuffer_node_t *dummy;
    if ((buffer == NULL) || (*buffer == NULL)) {
        return SBUFFER_FAILURE;
    }
    pthread_mutex_lock(&buffer_lock);
    while ((*buffer)->head) {
        dummy = (*buffer)->head;
        (*buffer)->head = (*buffer)->head->next;
        free(dummy);
    }
    pthread_mutex_unlock(&buffer_lock);
    free(*buffer);
    *buffer = NULL;
    pthread_cond_destroy(&data_available);
    pthread_mutex_destroy(&buffer_lock);
    return SBUFFER_SUCCESS;
}
// int sbuffer_remove(sbuffer_t *buffer, sensor_data_t *data, int consumer_id) {
//     if (buffer == NULL || (consumer_id != 0 && consumer_id != 1)) return SBUFFER_FAILURE;

//     pthread_mutex_lock(&buffer_lock);

//     while (buffer->head == NULL) { // Wait for data if buffer is empty
//         pthread_cond_wait(&data_available, &buffer_lock);
//     }

//     sbuffer_node_t *current = buffer->head;
//     sbuffer_node_t *previous = NULL;

//     // Traverse the buffer to find the first unprocessed node for this consumer
//     while (current != NULL && current->accessed[consumer_id]) {
//         previous = current;
//         current = current->next;
//     }

//     if (current == NULL) { // No available data for this consumer
//         pthread_mutex_unlock(&buffer_lock);
//         return SBUFFER_NO_DATA;
//     }

//     // Copy the data to the output
//     *data = current->data;

//     // Check for EOF and handle shutdown signal
//     if (data->id == 0) {
//         pthread_mutex_unlock(&buffer_lock);
//         return SBUFFER_NO_DATA;
//     }

//     // Mark this node as accessed by the current consumer
//     current->accessed[consumer_id] = true;
//     if (current->accessed[0] && current->accessed[1]) {
//     if (buffer->head == buffer->tail) // buffer has only one node
//     {
//         buffer->head = buffer->tail = NULL;
//     } else  // buffer has many nodes empty
//     {
//         buffer->head = buffer->head->next;
//     }
//     }
//     free(dummy);
//     pthread_mutex_unlock(&buffer_lock);
//     return SBUFFER_SUCCESS;
// }
int sbuffer_remove(sbuffer_t *buffer, sensor_data_t *data, int consumer_id) {
    if (buffer == NULL || (consumer_id != 0 && consumer_id != 1)) return SBUFFER_FAILURE;

    pthread_mutex_lock(&buffer_lock);

    while (true) {
        sbuffer_node_t *current = buffer->head;

        // Traverse the buffer to find the first unaccessed node for this consumer
        while (current != NULL && current->accessed[consumer_id]) {
            current = current->next;
        }
        if (current != NULL) {  //If the buffer is empty skip
            *data = current->data;
            if (data->id == 0) {
                pthread_mutex_unlock(&buffer_lock);
                return SBUFFER_NO_DATA;
            }
            current->accessed[consumer_id] = true;
            if (current->accessed[0] && current->accessed[1]) { //They both start processing from the head, 
                                                                //if both accessed are true for a node, it'll be the head
                if (buffer->head == buffer->tail) // buffer has only one node
                {
                    buffer->head = buffer->tail = NULL;
                } else                            // buffer has many nodes empty
                {
                    buffer->head = buffer->head->next;
                }
                free(current);
            }
            pthread_mutex_unlock(&buffer_lock);
            return SBUFFER_SUCCESS;
        }
        // No unprocessed data available for this consumer, block
        pthread_cond_wait(&data_available, &buffer_lock);
    }
}


int sbuffer_insert(sbuffer_t *buffer, sensor_data_t *data) {
    sbuffer_node_t *dummy;
    if (buffer == NULL) return SBUFFER_FAILURE;
    dummy = malloc(sizeof(sbuffer_node_t));
    if (dummy == NULL) return SBUFFER_FAILURE;
    dummy->data = *data;
    dummy->next = NULL;
    dummy->accessed[0] = false; // Assuming two consumers: Data Manager and Storage Manager
    dummy->accessed[1] = false; // Assuming two consumers: Data Manager and Storage Manager

    pthread_mutex_lock(&buffer_lock);
    if (buffer->tail == NULL) // buffer empty (buffer->head should also be NULL
    {
        buffer->head = buffer->tail = dummy;
    } else // buffer not empty
    {
        buffer->tail->next = dummy;
        buffer->tail = buffer->tail->next;
    }
    if(data->id==0) pthread_cond_broadcast(&data_available);
    else pthread_cond_signal(&data_available);

    pthread_mutex_unlock(&buffer_lock);
    return SBUFFER_SUCCESS;
}
