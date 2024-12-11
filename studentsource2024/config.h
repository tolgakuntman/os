/**
 * \author {AUTHOR}
 */

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>

#define READ_MSG_LENGTH 100

typedef uint16_t sensor_id_t;
typedef double sensor_value_t;
typedef time_t sensor_ts_t;         // UTC timestamp as returned by time() - notice that the size of time_t is different on 32/64 bit machine

int write_to_log_process(char *msg);

/** Creates child log process and set up a communication pipe to receive data from parent process.
 * \return returns 0 if the log process was created successfully, and -1 otherwise if an error occurred.
 */
int create_log_process();

/** Closes the connection with the log process and terminates it. Also reaps the child process from the process table.
 * \return returns 0 if the log process was terminated successfully, and -1 if an error occurred.
 */
int end_log_process();

typedef struct {
    sensor_id_t id;
    sensor_value_t value;
    sensor_ts_t ts;
} sensor_data_t;

typedef struct {
    void *arg1;
    void *arg2;
    void *arg3;
} arg_t;

#endif /* _CONFIG_H_ */
