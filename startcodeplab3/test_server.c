/**
 * \author {AUTHOR}
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "config.h"
#include "lib/tcpsock.h"
#include <pthread.h>
int conn_counter = 0;
int MAX_CONN;
int PORT;
/**
 * Implements a sequential test server (only one connection at the same time)
 */
void *listen_the_client(void *args){
    tcpsock_t *client = (tcpsock_t *)args;
    sensor_data_t data;
    int bytes, result;
    printf("Thread started to handle a client.\n");
    do {
        // read sensor ID
        bytes = sizeof(data.id);
        result = tcp_receive(client, (void *) &data.id, &bytes);
        // read temperature
        bytes = sizeof(data.value);
        result = tcp_receive(client, (void *) &data.value, &bytes);
        // read timestamp
        bytes = sizeof(data.ts);
        result = tcp_receive(client, (void *) &data.ts, &bytes);
        if ((result == TCP_NO_ERROR) && bytes) {
             printf("sensor id = %" PRIu16 " - temperature = %g - timestamp = %ld\n", data.id, data.value,
                    (long int) data.ts);
        }
    } while (result == TCP_NO_ERROR);
   
    tcp_close(&client);
    printf("Thread exiting after serving a client.\n");
    return NULL;
}
int main(int argc, char *argv[]) {
    tcpsock_t *server, *client;
    
    if(argc < 3) {
    	printf("Please provide the right arguments: first the port, then the max nb of clients");
    	return -1;
    }
    
    MAX_CONN = atoi(argv[2]);
    PORT = atoi(argv[1]);
    pthread_t threads[MAX_CONN];
    printf("Listening for a thread.\n");
    if (tcp_passive_open(&server, PORT) != TCP_NO_ERROR){
        exit(EXIT_FAILURE);
    }
    do{
        if (tcp_wait_for_connection(server, &client) != TCP_NO_ERROR) {
                exit(EXIT_FAILURE);
            }
         else {
                printf("Client connecting...\n");
                pthread_create(&threads[conn_counter], NULL, &listen_the_client, client);
                conn_counter++;
            }
    }while(conn_counter<MAX_CONN);
    printf("all clients joined, waiting for them to terminate\n");
    for(int i=0;i<MAX_CONN;i++){
        pthread_join(threads[i], NULL);
    }
    if (tcp_close(&server) != TCP_NO_ERROR) {
        exit(EXIT_FAILURE);
    }
    printf("server is down normally\n");
    return 0;
}




