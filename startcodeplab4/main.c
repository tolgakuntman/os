/**
 * \author tolgakuntman
 */
#include "config.h"
#include "sbuffer.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

pthread_mutex_t csv_lock;
void *writer(void *args){
    sbuffer_t *buffer=args;     //assuming that the buffer is initialized before the thread
    
    FILE * data = fopen("sensor_data", "rb");   //open the file
    printf("file opened\n");
    sensor_data_t *sensor_data=malloc(sizeof(sensor_data_t));
    while (fread(&sensor_data->id, sizeof(sensor_id_t), 1, data) == 1 &&    //check if there's data to read
           fread(&sensor_data->value, sizeof(sensor_value_t), 1, data) == 1 &&
           fread(&sensor_data->ts, sizeof(sensor_ts_t), 1, data) == 1) {

            sbuffer_insert(buffer,sensor_data);
            usleep(10000);//10 ms delay
           }
    sensor_data->id=0;  //eof
    sbuffer_insert(buffer,sensor_data);
    free(sensor_data);
    fclose(data);
    return NULL;
}

void *reader(void *args){
    sbuffer_t *buffer=args; 
    sensor_data_t *sensor_data=malloc(sizeof(sensor_data_t));
    FILE *file_csv= fopen("sensor_data_out.csv", "a");
    while (sbuffer_remove(buffer,sensor_data)!=SBUFFER_NO_DATA)
    {
        pthread_mutex_lock(&csv_lock);
        if(fprintf(file_csv,"%d,%lf,%ld\n", sensor_data->id, sensor_data->value, sensor_data->ts)<-1)  printf("I/O Error: failed to write sensor data.\n");
        //else   printf("Data insertion succeeded.\n");
        fflush(file_csv);
        pthread_mutex_unlock(&csv_lock);
        usleep(25000);//25 ms delay
    }
    free(sensor_data);
    fclose(file_csv);
    return NULL;
}

int main(){
    struct timespec ts, te;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    pthread_t writer_thread;
    pthread_t reader_thread1, reader_thread2,reader_thread3,reader_thread4;
    sbuffer_t *buf;
    sbuffer_init(&buf);
    printf("buffer initialized correctly\n");
    pthread_mutex_init(&csv_lock, NULL);
    printf("mutex initialized correctly\n");
    pthread_create( &writer_thread, NULL, &writer, buf);
    pthread_create( &reader_thread1, NULL, &reader, buf);
    pthread_create( &reader_thread2, NULL, &reader, buf);
    pthread_join(writer_thread, NULL);
    pthread_join(reader_thread1, NULL);
    pthread_join(reader_thread2, NULL);
    pthread_mutex_destroy(&csv_lock);

    sbuffer_free(&buf);
    printf("buffer freed correctly\n");
    clock_gettime(CLOCK_MONOTONIC, &te);

    printf("Execution time: %lf seconds\n", ((double)te.tv_sec + 1.0e-9*te.tv_nsec) -
                  ((double)ts.tv_sec + 1.0e-9*ts.tv_nsec));
}