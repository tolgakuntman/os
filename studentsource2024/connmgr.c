/**
 * \author  Tolga Kuntman
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "config.h"
#include "lib/tcpsock.h"
#include <pthread.h>
#include "connmgr.h"
#include "sbuffer.h"

int conn_counter = 0;
int MAX_CONN;
int PORT;

/**
 * Implements a sequential test server (only one connection at the same time)
 */
void *listen_the_client(void *arg){
    arg_t *args=arg;
    tcpsock_t *client = (tcpsock_t *)args->arg1;
    sensor_data_t data;
    sbuffer_t *sbuf=(sbuffer_t *)args->arg2;
    free(args);
    int bytes, result;
    int first_data=1;
    sensor_id_t id=0;
    printf("Thread started to handle a client.\n");
    do {
        // read sensor ID
        bytes = sizeof(data.id);
        result = tcp_receive(client, (void *) &data.id, &bytes,TIMEOUT);
        if(first_data&&result==TCP_NO_ERROR){
            id = data.id;
            char con_msg[READ_MSG_LENGTH] = {[0 ... READ_MSG_LENGTH-1] = '\0'};
            sprintf(con_msg, "Sensor node %hu has opened a new connection.", id);
            write_to_log_process(con_msg);
            first_data = 0;
        }
        if(result!=TCP_NO_ERROR){
            break;
        }
        // read temperature
        bytes = sizeof(data.value);
        result = tcp_receive(client, (void *) &data.value, &bytes,TIMEOUT);
        if(result!=TCP_NO_ERROR){
            break;
        }
        // read timestamp
        bytes = sizeof(data.ts);
        result = tcp_receive(client, (void *) &data.ts, &bytes,TIMEOUT);
        if(result!=TCP_NO_ERROR){
            break;
        }
        if ((result == TCP_NO_ERROR) && bytes==sizeof(data.ts)) {
             //printf("sensor id = %" PRIu16 " - temperature = %g - timestamp = %ld\n", data.id, data.value,
               //     (long int) data.ts);
            char inserted_msg[READ_MSG_LENGTH] = {[0 ... READ_MSG_LENGTH-1] = '\0'};
            sprintf(inserted_msg, "Sensor node %hu with value: %f is inserted correctly to the buffer.", data.id, data.value);
            write_to_log_process(inserted_msg);
            sbuffer_insert(sbuf, &data);
        }
    } while (true);
    char disconnect_msg[READ_MSG_LENGTH] = {[0 ... READ_MSG_LENGTH-1] = '\0'};
    if(result==TCP_TIMEOUT){
        sprintf(disconnect_msg, "Sensor node %hu timed out.", id); 
    }else{
    sprintf(disconnect_msg, "Sensor node %hu has closed the connection.", id);
    }
    write_to_log_process(disconnect_msg);
    tcp_close(&client);
    printf("Thread exiting after serving a client.\n");
    return NULL;
}
void *wait_for_clients(void *arg) {
    tcpsock_t *server, *client;
    arg_t *args=arg;
    
    PORT =*(int *)args->arg1;
    MAX_CONN =*(int *)args->arg2;
    sbuffer_t *sbuf=args->arg3;
    pthread_t threads[MAX_CONN];
    write_to_log_process("Listening for clients.");
    if (tcp_passive_open(&server, PORT) != TCP_NO_ERROR){
        write_to_log_process("Failed to open a TCP socket to listen.");
        exit(EXIT_FAILURE);
    }
    do{
        if (tcp_wait_for_connection(server, &client) != TCP_NO_ERROR) {
                write_to_log_process("Failed to put the socket 'socket' in a blocking wait mode");
                exit(EXIT_FAILURE);
        }else {
                //printf("Client connecting...\n");
                arg_t *args_client=(arg_t *)malloc(sizeof(arg_t));
                args_client->arg1=client;
                args_client->arg2=sbuf;
                pthread_create(&threads[conn_counter], NULL, &listen_the_client, args_client);
                conn_counter++;
        }
    }while(conn_counter<MAX_CONN);
    write_to_log_process("Max number of clients joined, waiting for them to disconnect");
    for(int i=0;i<MAX_CONN;i++){
        pthread_join(threads[i], NULL);
    }
    sensor_data_t eof={0,0,0};
    sbuffer_insert(sbuf,&eof);
    if (tcp_close(&server) != TCP_NO_ERROR) {
        write_to_log_process("Socket is not closed properly.");
        exit(EXIT_FAILURE);
    }
    write_to_log_process("server is down normally");
    return NULL;
}




